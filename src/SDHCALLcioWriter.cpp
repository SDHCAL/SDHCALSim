#include "SDHCALLcioWriter.h"

SDHCALLcioWriter::SDHCALLcioWriter(std::string fileName)
{
	writer = lcio::LCFactory::getInstance()->createLCWriter() ;
	writer->setCompressionLevel(2) ;
	writer->open(fileName , EVENT::LCIO::WRITE_NEW) ;

	lcEvent = NULL ;
	primaryParticle = NULL ;
	simVec = NULL ;
	mcVec = NULL ;
}

SDHCALLcioWriter::~SDHCALLcioWriter()
{
	clear() ;
	writer->close() ;
}

void SDHCALLcioWriter::clear()
{
	//	if (simVec)
	//		delete simVec ;
	//	if (mcVec)
	//		delete mcVec ;
	if (lcEvent)
		delete lcEvent ;
	if (primaryParticle)
		delete primaryParticle ;
}

void SDHCALLcioWriter::createLCEvent(const G4Event* event)
{
	lcEvent = new IMPL::LCEventImpl() ;
	lcEvent->setRunNumber( G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID() ) ;
	lcEvent->setEventNumber( event->GetEventID() ) ;
	lcEvent->setTimeStamp(0) ;
	lcEvent->setDetectorName(detectorName) ;
}

void SDHCALLcioWriter::writeLCEvent()
{
	lcEvent->setWeight(0) ;
	lcEvent->addCollection(simVec , "SDHCAL_Proto_EndCap") ;
	lcEvent->addCollection(mcVec , "particleGenericObject") ;

	//	UTIL::LCTOOLS::dumpEvent(lcEvent) ;
	//LCTOOLS::printLCGenericObjects(mcVec); //for DEBUG
	writer->writeEvent(lcEvent) ;
}

void SDHCALLcioWriter::createPrimaryParticle(const G4Event* event)
{
	primaryParticle = new IMPL::MCParticleImpl() ;

	G4PrimaryParticle* g4part = nullptr ;
	if ( event->GetPrimaryVertex() )
		g4part = event->GetPrimaryVertex()->GetPrimary() ;
	if (g4part)
	{
		primaryParticle->setPDG( g4part->GetPDGcode() ) ;
		G4ThreeVector g4mom = g4part->GetMomentum() ;
		double mom[3] ;
		for (int i = 0 ; i<3 ; i++)
			mom[i] = g4mom[i] ;
		primaryParticle->setMomentum( mom ) ;
	}
	else
	{
		primaryParticle->setPDG(99) ;
		double mom[3] = {0,0,0} ;
		primaryParticle->setMomentum( mom ) ;
	}
}

