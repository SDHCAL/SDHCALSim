//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon May 13 16:15:10 2019 by ROOT version 6.14/06
// from TTree LCIO/lcio event data tree
// found on file: lcioTree.root
//////////////////////////////////////////////////////////

#ifndef DeadTime_h
#define DeadTime_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1I.h>

// Header file for the classes stored in the TTree if any.
#include "IMPL/SimCalorimeterHitImpl.h"
#include "vector"
using namespace std;




struct MCContributionLocalisation
{
  MCContributionLocalisation() : lehit(NULL), indiceContribution(0) {}
  MCContributionLocalisation(IMPL::SimCalorimeterHitImpl* hit, int contrib) : lehit(hit), indiceContribution(contrib) {}
  IMPL::SimCalorimeterHitImpl* lehit;
  int indiceContribution;
};

struct compareTemps 
{
bool operator() (MCContributionLocalisation a, MCContributionLocalisation b)
{ 
return ( a.lehit->getTimeCont(a.indiceContribution) < b.lehit->getTimeCont(b.indiceContribution));}

} CompareTemps;


class ModeleTempsMort
{
public:
  virtual ~ModeleTempsMort() {;}
  void setRayonMort(double leRayonMort) {rayonMort=leRayonMort;}
  void setTempsMort(double leTempsMort) {tempsMort=leTempsMort;}
  virtual int NombreMuonsDetectes(vector<MCContributionLocalisation>& lesMuons, vector<MCContributionLocalisation>& lesPasMuons)=0;
  double rayonMort;
  double tempsMort;
private:
  
};


class sansTempsMort : public ModeleTempsMort
{
public:
  virtual int NombreMuonsDetectes(vector<MCContributionLocalisation>& lesMuons, vector<MCContributionLocalisation>& lesPasMuons)
  {
     return lesMuons.size();
  }
};

class DetecteurOff : public ModeleTempsMort
{
public:
  virtual int NombreMuonsDetectes(vector<MCContributionLocalisation>& lesMuons, vector<MCContributionLocalisation>& lesPasMuons)
  {
     return 0;
  }
};

class rayonTempsMort : public ModeleTempsMort
{
public:
   
   virtual int NombreMuonsDetectes(vector<MCContributionLocalisation>& lesMuons, vector<MCContributionLocalisation>& lesPasMuons);
private:
   
};

class BeaucoupDeMuons : public ModeleTempsMort
{
public:
  
   virtual int NombreMuonsDetectes(vector<MCContributionLocalisation>& lesMuons, vector<MCContributionLocalisation>& lesPasMuons);
private:

};

//design pattern decorator
class ajouteMuonDansTempsMortLente : public ModeleTempsMort
{
private:
  ModeleTempsMort &original;
public:
  ajouteMuonDansTempsMortLente(ModeleTempsMort &o) : original(o) {}
  virtual int NombreMuonsDetectes(vector<MCContributionLocalisation>& lesMuons, vector<MCContributionLocalisation>& lesPasMuons);
};


class TempsMortNonParalysable : public ModeleTempsMort
{
public:
   
   virtual int NombreMuonsDetectes(vector<MCContributionLocalisation>& lesMuons, vector<MCContributionLocalisation>& lesPasMuons);

private:
 
};

class EffetsDeBords : public ModeleTempsMort
{
public:

   virtual int NombreMuonsDetectes(vector<MCContributionLocalisation>& lesMuons, vector<MCContributionLocalisation>& lesPasMuons);

private:
 
};

class MODELE
{
public:
  enum VALEUR {RTM,BDM,NP,EDB,NMODELES};
  MODELE();
  ~MODELE();
  ModeleTempsMort *get(VALEUR);
private:
  ModeleTempsMort *Mes_modeles[NMODELES];
};



class DeadTime {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   vector<IMPL::SimCalorimeterHitImpl*> *SDHCAL_Proto_EndCap;

   // List of branches
   TBranch        *b_SDHCAL_Proto_EndCap;   //!

   DeadTime(TTree *tree=0);
   virtual ~DeadTime();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   virtual void trouveLesMuons(int numeroChambre); 
   void setModeleDeTempsMort(ModeleTempsMort *leModele) { modele=leModele;}
   double getEfficacite() {return (efficacite*100);}
   double getFlux() {return (flux*(1000/3));}

private:
   vector<MCContributionLocalisation> lesMuons; 
   vector<MCContributionLocalisation> lesPasMuons;
   ModeleTempsMort *modele;
   double efficacite;
   double flux;
 
};

#endif

#ifdef DeadTime_cxx
DeadTime::DeadTime(TTree *tree) : fChain(0), modele(NULL), efficacite(-1) 
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
}

DeadTime::~DeadTime()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t DeadTime::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t DeadTime::LoadTree(Long64_t entry)
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

void DeadTime::Init(TTree *tree)
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

Bool_t DeadTime::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void DeadTime::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t DeadTime::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef DeadTime_cxx
