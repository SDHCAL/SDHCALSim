#include "LyonRun.hh"
#include "LyonRunActionMessenger.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "LyonTrackHit.hh"
#include <iostream>
#include <cmath>

#include "lcio.h"
#include "IO/LCWriter.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCEventImpl.h"
#include "IOIMPL/LCFactory.h"
#include "DHCalEventReader.hh"
#include "IMPL/SimCalorimeterHitImpl.h"


int _nevt=0;

LyonRun::LyonRun()
{
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
 
  G4cout << "LyonRun Print G4SDManager Tree " << G4endl;
  SDMan->ListTree();
  
  _collectionName="LyonTrackHits";
  _collectionID=SDMan->GetCollectionID(_collectionName);
  G4cout << "scorer " << _collectionName <<" : "<<_collectionID<<G4endl;
   

  _nTrack=_maxTrack;

  _evtID=0;

  dher_=new DHCalEventReader();
  dher_->openOutput("test.slcio");
  
   
  
 
  ///////-----------------------------------------------------
}
LyonRun::~LyonRun() 
{
  delete _mcpart;
  dher_->closeOutput();
  delete dher_;  
}






void LyonRun::RecordEvent(const G4Event* evt)
{
  _mcpart=new IMPL::MCParticleImpl();
  G4PrimaryParticle * g4part=0;
  if (evt->GetPrimaryVertex() != 0)
    g4part=evt->GetPrimaryVertex()->GetPrimary();
  if (g4part !=0)
    {
      _mcpart->setPDG(g4part->GetPDGcode());
      G4ThreeVector g4mom=g4part->GetMomentum();
      double mom[3];
      for (int i=0; i<3; i++) mom[i]=g4mom[i];
      _mcpart->setMomentum( mom );
    }
  else 
    {
      _mcpart->setPDG(99);
      double mom[3]={0,0,0};
      _mcpart->setMomentum( mom );
    }
  _evtID+=1;

  for (int i=0; i<_nTrack; i++)
    {
      _replicaNo[i]=0; _x[i]=0; _y[i]=0; _z[i]=0; _edep[i]=0;
      _dx[i]=0; _dy[i]=0; _dz[i]=0;
      _stepStatus[i]=0;_pID[i]=0;_time[i]=0;_charge[i]=0;_trackid[i]=0;
      _parentid[i]=0;_InducedCharge[i]=0;//by rhan
    }
  
 
  

  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  if(!HCE) return;
  numberOfEvent++;
  _nevt=numberOfEvent;
  

  LyonTrackHitsCollection *HitColl=(LyonTrackHitsCollection*) HCE->GetHC(_collectionID);
  //G4cout << "Found collection of size " << HitColl->GetSize() << G4endl;
  _nTrack=HitColl->GetSize();
  std::vector<LyonTrackHit*> *HitVector=HitColl->GetVector();

  if (_nTrack > _maxTrack) _nTrack=_maxTrack;
  dher_->processEvent(evt_);
  dher_->createEvent(100,"SD-HCAL"); // NB : changer detector name ?
 
  std::vector<LyonTrackHit*> lyonTrackHitVec;

  for (int i=0; i<_nTrack; i++)
    {
      LyonTrackHit* hitP=(*HitVector)[i];
      lyonTrackHitVec.push_back(hitP);
    }
  dher_->createSimCalorimeterHits(lyonTrackHitVec,_mcpart); 

  dher_->writeEvent(_collectionID,numberOfEvent,"SDHCAL_Proto_EndCap");
 
   
  lyonTrackHitVec.clear();
}


