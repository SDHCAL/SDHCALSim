#include "DHCalEventReader.hh"
#include <math.h>
#include <map>

#include <UTIL/CellIDEncoder.h>
#include "globals.hh"


extern double _Seuil;
extern int _nPad;

DHCalEventReader::DHCalEventReader()
{
  evt_ = 0;
  runh_ = 0;
  lcReader_=0;
  lcWriter_=0;
  _step=new MCParticleCont();
}

DHCalEventReader::~DHCalEventReader()
{
  delete _step;
  delete _particle;
  clear();
}

void DHCalEventReader::readRun()
{

  LCTOOLS::dumpRunHeader(runh_);
  
}

void DHCalEventReader::processRunHeader( LCRunHeader* run) 
{
   
   
  runh_ =(IMPL::LCRunHeaderImpl*) run;
} 

  //! Interface for readStream
void DHCalEventReader::processEvent( LCEvent * evt )  
  {
   
    evt_ = (IMPL::LCEventImpl*) evt;
    SimVec=new IMPL::LCCollectionVec(LCIO::SIMCALORIMETERHIT) ;
    mcVec=new IMPL::LCCollectionVec(LCIO::LCGENERICOBJECT) ;

    IMPL::LCFlagImpl chFlag(0) ;
    EVENT::LCIO bitinfo;
    chFlag.setBit(bitinfo.CHBIT_LONG ) ;   // sim calorimeter hit position
    //chFlag.setBit(bitinfo.CHBIT_BARREL ) ; // barrel
    chFlag.setBit(bitinfo.CHBIT_ID1 ) ;    // cell ID 
    chFlag.setBit(bitinfo.CHBIT_STEP ) ;   // step info
    SimVec->setFlag( chFlag.getFlag()  ) ;   


  }



void DHCalEventReader::clear()
{

}


int IJKtoKey(int I, int J, int K){return 100*100*K+100*J+I;}
int KeytoI(int key) {return key%100;}
int KeytoJ(int key) {return (key/100)%100;}
int KeytoK(int key) {return key/100/100;}



void DHCalEventReader::createSimCalorimeterHits(std::vector<LyonTrackHit*> lyonTrackHitVec,IMPL::MCParticleImpl *part)
{

  UTIL::CellIDEncoder<SimCalorimeterHitImpl> IDcoder("M:3,S-1:3,I:9,J:9,K-1:6",SimVec);
  double lengthUnit=mm;
  double energyUnit=eV;
  double timeUnit=ns;


  std::map<int,IMPL::SimCalorimeterHitImpl* > hitMap;
  for (std::vector<LyonTrackHit*>::iterator it=lyonTrackHitVec.begin(); 
       it != lyonTrackHitVec.end(); it++)
    {
      
      _step->Particle = part;
      _step->Energy = (*it)->energyDeposited()/energyUnit;
      _step->Time = (*it)->time()/timeUnit;;
      _step->PDG = (*it)->pdgID();
      _step->StepPosition[0] = (*it)->entrancePoint().x()/lengthUnit + (*it)->deltaPosition().x()/2/lengthUnit;
      _step->StepPosition[1] = (*it)->entrancePoint().y()/lengthUnit + (*it)->deltaPosition().y()/2/lengthUnit;
      _step->StepPosition[2] = (*it)->entrancePoint().z()/lengthUnit + (*it)->deltaPosition().z()/2/lengthUnit;
      
      int I=int((499.584+_step->StepPosition[0])/10.408);
      int J=int((499.584+_step->StepPosition[1])/10.408);
      int K=int((627.114+_step->StepPosition[2])/26.131);
      //      int K=int((-1.22+651.944+_step.StepPosition[2])/26.131);

      if (K>47) {cout << "WARNING K=" <<K<< " position _step= " << _step->StepPosition[2] << " so " << (-1.22+651.944+_step->StepPosition[2])/26.131 << endl; K=47;}

      int key=IJKtoKey(I,J,K);
      if (hitMap.count(key)==0)
	{
	  hitMap[key]=new IMPL::SimCalorimeterHitImpl();
	  IDcoder["M"]=0;
	  IDcoder["S-1"]=3; //stave 4
	  IDcoder["I"]=I; //first is at 0
	  IDcoder["J"]=J; //first is at 0
          IDcoder["K-1"]=K; //first layer should be K-1=0
	  IDcoder.setCellID( hitMap[key] );
	  //should set the position
	  float pos[3]; //la position est le centre de la cellule dans les 3 directions
	  pos[0]=I*10.408 + 5.204 - 499.584;
	  pos[1]=J*10.408 + 5.204 - 499.584;
	  pos[2]=(K)*26.131 - 625.213;
	  //	  pos[2]=(K+1)*26.131 - 651.944 + 0.6;
	  //on retranslate les positions
	  hitMap[key]->setPosition(pos);
	}
      hitMap[key]->addMCParticleContribution( _step->Particle,
					      _step->Energy,
					      _step->Time,
					      _step->PDG, 
					      _step->StepPosition
					      ) ;

      _particle=new LCGenericObjectImpl();
      _particle->setIntVal(0,hitMap[key]->getCellID0());
      _particle->setIntVal(1,hitMap[key]->getNMCContributions());
      _particle->setFloatVal(0,(*it)->entrancePoint().x()/lengthUnit);
      _particle->setFloatVal(1,(*it)->entrancePoint().y()/lengthUnit);
      _particle->setFloatVal(2,(*it)->entrancePoint().z()/lengthUnit);
      _particle->setFloatVal(3,(*it)->exitPoint().x()/lengthUnit);
      _particle->setFloatVal(4,(*it)->exitPoint().y()/lengthUnit);
      _particle->setFloatVal(5,(*it)->exitPoint().z()/lengthUnit);

      mcVec->addElement(_particle);
      
    } 
  
  for (std::map<int,IMPL::SimCalorimeterHitImpl* >::iterator itmap=hitMap.begin();
       itmap != hitMap.end(); itmap++)
    SimVec->addElement(itmap->second);

}


void DHCalEventReader::openOutput(std::string filename)
{
  lcWriter_ = new LCSplitWriter( LCFactory::getInstance()->createLCWriter() , 2040109465 ) ;
  lcWriter_->setCompressionLevel(2);
  lcWriter_->open(filename) ;
}


void DHCalEventReader::writeEvent(int runNum,int evtNum,std::string srhcol)
{
  std::cout<<"Event written" <<std::endl;   
  evt_->setRunNumber(runNum);
  evt_->setEventNumber(evtNum);
  evt_->setTimeStamp(0);
  evt_->setDetectorName("SimuLyonPrototype");
  evt_->setWeight(0);
  evt_->addCollection(SimVec,srhcol);
  evt_->addCollection(mcVec,"particleGenericObject");
  LCTOOLS::dumpEvent(evt_);
  //LCTOOLS::printLCGenericObjects(mcVec); //for DEBUG
  lcWriter_->writeEvent(evt_);
}




void DHCalEventReader::closeOutput()
{
  lcWriter_->close();
}


void DHCalEventReader::createEvent(int nrun,std::string detname)
{
  // Clear DIF and Frames
  this->clear();
  // Create the event
  int lastevt=0;
  if (evt_!=0) {
    lastevt=evt_->getEventNumber();
    delete evt_;
  }
  evt_ =  new IMPL::LCEventImpl();
  evt_->setRunNumber(nrun);
  evt_->setEventNumber(lastevt+1);
  evt_->setTimeStamp(0);
  evt_->setDetectorName(detname);

}