void SDHCALLcioWriter::createSimCalorimeterHits(std::vector<SDHCALHit*> hits)
{
	simVec = new IMPL::LCCollectionVec(EVENT::LCIO::SIMCALORIMETERHIT) ;
	mcVec = new IMPL::LCCollectionVec(EVENT::LCIO::LCGENERICOBJECT) ;

	IMPL::LCFlagImpl chFlag(0) ;
	EVENT::LCIO bitinfo ;
	chFlag.setBit( bitinfo.CHBIT_LONG ) ;   // sim calorimeter hit position
	//chFlag.setBit( bitinfo.CHBIT_BARREL ) ; // barrel
	chFlag.setBit( bitinfo.CHBIT_ID1 ) ;    // cell ID
	chFlag.setBit( bitinfo.CHBIT_STEP ) ;   // step info
	simVec->setFlag( chFlag.getFlag() ) ;

	UTIL::CellIDEncoder<IMPL::SimCalorimeterHitImpl> IDcoder("M:3,S-1:3,I:9,J:9,K-1:6" , simVec) ;

	//	double lengthUnit = CLHEP::mm ;
	double energyUnit = CLHEP::eV ;
	double timeUnit = CLHEP::ns ;

	std::map<int,IMPL::SimCalorimeterHitImpl*> hitMap ;
	for (std::vector<SDHCALHit*>::iterator it = hits.begin() ; it != hits.end() ; it++)
	{
		IMPL::MCParticleCont* step = new IMPL::MCParticleCont() ;
		step->Particle = primaryParticle ;
		step->Energy = static_cast<float>( (*it)->getEnergyDeposited()/energyUnit ) ;
		step->Time = static_cast<float>( (*it)->getTime()/timeUnit ) ;
		step->PDG = (*it)->getPdgID() ;

		int I = (*it)->getI() ; // + 1 ;
		int J = (*it)->getJ() ; // + 1 ;
		int K = (*it)->getRPCID() ;


//		G4ThreeVector realPos = (*it)->getRPC()->getGlobalCoord(I,J) ;
//		float cellPos[3] ;
//		cellPos[0] = static_cast<float>( realPos.x() ) ;
//		cellPos[1] = static_cast<float>( realPos.y() ) ;
//		cellPos[2] = static_cast<float>( realPos.z() ) ;

		G4double cellSize = (*it)->getRPC()->getCellSize() ;
		float cellPos[3] ;
		cellPos[0] = static_cast<float>( (I+1)*cellSize ) ;
		cellPos[1] = static_cast<float>( (J+1)*cellSize ) ;
		cellPos[2] = static_cast<float>( (K+1)*26.131 ) ; //same as TriventProc.cc

//		G4ThreeVector globalPos = (*it)->getPos() ;

		G4ThreeVector cheatPos( cellPos[0] + (*it)->getCoordInPad().x() ,
								cellPos[1] + (*it)->getCoordInPad().y() ,
								cellPos[2] + (*it)->getCoordInPad().z() ) ;

//		step->StepPosition[0] = static_cast<float>( globalPos.x() ) ;
//		step->StepPosition[1] = static_cast<float>( globalPos.y() ) ;
//		step->StepPosition[2] = static_cast<float>( globalPos.z() ) ;

		step->StepPosition[0] = static_cast<float>( cheatPos.x() ) ;
		step->StepPosition[1] = static_cast<float>( cheatPos.y() ) ;
		step->StepPosition[2] = static_cast<float>( cheatPos.z() ) ;

//		int key = 100*100*K + 100*J + I ;
		int key = (I<<15) + (J<<6) + K ;

		if ( !hitMap.count(key) )
		{
			hitMap[key] = new IMPL::SimCalorimeterHitImpl() ;
			IDcoder["M"] = 0 ;
			IDcoder["S-1"] = 3 ; //stave 4
			IDcoder["I"] = I+1 ; //first is at 1
			IDcoder["J"] = J+1 ; //first is at 1
			IDcoder["K-1"] = K ; //first layer should be K-1=0
			IDcoder.setCellID( hitMap[key] ) ;

			hitMap[key]->setPosition(cellPos) ;
		}

//		G4cout << "pdg : " << step->Particle->getPDG() << G4endl ;
		hitMap[key]->addMCParticleContribution( step->Particle , step->Energy , step->Time , step->PDG , step->StepPosition ) ;

//		G4ThreeVector globalBeginPos = (*it)->getBeginPos() ;
//		G4ThreeVector globalEndPos = (*it)->getEndPos() ;

//		G4ThreeVector deltaInLocalCoord = (*it)->getRPC()->globalToRpcCoordTransform( (*it)->getDeltaPos() ) ;

		G4ThreeVector cheatBeginPos = cheatPos - 0.5*(*it)->getDeltaPos() ;
		G4ThreeVector cheatEndPos = cheatPos + 0.5*(*it)->getDeltaPos() ;

		IMPL::LCGenericObjectImpl* particle = new IMPL::LCGenericObjectImpl() ;
		particle->setIntVal(0 , hitMap[key]->getCellID0() ) ;
		particle->setIntVal(1 , hitMap[key]->getNMCContributions() ) ;
		particle->setIntVal(2 , (*it)->getTrackStatus() ) ;

//		particle->setFloatVal(0 , static_cast<float>( globalBeginPos.x() ) ) ;
//		particle->setFloatVal(1 , static_cast<float>( globalBeginPos.y() ) ) ;
//		particle->setFloatVal(2 , static_cast<float>( globalBeginPos.z() ) ) ;
//		particle->setFloatVal(3 , static_cast<float>( globalEndPos.x() ) ) ;
//		particle->setFloatVal(4 , static_cast<float>( globalEndPos.y() ) ) ;
//		particle->setFloatVal(5 , static_cast<float>( globalEndPos.z() ) ) ;

		particle->setFloatVal(0 , static_cast<float>( cheatBeginPos.x() ) ) ;
		particle->setFloatVal(1 , static_cast<float>( cheatBeginPos.y() ) ) ;
		particle->setFloatVal(2 , static_cast<float>( cheatBeginPos.z() ) ) ;
		particle->setFloatVal(3 , static_cast<float>( cheatEndPos.x() ) ) ;
		particle->setFloatVal(4 , static_cast<float>( cheatEndPos.y() ) ) ;
		particle->setFloatVal(5 , static_cast<float>( cheatEndPos.z() ) ) ;

		particle->setFloatVal(6 , static_cast<float>( (*it)->getTrueLength() ) ) ;

		mcVec->addElement(particle) ;
	}

	for (std::map<int,IMPL::SimCalorimeterHitImpl*>::iterator itmap = hitMap.begin() ; itmap != hitMap.end() ; itmap++)
		simVec->addElement(itmap->second) ;

}

void SDHCALLcioWriter::setValue(std::string key , int value)
{
	lcEvent->parameters().setValue( key , value ) ;
}
void SDHCALLcioWriter::setValue(std::string key , float value)
{
	lcEvent->parameters().setValue( key , value ) ;
}
void SDHCALLcioWriter::setValue(std::string key , double value)
{
	lcEvent->parameters().setValue( key , static_cast<float>( value ) ) ;
}
void SDHCALLcioWriter::setValue(std::string key , std::vector<int> values)
{
	lcEvent->parameters().setValues( key , values ) ;
}
void SDHCALLcioWriter::setValue(std::string key , std::vector<float> values)
{
	lcEvent->parameters().setValues( key , values ) ;
}
void SDHCALLcioWriter::setValue(std::string key , std::vector<double> values)
{
	std::vector<float> floatValues ;

	for ( std::vector<double>::const_iterator it = values.begin() ; it != values.end() ; ++it )
		floatValues.push_back( static_cast<float>( *it ) ) ;

	lcEvent->parameters().setValues( key , floatValues ) ;
}
void SDHCALLcioWriter::setValue(std::string key , G4ThreeVector values)
{
	std::vector<float> floatValues ;
	floatValues.push_back( static_cast<float>( values.x() ) ) ;
	floatValues.push_back( static_cast<float>( values.y() ) ) ;
	floatValues.push_back( static_cast<float>( values.z() ) ) ;

	lcEvent->parameters().setValues( key , floatValues ) ;
}

