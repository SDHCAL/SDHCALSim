#include "SDHCALLcioWriter.hpp"
#include "SDHCALHit.hpp"

#include "G4RunManager.hh"
#include "G4Run.hh"

#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/LCFlagImpl.h"
#include "UTIL/CellIDEncoder.h"

G4ThreadLocal IMPL::LCEventImpl* SDHCALLcioWriter::m_LcEvent=nullptr;
G4ThreadLocal std::map<G4int,IMPL::MCParticleImpl*> SDHCALLcioWriter::m_PrimaryParticleMap=std::map<G4int,IMPL::MCParticleImpl*>{};

G4ThreadLocal IMPL::LCCollectionVec* SDHCALLcioWriter::m_ParticleCol=nullptr;
G4ThreadLocal IMPL::LCCollectionVec* SDHCALLcioWriter::m_SimVec=nullptr;

std::unique_ptr<lcio::LCWriter> SDHCALLcioWriter::m_Writer{nullptr};
G4String SDHCALLcioWriter::m_DetectorName="";

void SDHCALLcioWriter::openFile()
{
  m_Writer.reset(lcio::LCFactory::getInstance()->createLCWriter());
  m_Writer->setCompressionLevel(2);
  m_Writer->open(m_FileName, EVENT::LCIO::WRITE_NEW);
}

void SDHCALLcioWriter::closeFile()
{
  clear();
  m_Writer->close();
}

void SDHCALLcioWriter::clear()
{
  m_LcEvent=nullptr;
  m_PrimaryParticleMap.clear();
}

void SDHCALLcioWriter::createLCEvent(const G4Event* event)
{
  m_LcEvent=new IMPL::LCEventImpl();
  m_LcEvent->setRunNumber(G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID());
  m_LcEvent->setEventNumber(event->GetEventID());
  m_LcEvent->setTimeStamp(0);
  m_LcEvent->setDetectorName(m_DetectorName);
}

void SDHCALLcioWriter::writeLCEvent()
{
  m_LcEvent->setWeight(0);
  m_LcEvent->addCollection(m_SimVec,"SDHCAL_Proto_EndCap");
  m_LcEvent->addCollection(m_ParticleCol,"primaryParticles");
  m_Writer->writeEvent(m_LcEvent);
}

void SDHCALLcioWriter::createPrimaryParticles(const G4Event* event)
{
  m_ParticleCol=new IMPL::LCCollectionVec(EVENT::LCIO::MCPARTICLE);
  for(int i=0;i<event->GetNumberOfPrimaryVertex();++i)
  {
    const G4PrimaryVertex* vertex=event->GetPrimaryVertex(i);
    const G4PrimaryParticle* particle=vertex->GetPrimary();
    G4int id=particle->GetTrackID();
    IMPL::MCParticleImpl* primaryPart=new IMPL::MCParticleImpl();
    primaryPart->setPDG(particle->GetPDGcode());
    primaryPart->setMass(static_cast<float>(particle->GetMass()/CLHEP::GeV));
    primaryPart->setTime(static_cast<float>(vertex->GetT0()/CLHEP::ns));
    primaryPart->setCharge(static_cast<float>(particle->GetCharge()));
    G4ThreeVector g4mom=particle->GetMomentum()/CLHEP::GeV;
    double mom[3];
    for(int j:{0,1,2})mom[j]=g4mom[j];
    primaryPart->setMomentum(mom);
    G4ThreeVector g4Pos=vertex->GetPosition()/CLHEP::mm;
    double pos[3];
    for(int j:{0,1,2})pos[j]=g4Pos[j];
    primaryPart->setVertex(pos);
    m_PrimaryParticleMap.insert({id,primaryPart});
    m_ParticleCol->addElement(primaryPart);
  }
}

