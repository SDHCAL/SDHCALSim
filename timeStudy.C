#define timeStudy_cxx
#include "timeStudy.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
//#include <vector>

//#include "UTIL/CellIDDecoder.h"





void timeStudy::MissedHitIdentifier(vector<HitPhaseSpace*>& HPS, vector<HitDataRecords*>& HDRs, int& MultiContriCounter2)
{
  for(int iChambre=0; iChambre < Nchambers; iChambre++)
    {      
      int nHits = HPS[iChambre]->HitPhaseCollection.size();
      for(vector<int> ::iterator iter = HPS[iChambre]->MuonID.begin(); iter != HPS[iChambre]->MuonID.end(); ++iter)
	{
	  HitPhase* MuonPhase = HPS[iChambre]->HitPhaseCollection[(*iter)];	  
	  float minT=1e9;
	  for(int i = 0; i<nHits; i++)
	    {
	      if(i!=(*iter))
		{
		  HitPhase* AnotherPhase = HPS[iChambre]->HitPhaseCollection[i];
		  if(AnotherPhase->HitID == MuonPhase->HitID)
		    {
		      MultiContriCounter2++;
		      // cout<<"delta t ="<< (MuonPhase->HitTime)-(AnotherPhase->HitTime)<<";  Dis = "<<AnotherPhase->distanceXY(*MuonPhase)<<endl;  
		    }
		  if (AnotherPhase->distanceXY(*MuonPhase) < RadiusAvalancheBlindness ) //in mm
		    {		     
		      float t = (MuonPhase->HitTime)-(AnotherPhase->HitTime);
		      
		      if (t>0 && t<minT)
			{
			  minT = t;			   
			}
		    }
		}
	    }
	  // cout << "min T = " << minT << endl;
	  for(int it = 0; it<EffiSize; it++)
	    {
	      float Dtime = it*TimeStep;
	      if(minT<Dtime)
		{
		  HDRs[iChambre]->nMissedHit[it]++;
		  // cout<<"Dead time = "<<Dtime<<"ns; A hit missed, deta t = "<< minT <<"; chambre: "<<iChambre<<endl;		  
		}
	    }
	  HDRs[iChambre]->minTimeVec.push_back(minT);
	}
    }
}

