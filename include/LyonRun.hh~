#ifndef LyonRun_h 
#define LyonRun_h 1

#include "globals.hh"
#include "G4Run.hh"

//by rhan for lcio
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/RawCalorimeterHitImpl.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"
#include "EVENT/LCIO.h"
#include "lcio.h"
#include "IO/LCWriter.h"
#include "IMPL/LCRunHeaderImpl.h"
//#include "SD_ReadASCII_LCIO_SlowControl.hh"
//#include "SD_LCIO_RawEvent.hh"
#include "IMPL/LCEventImpl.h"

using namespace lcio;															

//class TFile;
//class TTree;
class DHCalEventReader;
class LyonRunActionMessenger;
class LyonRun : public G4Run
{
public:
  
 
  LyonRun();
  virtual ~LyonRun();

  virtual void RecordEvent(const G4Event*);

   //by rhan--------------------  
 
  //for pad map
 

 

 

private:
  G4String _collectionName;
  G4int _collectionID;
  
private:
  //For Root Tree
//  TFile *_rootFile;
//  TTree *_rootTree;
//  //for LCIO by rhan
  //IO::LCWriter*  lcWriter; 
  LCRunHeader* run;
  IMPL::LCEventImpl* evt_; /// LCIO Event ptr
  DHCalEventReader* dher_;
  //  G4Event* EventBlock;
  //////////
  enum {_maxTrack=100000};
  int _evtID;
  int _nTrack;
  int _replicaNo[_maxTrack];
  float _x[_maxTrack],_y[_maxTrack],_z[_maxTrack];
  float _dx[_maxTrack],_dy[_maxTrack],_dz[_maxTrack];
  float _edep[_maxTrack];
  int _stepStatus[_maxTrack];
  int _pID[_maxTrack];
  int _charge[_maxTrack];
  float _time[_maxTrack];
  int  _trackid[_maxTrack];//by rhan
  int _parentid[_maxTrack];//by rhan
  float _InducedCharge[_maxTrack];//by rhan
  int _PadID[_maxTrack];//by rhan

  MCParticleImpl _mcpart;

};

#endif
