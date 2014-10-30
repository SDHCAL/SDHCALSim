#include "LyonRun.hh"
#include "LyonRunActionMessenger.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
//#include "G4THitsMap.hh"
#include "LyonTrackHit.hh"
#include <iostream>
#include <cmath>

//setuper le makefile pur que ca le trouve
//#include "TFile.h"
//#include "TTree.h"
//#include <TRandom.h>
//#include <TMath.h>
//#include <TF1.h>
//for Lcio by rhan
#include "lcio.h"
#include "IO/LCWriter.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCEventImpl.h"
#include "IOIMPL/LCFactory.h"
#include "DHCalEventReader.hh"
#include "IMPL/SimCalorimeterHitImpl.h"

//std::vector< IMPL::MCParticleCont* > vec ;
MCParticleContVec vec;

int _nevt=0;
extern double _Distance;
// for charge distribution of Polya function by rhan
//Double_t Polya(Double_t* x, Double_t* params)
//{
//	// Polya function.
//	//0 = Polya - average charge
//       // 1 = Polya - free parameter
//      //  2 = Polya - amplitude
//	// Parameters:
//	// 0 = average charge (gain)
//	// 1 = free parameter determining the shape of the distribution
//	Double_t q, t;
//	if(*x <= 0)
//		return 0;
//	q = *x / params[0];
//	t = 1.0 + params[1];
//	return params[2]*(pow(t, t) 
//		/ TMath::Gamma(t)
//
//		* TMath::Power(q, params[1])
//			  * TMath::Exp(-t*q));
//}


LyonRun::LyonRun()
{
  //digitiseur=new LyonDigi();
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
 
  G4cout << "LyonRun Print G4SDManager Tree " << G4endl;
  /*r (int i=0; i<100; i++)
    {
  G4cout << "************************************************************ " << G4endl;
  }
  */
  SDMan->ListTree();
  
  _collectionName="LyonTrackHits";
  _collectionID=SDMan->GetCollectionID(_collectionName);
  G4cout << "scorer " << _collectionName <<" : "<<_collectionID<<G4endl;
   
//  _rootFile=new TFile("test.root","RECREATE");
//  _rootTree=new TTree("beamTest","ROOT file simu LYON beam test");
//
//  _rootTree->Branch("evtID", &_evtID, "evtID/I");
//  _rootTree->Branch("nTrack", &_nTrack, "nTrack/I");
//  _rootTree->Branch("replicaNo",&_replicaNo,"replicaNo[nTrack]/I");
//  _rootTree->Branch("x",&_x,"x[nTrack]/F");
//  _rootTree->Branch("y",&_y,"y[nTrack]/F");
//  _rootTree->Branch("z",&_z,"z[nTrack]/F");
//  _rootTree->Branch("dx",&_dx,"Deltax[nTrack]/F");
//  _rootTree->Branch("dy",&_dy,"Deltay[nTrack]/F");
//  _rootTree->Branch("dz",&_dz,"Deltaz[nTrack]/F");
//  _rootTree->Branch("edep",&_edep,"edep[nTrack]/F");
//  _rootTree->Branch("boundaryStatus",&_stepStatus,"boundary[nTrack]/I");
//  _rootTree->Branch("particleID",&_pID,"pID[nTrack]/I");
//  _rootTree->Branch("q",&_charge,"charge[nTrack]/I");
//  _rootTree->Branch("time",&_time,"time[nTrack]/F");
//  //add by rhan
//  _rootTree->Branch("trackID",&_trackid,"trackid[nTrack]/I");
//  _rootTree->Branch("parentID",&_parentid,"parentid[nTrack]/I");
//  _rootTree->Branch("Qinduced",&_InducedCharge,"InducedCharge[nTrack]/F");

  _nTrack=_maxTrack;

  _evtID=0;

  //for LCIO output by rhan-----------------------------------------

 // lcWriter = (IOIMPL::LCFactory::getInstance())->createLCWriter() ;
  //lcWriter->setCompressionLevel(0);
  //lcWriter->open("hantest", LCIO::WRITE_NEW  )
  dher_=new DHCalEventReader();
  dher_->openOutput("test.slcio");
  
   
  
 
  ///////-----------------------------------------------------
}
LyonRun::~LyonRun() 
{
  
  
//  _rootFile->Write();
//  _rootFile->Close();
// 
//  //delete _rootTree;
//  delete _rootFile;
  //delete digitiseur;
     //for lcio by rhan
  dher_->closeOutput();
  
}