void timeStudy::Loop()
{
  //   In a ROOT session, you can do:
  //      Root > .L timeStudy.C
  //      Root > timeStudy t
  //      Root > t.GetEntry(12); // Fill t data members with entry number 12
  //      Root > t.Show();       // Show values of entry 12
  //      Root > t.Show(16);     // Read and show values of entry 16
  //      Root > t.Loop();       // Loop on all entries
  //

  //     This is the loop skeleton where:
  //    jentry is the global entry number in the chain
  //    ientry is the entry number in the current Tree
  //  Note that the argument to GetEntry must be:
  //    jentry for TChain::GetEntry
  //    ientry for TTree::GetEntry and TBranch::GetEntry
  //
  //       To read only selected branches, Insert statements like:
  // METHOD1:
  //    fChain->SetBranchStatus("*",0);  // disable all branches
  //    fChain->SetBranchStatus("branchname",1);  // activate branchname
  // METHOD2: replace line
  //    fChain->GetEntry(jentry);       //read all branches
  //by  b_branchname->GetEntry(ientry); //read only this branch
  if (fChain == 0) return;

  //   nMuonHitHisto=new TH1I("nmuonhits","Nombre de hits dus aux muons",20,0,20);
  //muonHitTimeHisto=new TH1F("muonhitstime","temps des hits dus aux muons (ns)",1000,0,1e6);

  // minTimeHisto_P = new vector<TH1F*>(7, (new TH1F("minTime","Minimum time between a hit and a muon hit(ns)", 1000, 0, 1e6)));
  //vector<TH1F*>& minTimeHisto = *minTimeHisto_P;
  
  //UTIL::CellIDDecoder<SimCalorimeterHit> cd("M:3,S-1:3,I:9,J:9,K-1:6");
   
  //vector<IMPL::SimCalorimeterHitImpl*> *SDHCAL_Proto_EndCap;
  // int SameHitCounter = 0;

  Long64_t nentries = fChain->GetEntriesFast();  
  Long64_t nbytes = 0, nb = 0;
  int MultiContriCounter = 0;
  int MultiContriCounter2 = 0;
 

  vector<HitDataRecords*>* HDRs_P = new vector<HitDataRecords*>;
  //(Nchambers=7, new HitDataRecords);
  vector<HitDataRecords*> & HDRs = *HDRs_P;
   for(int i=0; i<Nchambers; i++) HDRs.push_back(new HitDataRecords);
     
  
  for (Long64_t jentry=0; jentry<nentries;jentry++)
    {
      vector<HitPhaseSpace*>* HPS_P = new vector<HitPhaseSpace*>;
      //(Nchambers=7, new HitPhaseSpace);
      vector<HitPhaseSpace*>& HPS = *HPS_P;
      for(int i=0; i<Nchambers; i++) HPS.push_back(new HitPhaseSpace);
      
      
       
      // int nMuonHit=0;
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;      
      int nHits=SDHCAL_Proto_EndCap->size();
      std::vector<IMPL::SimCalorimeterHitImpl*> &hitvec=*SDHCAL_Proto_EndCap;
      for (int ihit=0; ihit<nHits; ++ihit)
	{
	  IMPL::SimCalorimeterHitImpl* hit=hitvec[ihit];
	  int iChambre=( ( (hit->getCellID0())>>24)&63);
	  //cout << ( ( (hit->getCellID0())>>24)&63) << " : ";
	  //cout << "chambre " << cd(hit)["K-1"] << " a z=" << hit->getPosition()[2] << endl;
	  // cout << "chambre " << numeroChambre << " a z=" << hit->getPosition()[2] << endl;
	  //cout << "test " << hit->_vec.size() << endl;
	  //cout << "PDG : ";
	  int ct = 0, cm=0;
	  for (IMPL::MCParticleContVec::iterator it=hit->_vec.begin(); it!=hit->_vec.end(); ++it)
	    {
	      ct++;
	      HitPhase* aPointer = new HitPhase;
	      aPointer->HitTime = (*it)->Time;
	      aPointer->HitPosition[0] = (*it)->StepPosition[0];
	      aPointer->HitPosition[1] = (*it)->StepPosition[1];
	      aPointer->HitPosition[2] = (*it)->StepPosition[2];
	      aPointer->HitID = ihit;
	      HPS[iChambre]->HitPhaseCollection.push_back(aPointer);
	      if ((*it)->PDG == -13)
		{
		  cm++;
		  HPS[iChambre]->MuonID.push_back(HPS[iChambre]->HitPhaseCollection.size()-1);
		  // cout<<"muon hit time: "<<(*it)->Time<<"at chambre "<<iChambre<<endl;
		  HDRs[iChambre]->nTotalMuonHit++;
		  // ++nMuonHit;
		  //		       muonHitTimeHisto->Fill( (*it)->Time );
		  //		       muonHitTime = (*it)->Time;
		  //		       muonHitPosition = (*it)->StepPosition;		      
		}		  
	    }
	  if( cm>0 && ct>1 )
	    {
	      //cout<<"ct = "<<ct<<";   cm = "<<cm<<endl;
	      MultiContriCounter+=(ct-cm);
	    }
	  //cout << endl;
	    
	}
      MissedHitIdentifier(HPS,  HDRs, MultiContriCounter2);

      for(int i =0; i<Nchambers ; i++) 	delete HPS[i];

      delete HPS_P;
      
      
      //nMuonHitHisto->Fill(nMuonHit);
      //cout << " nombre de hits muons " << nMuonHit << endl;
    }
  for(int iChambre = 0; iChambre<Nchambers; iChambre++)     
    {
      cout<<"Number of total muon hits(including those missed) for Chambre "<<iChambre<<" is: "<<HDRs[iChambre]->nTotalMuonHit<<endl;
      //cout<<"Number of missed hits for Chambre "<<iChambre<<" is: "<<HDRs[iChambre]->nMissedHit<<endl;
      //cout<<"Those missed hits are at time: "<<endl;
       for(vector<float>:: iterator iter = HDRs[iChambre]->minTimeVec.begin(); iter != HDRs[iChambre]->minTimeVec.end(); iter++)
	{
	  minTimeHisto[iChambre]->Fill(  (*iter)  );
	  // if((*iter)<50) cout<< (*iter)<<endl;
	}
      // float t1 = float(nMissedHit);
      // float t2 = float(MuonID->size());
      //float fT = HDRs[iChambre]->nTotalMuonHit;
       // float fM = HDRs[iChambre]->nMissedHit;
       // cout<<"L'effecacité de la chambre "<<iChambre<<" est: "<<(fT-fM)/fT<<endl;
       float nT = HDRs[iChambre]->nTotalMuonHit;
       for(int i=0; i<EffiSize; i++)
	 {
	   float nM = HDRs[iChambre]->nMissedHit[i];	   
	   EffiByTime[iChambre][i] = (nT-nM)/nT;
	 }
    }
  cout<<"multi-contributors number 1(excluded muons) "<<MultiContriCounter<<endl;
  cout<<"multi-contributors number 2(excluded muons) "<<MultiContriCounter2<<endl;
  for(int i =0; i<Nchambers ; i++) delete HDRs[i];
  delete HDRs_P;
    
}

