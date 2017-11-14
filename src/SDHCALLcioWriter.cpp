#include "SDHCALLcioWriter.h"

SDHCALLcioWriter::SDHCALLcioWriter(std::string fileName)
{
	writer = lcio::LCFactory::getInstance()->createLCWriter() ;
	writer->setCompressionLevel(2) ;
	writer->open(fileName , EVENT::LCIO::WRITE_NEW) ;
}

SDHCALLcioWriter::~SDHCALLcioWriter()
{
	clear() ;
	writer->close() ;
}

void SDHCALLcioWriter::clear()
{
	if (lcEvent)
		delete lcEvent ;
	//	if (primaryParticle)
	//		delete primaryParticle ;
	primaryParticleMap.clear() ;
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
	lcEvent->addCollection(particleCol , "primaryParticles") ;

	writer->writeEvent(lcEvent) ;
}

void SDHCALLcioWriter::createPrimaryParticles(const G4Event* event)
{
	particleCol = new IMPL::LCCollectionVec(EVENT::LCIO::MCPARTICLE) ;
	for ( int i = 0 ; i < event->GetNumberOfPrimaryVertex() ; ++i )
	{
		const G4PrimaryVertex* vertex = event->GetPrimaryVertex(i) ;
		const G4PrimaryParticle* particle = vertex->GetPrimary() ;

		G4int id = particle->GetTrackID() ;

		IMPL::MCParticleImpl* primaryPart = new IMPL::MCParticleImpl() ;
		primaryPart->setPDG( particle->GetPDGcode() ) ;
		primaryPart->setMass( static_cast<float>( particle->GetMass()/CLHEP::GeV ) ) ;
		primaryPart->setTime( static_cast<float>( vertex->GetT0()/CLHEP::ns ) ) ;
		primaryPart->setCharge( static_cast<float>( particle->GetCharge() ) ) ;

		G4ThreeVector g4mom = particle->GetMomentum()/CLHEP::GeV ;
		double mom[3] ;
		for ( int j : {0,1,2} )
			mom[j] = g4mom[j] ;
		primaryPart->setMomentum( mom ) ;

		G4ThreeVector g4Pos = vertex->GetPosition()/CLHEP::mm ;
		double pos[3] ;
		for ( int j : {0,1,2} )
			pos[j] = g4Pos[j] ;
		primaryPart->setVertex( pos ) ;

		primaryParticleMap.insert( {id , primaryPart} ) ;

		particleCol->addElement(primaryPart) ;
	}
}

void SDHCALLcioWriter::createSimCalorimeterHits(std::vector<SDHCALHit*> hits)
{
	simVec = new IMPL::LCCollectionVec(EVENT::LCIO::SIMCALORIMETERHIT) ;

	IMPL::LCFlagImpl chFlag(0) ;
	EVENT::LCIO bitinfo ;
	chFlag.setBit( bitinfo.CHBIT_LONG ) ;   // sim calorimeter hit position
	chFlag.setBit( bitinfo.CHBIT_ID1 ) ;    // cell ID
	chFlag.setBit( bitinfo.CHBIT_STEP ) ;   // step info
	simVec->setFlag( chFlag.getFlag() ) ;

	UTIL::CellIDEncoder<IMPL::SimCalorimeterHitImpl> IDcoder("M:3,S-1:3,I:9,J:9,K-1:6" , simVec) ;

	//	double lengthUnit = CLHEP::mm ;
	double energyUnit = CLHEP::GeV ;
	double timeUnit = CLHEP::ns ;

	std::map<int,IMPL::SimCalorimeterHitImpl*> hitMap ;
	for ( const auto& hit : hits )
	{
		IMPL::MCParticleCont* step = new IMPL::MCParticleCont() ;
		step->Particle = primaryParticleMap.at( hit->getPrimaryID() ) ;
		step->Energy = static_cast<float>( hit->getEnergyDeposited()/energyUnit ) ;
		step->Time = static_cast<float>( hit->getTime()/timeUnit ) ;
		step->PDG = hit->getPdgID() ;
//		step->Length = static_cast<float>( hit->getTrueLength() ) ;
		step->Length = static_cast<float>( hit->getDeltaPos().mag() ) ;

		int I = hit->getI() ; // + 1 ;
		int J = hit->getJ() ; // + 1 ;
		int K = hit->getRPCID() ;


		//		G4ThreeVector realPos = (*it)->getRPC()->getGlobalCoord(I,J) ;
		//		float cellPos[3] ;
		//		cellPos[0] = static_cast<float>( realPos.x() ) ;
		//		cellPos[1] = static_cast<float>( realPos.y() ) ;
		//		cellPos[2] = static_cast<float>( realPos.z() ) ;

		G4double cellSize = hit->getRPC()->getCellSize() ;
		float cellPos[3] ;
		cellPos[0] = static_cast<float>( (I+1)*cellSize ) ;
		cellPos[1] = static_cast<float>( (J+1)*cellSize ) ;
		cellPos[2] = static_cast<float>( (K+1)*26.131 ) ; //same as TriventProc.cc

		//		G4ThreeVector globalPos = (*it)->getPos() ;

		G4ThreeVector cheatPos( cellPos[0] + hit->getCoordInPad().x() ,
				cellPos[1] + hit->getCoordInPad().y() ,
				cellPos[2] + hit->getCoordInPad().z() ) ;

		//		step->StepPosition[0] = static_cast<float>( globalPos.x() ) ;
		//		step->StepPosition[1] = static_cast<float>( globalPos.y() ) ;
		//		step->StepPosition[2] = static_cast<float>( globalPos.z() ) ;

		step->StepPosition[0] = static_cast<float>( cheatPos.x() ) ;
		step->StepPosition[1] = static_cast<float>( cheatPos.y() ) ;
		step->StepPosition[2] = static_cast<float>( cheatPos.z() ) ;

		int key = (K<<18) + (I<<9) + J ;

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

		hitMap[key]->addMCParticleContribution( step->Particle , step->Energy , step->Time , step->Length , step->PDG , step->StepPosition ) ;
	}

	for ( const auto& hit : hitMap )
		simVec->addElement(hit.second) ;
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