void LyonRun::RecordEvent(const G4Event* evt)
{

  G4PrimaryParticle * g4part=0;
  if (evt->GetPrimaryVertex() != 0)
    g4part=evt->GetPrimaryVertex()->GetPrimary();
  if (g4part !=0)
    {
      _mcpart.setPDG(g4part->GetPDGcode());
      G4ThreeVector g4mom=g4part->GetMomentum();
      double mom[3];
      for (int i=0; i<3; i++) mom[i]=g4mom[i];
      _mcpart.setMomentum( mom );
    }
  else 
    {
      _mcpart.setPDG(99);
      double mom[3]={0,0,0};
      _mcpart.setMomentum( mom );
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
  double lengthUnit=mm;
  double energyUnit=eV;
  double timeUnit=ns;
  dher_->processEvent(evt_);
  dher_->createEvent(100,"SD-HCAL"); // NB : changer detector name ?
 
  for (int i=0; i<_nTrack; i++)
    {
      LyonTrackHit* hitP=(*HitVector)[i];
      _replicaNo[i]=hitP->replicaCopyNumber();
      _x[i]=hitP->entrancePoint().x()/lengthUnit;
      _y[i]=hitP->entrancePoint().y()/lengthUnit;
      _z[i]=hitP->entrancePoint().z()/lengthUnit;
      _dx[i]=hitP->deltaPosition().x()/lengthUnit;
      _dy[i]=hitP->deltaPosition().y()/lengthUnit;
      _dz[i]=hitP->deltaPosition().z()/lengthUnit;
      _edep[i]=hitP->energyDeposited()/energyUnit;
      if (hitP->isEntering()) _stepStatus[i]=1;
      if (hitP->isLeaving()) _stepStatus[i]+=2;
      _pID[i]=hitP->pdgID();
      _time[i]=hitP->time()/timeUnit;
      _charge[i]=hitP->charge();
      _trackid[i]=hitP->trackid();//by rhan
      _parentid[i]=hitP->parentid();//by rhan
      //for charge calculation by rhan--------
      

      //TF1 *g1 = new TF1("g1",Polya,0.0,10.0,3);
      //
      ////  g1->SetParameters(1.6,2.0,1);
      //g1->SetParameters(1.6,16.3,1);
      // _InducedCharge[i]=g1->GetRandom();
    
      //------------------------------////////////
       //   std::cout<<"in Lyon RunevtID=="<<_nevt<<"  "<<_x[i]<<" "<<_y[i]<<std::endl;
       //digitiseur->Digitize(_x[i],_y[i],_z[i],_replicaNo[i],_edep[i],_InducedCharge[i],_trackid[i]);
       MCParticleCont* contribution = new MCParticleCont();
       contribution->Particle = &_mcpart;
       contribution->Energy = _edep[i];
       contribution->Time = _time[i];
       contribution->PDG = _pID[i];
       contribution->StepPosition[0] = _x[i] + _dx[i]/2;
       contribution->StepPosition[1] = _y[i] + _dy[i]/2;
       contribution->StepPosition[2] = _z[i] + _dz[i]/2;
       vec.push_back(contribution);
    }
  //fill pad.root
  //digitiseur->FillTree();
  //fill test.root
  //  _rootTree->Fill();
  //write slcio
  dher_->createSimCalorimeterHits(); 

  dher_->writeEvent(_collectionID,numberOfEvent,"SDHCAL_Proto_EndCap");
 
   
  //dher_->processRunHeader(run);
  
  //dher_->createRunHeader(10,"SD-HCAL");
  
  //dher_->writeRunHeader(); 
  

  

  // std::cout<<"dumprun start"<<std::endl;
  
  //dher_->readRun();
  vec.clear();
  //std::cout<<"dumprun end"<<std::endl;
 
  
  
}


