#include "SDHCALEventAction.hpp"

#include "G4RunManager.hh"

#include "SDHCALRunAction.hpp"
#include "SDHCALSteppingAction.hpp"
#include "SDHCALTrackingAction.hpp"
#include "SDHCALStackingAction.hpp"
#include "SDHCALPrimaryGeneratorAction.hpp"
#include "SDHCALHit.hpp"

SDHCALEventAction::SDHCALEventAction(SDHCALRunAction* runAction,SDHCALSteppingAction* steppingAction,SDHCALStackingAction* stackingAction,SDHCALTrackingAction* trackingAction,SDHCALPrimaryGeneratorAction* primaryGeneratorAction ):G4UserEventAction(),m_RunAction(runAction),m_SteppingAction(steppingAction),m_StackingAction(stackingAction),m_TrackingAction(trackingAction),m_PrimaryGeneratorAction(primaryGeneratorAction)
{
}


void SDHCALEventAction::BeginOfEventAction(const G4Event* event)
{
  beginClock = clock() ;
  m_SteppingAction->reset();
  m_TrackingAction->reset();
  m_StackingAction->reset();
}

void SDHCALEventAction::EndOfEventAction(const G4Event* event)
{
  G4AutoLock	l(&aMutex);
	G4cout << "-------------------------------------------------------" << G4endl ;
	G4cout << "Event " << event->GetEventID() << G4endl ;
  SDHCALLcioWriter* lcioWriter = m_RunAction->getLcioWriter() ;
  lcioWriter->clear() ;
  lcioWriter->createLCEvent(event) ;
	SDHCALRootWriter* rootWriter = m_RunAction->getRootWriter() ;

	std::vector<SDHCALHit*> hits ;
   //std::cout<< event->GetHCofThisEvent()->GetNumberOfCollections() <<std::endl;
	for (int i = 0 ; i < event->GetHCofThisEvent()->GetNumberOfCollections() ; i++ )
	{
		SDHCALHitCollection* hCol = dynamic_cast<SDHCALHitCollection*>( event->GetHCofThisEvent()->GetHC(i) ) ;
		std::vector<SDHCALHit*> hitVec = *( hCol->GetVector() ) ;
		hits.insert(hits.end() , hitVec.begin() , hitVec.end() ) ;
    
	}

	for(std::size_t i=0;i!=hits.size();++i)
  {
    hits[i]->setPrimaryID(m_TrackingAction->getPrimaryParent(hits[i]->getTrackID()));
  }
	
	lcioWriter->createPrimaryParticles(event) ;

	lcioWriter->createSimCalorimeterHits(hits) ;

	std::shared_ptr<SDHCALGun> gun = m_PrimaryGeneratorAction->getGunVec().at(0) ;

	G4double primaryEnergy = gun->GetParticleEnergy()/CLHEP::GeV ;
	G4ThreeVector primaryMom = gun->GetParticleMomentumDirection() ;
	G4ThreeVector primaryPos = gun->GetParticlePosition() ;
	G4int particleID = gun->GetParticleDefinition()->GetPDGEncoding() ;

	lcioWriter->setValue("ParticleID" , particleID) ;
	lcioWriter->setValue("ParticleMomentum" , primaryMom) ;
	lcioWriter->setValue("ParticlePosition" , primaryPos) ;
	lcioWriter->setValue("ParticleEnergy" , primaryEnergy) ;


	m_PrimaryGeneratorAction->print() ;
	G4cout << G4endl ;

	m_SteppingAction->processSteps() ;

	G4double depositedEnergy = m_SteppingAction->getDepositedEnergy()/CLHEP::GeV ;
	G4double leakedEnergy = m_SteppingAction->getLeakedEnergy()/CLHEP::GeV ;
	G4double emFraction = m_SteppingAction->getEMFraction() ;
	G4double depositedEnergyNeutron = m_SteppingAction->getDepositedEnergyPerParticleType()[2112]/CLHEP::GeV ;

	G4cout << "Deposited energy : " << depositedEnergy << " GeV" << G4endl ;
	G4cout << "Leaked energy : " << leakedEnergy << " GeV" << G4endl ;
	G4cout << "EM Fraction : " << 100*emFraction << " %" << G4endl ;
	G4cout << G4endl ;

	lcioWriter->setValue("DepositedEnergy" , depositedEnergy ) ;
	lcioWriter->setValue("DepositedEnergyNeutron" , depositedEnergyNeutron ) ;
	lcioWriter->setValue("LeakedEnergy" , leakedEnergy ) ;
	lcioWriter->setValue("EMFraction" , emFraction ) ;

	const auto& nParticlesPerIDMap = m_StackingAction->getNumberOfParticlesPerID() ;
	G4int nNeutrons = 0 ;
	G4int nPi0 = 0 ;

	if ( nParticlesPerIDMap.count(2112) )
		nNeutrons = nParticlesPerIDMap.at(2112) ;
	if ( nParticlesPerIDMap.count(111) )
		nPi0 = nParticlesPerIDMap.at(111) ;

	lcioWriter->setValue("nNeutrons" , nNeutrons ) ;
	lcioWriter->setValue("nPi0" , nPi0 ) ;



	lcioWriter->writeLCEvent() ;



	int nRealHits = 0 ;
	for ( std::vector<SDHCALHit*>::const_iterator it = hits.begin() ; it != hits.end() ; ++it )
	{
		if ( ( (*it)->getEndPos() - (*it)->getBeginPos() ).mag() > 0 )
			nRealHits++ ;
	}

	//processing time
	double timeOfThisEvent = 1.0*( clock() - beginClock )/CLOCKS_PER_SEC ;
	G4cout << "computing time : " << timeOfThisEvent << G4endl ;

	averageTime = (averageTime*nEventsProcessed + timeOfThisEvent)/(nEventsProcessed+1) ;
	nEventsProcessed++ ;
	G4cout << "average time : " << averageTime << G4endl ;


	rootWriter->setEventNumber( event->GetEventID() ) ;
	rootWriter->setNHit( nRealHits ) ;
	rootWriter->setPrimaryID( particleID ) ;
	rootWriter->setPrimaryEnergy( primaryEnergy ) ;
	rootWriter->setPrimaryPos( primaryPos ) ;
	rootWriter->setPrimaryMom( primaryMom ) ;
	rootWriter->setDepositedEnergy( depositedEnergy ) ;
	rootWriter->setDepositedEnergyNeutron( depositedEnergyNeutron ) ;
	rootWriter->setNNeutrons( nNeutrons ) ;
	rootWriter->setNPi0( nPi0 ) ;
	rootWriter->setLeakedEnergy( leakedEnergy ) ;
	rootWriter->setEmFraction( emFraction ) ;
	rootWriter->setComputingTime( timeOfThisEvent ) ;

	std::vector<double> stepCosAngle ;
	std::vector<double> stepLength ;
	std::vector<double> stepTime ;
	for ( std::vector<SDHCALHit*>::const_iterator it = hits.begin() ; it != hits.end() ; ++it )
	{
		stepCosAngle.push_back( std::cos( (*it)->getDeltaPos().angle( (*it)->getRPC()->rpcToGlobalCoordTransform( G4ThreeVector(0,0,1) ) ) ) ) ;
		stepLength.push_back( (*it)->getTrueLength() ) ;
		stepTime.push_back( (*it)->getTime() ) ;
	}
	
	rootWriter->setStepCosAngle( stepCosAngle ) ;
	rootWriter->setStepLength( stepLength ) ;
	rootWriter->setStepTime( stepTime ) ;
	rootWriter->fillTree() ;
}