void SDHCALLcioWriter::createSimCalorimeterHits(std::vector<SDHCALHit*> hits)
{
  m_SimVec=new IMPL::LCCollectionVec(EVENT::LCIO::SIMCALORIMETERHIT);
  IMPL::LCFlagImpl chFlag(0);
  EVENT::LCIO bitinfo;
  chFlag.setBit( bitinfo.CHBIT_LONG);   // sim calorimeter hit position
  chFlag.setBit( bitinfo.CHBIT_ID1);    // cell ID
  chFlag.setBit( bitinfo.CHBIT_STEP);   // step info
  m_SimVec->setFlag( chFlag.getFlag());
  UTIL::CellIDEncoder<IMPL::SimCalorimeterHitImpl> IDcoder("M:3,S-1:3,I:9,J:9,K-1:6",m_SimVec);
  double energyUnit=CLHEP::GeV;
  double timeUnit=CLHEP::ns;
  std::map<int,IMPL::SimCalorimeterHitImpl*> hitMap;
  for ( const auto& hit : hits )
  {
    IMPL::MCParticleCont* step = new IMPL::MCParticleCont();
    step->Particle=m_PrimaryParticleMap.at(hit->getPrimaryID());
    step->Energy=static_cast<float>(hit->getEnergyDeposited()/energyUnit);
    //step->Energy = static_cast<float>( hit->getBetaGamma() ) ;
    step->Time=static_cast<float>(hit->getTime()/timeUnit);
    step->PDG=hit->getPdgID();
    //step->Length = static_cast<float>( hit->getTrueLength() ) ;
    step->Length=static_cast<float>(hit->getDeltaPos().mag());
    int I=hit->getI(); // + 1 ;
    int J=hit->getJ(); // + 1 ;
    int K=hit->getK();
    //G4ThreeVector realPos = (*it)->getRPC()->getGlobalCoord(I,J) ;
    //float cellPos[3] ;
    //cellPos[0] = static_cast<float>( realPos.x() ) ;
    //cellPos[1] = static_cast<float>( realPos.y() ) ;
    //cellPos[2] = static_cast<float>( realPos.z() ) ;
    G4double cellSizeX=hit->getCellSizeX();
    G4double cellSizeY=hit->getCellSizeY();
    float cellPos[3];
    cellPos[0]=static_cast<float>((I+1)*cellSizeX);
    cellPos[1]=static_cast<float>((J+1)*cellSizeY);
    cellPos[2]=static_cast<float>((K+1)*26.131); //same as TriventProc.cc
    //G4ThreeVector globalPos = (*it)->getPos() ;
    G4ThreeVector cheatPos(cellPos[0]+ hit->getCoordInPad().x(),
    cellPos[1]+hit->getCoordInPad().y(),
    cellPos[2]+hit->getCoordInPad().z());
    step->StepPosition[0]=static_cast<float>(cheatPos.x());
    step->StepPosition[1]=static_cast<float>(cheatPos.y());
    step->StepPosition[2]=static_cast<float>(cheatPos.z());
    int key =(K<<18)+(I<<9)+J;
    if ( !hitMap.count(key) )
    {
      hitMap[key]=new IMPL::SimCalorimeterHitImpl();
      IDcoder["M"]=0;
      IDcoder["S-1"]=3; //stave 4
      IDcoder["I"]=I+1; //first is at 1
      IDcoder["J"]=J+1; //first is at 1
      IDcoder["K-1"]=K; //first layer should be K-1=0
      IDcoder.setCellID(hitMap[key]);
      hitMap[key]->setPosition(cellPos);
    }
    hitMap[key]->addMCParticleContribution(step->Particle,step->Energy,step->Time,step->Length,step->PDG,step->StepPosition);
  }
  for(const auto& hit : hitMap)m_SimVec->addElement(hit.second);
}

void SDHCALLcioWriter::setValue(std::string key,int value)
{
  m_LcEvent->parameters().setValue(key,value);
}

void SDHCALLcioWriter::setValue(std::string key,float value)
{
  m_LcEvent->parameters().setValue(key,value);
}

void SDHCALLcioWriter::setValue(std::string key,double value)
{
  m_LcEvent->parameters().setValue(key,static_cast<float>(value));
}

void SDHCALLcioWriter::setValue(std::string key,std::vector<int> values)
{
  m_LcEvent->parameters().setValues(key,values);
}

void SDHCALLcioWriter::setValue(std::string key,std::vector<float> values)
{
  m_LcEvent->parameters().setValues(key,values);
}

void SDHCALLcioWriter::setValue(std::string key,std::vector<double> values)
{
  std::vector<float> floatValues
  {
    static_cast<float>(values[0]),
    static_cast<float>(values[1]),
    static_cast<float>(values[2])
  };
  m_LcEvent->parameters().setValues(key,floatValues);
}

void SDHCALLcioWriter::setValue(std::string key,G4ThreeVector values)
{
  std::vector<float> floatValues
  {
    static_cast<float>(values.x()),
    static_cast<float>(values.y()),
    static_cast<float>(values.z())
  };
  m_LcEvent->parameters().setValues(key,floatValues);
}
