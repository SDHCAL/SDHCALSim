#include "DHCalEventReader.hh"
#include <math.h>
#include <map>

#include <UTIL/CellIDEncoder.h>
#include "IMPL/SimCalorimeterHitImpl.h"
extern MCParticleContVec vec;


extern double _Seuil;
extern int _nPad;

DHCalEventReader::DHCalEventReader()
{
  evt_ = 0;
  runh_ = 0;
  lcReader_=0;
  lcWriter_=0;
  
}

DHCalEventReader::~DHCalEventReader()
{
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



void DHCalEventReader::createSimCalorimeterHits()
{

  UTIL::CellIDEncoder<SimCalorimeterHitImpl> IDcoder("M:3,S-1:3,I:9,J:9,K-1:6",SimVec);


  std::map<int,IMPL::SimCalorimeterHitImpl* > hitMap;
  for (MCParticleContVec::iterator it=vec.begin(); it != vec.end(); it++)
    {
      IMPL::MCParticleCont& step=*(*it);
      int I=int((499.584+step.StepPosition[0])/10.408);
      int J=int((499.584+step.StepPosition[1])/10.408);
      int K=int((627.114+step.StepPosition[2])/26.131);
      //      int K=int((-1.22+651.944+step.StepPosition[2])/26.131);

      if (K>47) {cout << "WARNING K=" <<K<< " position step= " << step.StepPosition[2] << " so " << (-1.22+651.944+step.StepPosition[2])/26.131 << endl; K=47;}

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
      hitMap[key]->addMCParticleContribution( step.Particle,
					      step.Energy,
					      step.Time,
					      step.PDG, 
					      step.StepPosition
					      ) ;
      
    } //end for vec iterator
  
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
  LCTOOLS::dumpEvent(evt_);
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
