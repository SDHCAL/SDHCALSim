//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jun 29 17:23:17 2016 by ROOT version 5.34/14
// from TTree LCIO/lcio event data tree
// found on file: lcioTree.root
//////////////////////////////////////////////////////////

#ifndef timeStudy_h
#define timeStudy_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1I.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <string>

//LCIO header file
#include "IMPL/SimCalorimeterHitImpl.h"


const static int EffiSize = 10000;
//const static float TimeStep = 500.0;
//const static float TempsMort = 5;
// Fixed size dimensions of array or collections stored in the TTree if any.



class timeStudy {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   vector<IMPL::SimCalorimeterHitImpl*> *SDHCAL_Proto_EndCap;

   // List of branches
   TBranch        *b_SDHCAL_Proto_EndCap;   //!

   timeStudy(TTree *tree=0);
   virtual ~timeStudy();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);


   float TimeStep;
   float RadiusAvalancheBlindness;
   void setDefaultParameters() 
   {
     TimeStep=500.0; //500 ns
     RadiusAvalancheBlindness=4; //4 mm
   }

   //   TH1I *nMuonHitHisto;
   //  TH1F *muonHitTimeHisto;
   TH1F* minTimeHisto[7];
   // , minTimeHisto(7, (new TH1F("minTime","Minimum time between a hit and a muon hit(ns)", 1000, 0, 1e6)))
   float* EffiByTime[7];
   float  DeadTime[EffiSize];
   TGraph *generateTGraph(int chamberNumber) 
   {
     return new TGraph(EffiSize,DeadTime,EffiByTime[chamberNumber]);
   }

   //   float  eff;
};

#endif

#ifdef timeStudy_cxx
timeStudy::timeStudy(TTree *tree) : fChain(0)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("lcioTree.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("lcioTree.root");
      }
      f->GetObject("LCIO",tree);

   }
   Init(tree);

   setDefaultParameters();
   
   for(Int_t i=0; i<7; i++)
     {
       TString HisName;
       HisName.Form("%d",i);
       HisName = "TimeHisot@Chambre"+HisName;
       minTimeHisto[i] = new TH1F(HisName,"Minimum time between a hit and a muon hit(ns)", 1000, 0, 1e6);
     }
   for(int i=0; i<EffiSize; i++) DeadTime[i] = TimeStep*((float)i);
   for(int i=0; i<7; i++) EffiByTime[i] = new float[EffiSize];
   //   eff = 0;  
}

timeStudy::~timeStudy()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
   for(int j=0; j<7; j++){ delete[] EffiByTime[j];}
}

Int_t timeStudy::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t timeStudy::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void timeStudy::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   SDHCAL_Proto_EndCap = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("SDHCAL_Proto_EndCap", &SDHCAL_Proto_EndCap, &b_SDHCAL_Proto_EndCap);
   Notify();
}

Bool_t timeStudy::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void timeStudy::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t timeStudy::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef timeStudy_cxx
