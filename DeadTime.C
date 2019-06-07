#define DeadTime_cxx
#include "DeadTime.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <algorithm>
#include <string>
#include <map>
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TF1.h"
#include "TGraphErrors.h"


#include "UTIL/CellIDDecoder.h"

UTIL::CellIDDecoder<IMPL::SimCalorimeterHitImpl>theHitDecoder("M:3,S-1:3,I:9,J:9,K-1:6");


void DeadTime::Loop()
{
//   In a ROOT session, you can do:
//      root> .L DeadTime.C
//      root> DeadTime t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
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

   Long64_t nentries = fChain->GetEntriesFast();

   if (modele==NULL) 
   {
      cout << "Le modele de temps mort n'est pas defini" << endl;
      return;
   }

   double nMuonsEnvoyes(0), nMuonsDetectes(0);
   std::vector<double> event_flux;

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      trouveLesMuons(0);
      if (lesMuons.empty()) continue;
      nMuonsEnvoyes+=lesMuons.size();
      nMuonsDetectes+=modele->NombreMuonsDetectes(lesMuons,lesPasMuons);
     
      int NpasMuon=0;
      double timeMin(0),timeMax(0);
      bool timeSet=false;
      for (vector<MCContributionLocalisation>::iterator itReste=lesPasMuons.begin(); itReste!= lesPasMuons.end(); ++itReste)
      {
         const float* RestePos=itReste->lehit->getStepPosition(itReste->indiceContribution);
         double ResteTime=itReste->lehit->getTimeCont(itReste->indiceContribution);
         if (timeSet) {if (ResteTime<timeMin) timeMin=ResteTime; if (ResteTime>timeMax) timeMax=ResteTime;}
         else {timeMin=ResteTime; timeMax=ResteTime; timeSet=true;}
	 NpasMuon++;
      }
      if (timeMax>timeMin)
         event_flux.push_back(NpasMuon/((timeMax-timeMin)*(3.14159*0.5*0.5)*1e-9));
   }
   efficacite=nMuonsDetectes/double(nMuonsEnvoyes);

   cout << "L'efficacite de detection est de "<< efficacite*100 << " %" << endl; 
   if (event_flux.empty()) return;
   double meanflux=0;
   for (std::vector<double>::iterator it=event_flux.begin(); it!=event_flux.end(); ++it) 
   meanflux+=(*it);
   flux = meanflux/event_flux.size();
   cout << "Le flux moyen est de " << flux*(1000/3)<< " particules/cm^2/s" << endl;
   cout << "-------------------------------------------------------------" << endl;
}


void DeadTime::trouveLesMuons(int numeroChambre)
{
   lesMuons.clear();
   lesPasMuons.clear();
  
   for (vector<IMPL::SimCalorimeterHitImpl*>::iterator it=SDHCAL_Proto_EndCap->begin(); it!=SDHCAL_Proto_EndCap->end(); ++it)
   {
      IMPL::SimCalorimeterHitImpl *hit=*it;
      int numChambre=theHitDecoder(hit)["K-1"];
      if (numChambre != numeroChambre) continue;
      for (int i=0; i<hit->getNMCContributions(); ++i)			
      { 
         if (hit->getPDGCont(i) == -13) lesMuons.push_back(MCContributionLocalisation(hit,i));
         else lesPasMuons.push_back(MCContributionLocalisation(hit,i));
      }
   }
}



int rayonTempsMort::NombreMuonsDetectes(vector<MCContributionLocalisation>& lesMuons, vector<MCContributionLocalisation>& lesPasMuons)
{
   int nDetectes=0;
   for (vector<MCContributionLocalisation>::iterator itMuon=lesMuons.begin(); itMuon!= lesMuons.end(); ++itMuon)
   {
      bool vu=true;
      const float* MuonPos=itMuon->lehit->getStepPosition(itMuon->indiceContribution);
      double MuonTime=itMuon->lehit->getTimeCont(itMuon->indiceContribution);
      for (vector<MCContributionLocalisation>::iterator itReste=lesPasMuons.begin(); itReste!= lesPasMuons.end(); ++itReste)
      {
         const float* RestePos=itReste->lehit->getStepPosition(itReste->indiceContribution);
         double ResteTime=itReste->lehit->getTimeCont(itReste->indiceContribution);
         double distCarre=(MuonPos[0]-RestePos[0])*(MuonPos[0]-RestePos[0])+(MuonPos[1]-RestePos[1])*(MuonPos[1]-RestePos[1]);
         vu=!(distCarre<rayonMort*rayonMort && MuonTime-ResteTime<tempsMort && MuonTime>ResteTime);
         if (!vu) break;
      }
      if (vu) ++nDetectes;
   }
   return nDetectes;
}

int BeaucoupDeMuons::NombreMuonsDetectes(vector<MCContributionLocalisation>& lesMuons, vector<MCContributionLocalisation>& lesPasMuons)
{
   int nDetectes=0;
   for (vector<MCContributionLocalisation>::iterator itMuon=lesMuons.begin(); itMuon!= lesMuons.end(); ++itMuon)
   {
      bool vu=true;
      const float* MuonPos=itMuon->lehit->getStepPosition(itMuon->indiceContribution);
      double MuonTime=itMuon->lehit->getTimeCont(itMuon->indiceContribution);
      for (vector<MCContributionLocalisation>::iterator itReste=lesPasMuons.begin(); itReste!= lesPasMuons.end(); ++itReste)
      {
         const float* RestePos=itReste->lehit->getStepPosition(itReste->indiceContribution);
         double ResteTime=itReste->lehit->getTimeCont(itReste->indiceContribution);
         double distCarre=(MuonPos[0]-RestePos[0])*(MuonPos[0]-RestePos[0])+(MuonPos[1]-RestePos[1])*(MuonPos[1]-RestePos[1]);
         vu=!(distCarre<rayonMort*rayonMort && MuonTime-ResteTime<tempsMort && MuonTime>ResteTime);
         if (!vu) break;
      }
      if (vu) 
      {
        for (vector<MCContributionLocalisation>::iterator itMuonBis=lesMuons.begin(); itMuonBis!= lesMuons.end(); ++itMuonBis)
        {
           if (itMuonBis==itMuon) continue;
           const float* MuonBisPos=itMuonBis->lehit->getStepPosition(itMuonBis->indiceContribution);
           double MuonBisTime=itMuonBis->lehit->getTimeCont(itMuonBis->indiceContribution);
           double distCarre=(MuonPos[0]-MuonBisPos[0])*(MuonPos[0]-MuonBisPos[0])+(MuonPos[1]-MuonBisPos[1])*(MuonPos[1]-MuonBisPos[1]);
           vu=!(distCarre<rayonMort*rayonMort && MuonTime-MuonBisTime<tempsMort && MuonTime>MuonBisTime);
         if (!vu) break;
        }
      }
      if (vu) ++nDetectes;
   }
   return nDetectes;


}

int ajouteMuonDansTempsMortLente::NombreMuonsDetectes(vector<MCContributionLocalisation>& lesMuons, vector<MCContributionLocalisation>& lesPasMuons)
{
   int nDetectes=0;
   for (vector<MCContributionLocalisation>::iterator itMuon=lesMuons.begin(); itMuon!= lesMuons.end(); ++itMuon)
   {
      vector<MCContributionLocalisation> tousleshits=lesPasMuons;
      vector<MCContributionLocalisation> ceMuon;
      ceMuon.push_back(*itMuon);	
      for (vector<MCContributionLocalisation>::iterator itMuonBis=lesMuons.begin(); itMuonBis!= lesMuons.end(); ++itMuonBis)
      {
         if (itMuonBis==itMuon) continue;
         tousleshits.push_back(*itMuonBis);
      }
    nDetectes+=original.NombreMuonsDetectes(ceMuon,tousleshits);
   }
   return nDetectes;
}


int TempsMortNonParalysable::NombreMuonsDetectes(vector<MCContributionLocalisation>& lesMuons, vector<MCContributionLocalisation>& lesPasMuons)
{
   int nDetectes=0;
   for (vector<MCContributionLocalisation>::iterator itMuon=lesMuons.begin(); itMuon!= lesMuons.end(); ++itMuon)
   {
      bool vu=true;
      double MuonTime=itMuon->lehit->getTimeCont(itMuon->indiceContribution);
      const float* MuonPos=itMuon->lehit->getStepPosition(itMuon->indiceContribution);  
      vector<MCContributionLocalisation> hits;
	
	for ( vector<MCContributionLocalisation>::iterator itReste=lesPasMuons.begin(); itReste!= lesPasMuons.end(); )
	{

         	double ResteTime=itReste->lehit->getTimeCont(itReste->indiceContribution);

		if ( ResteTime > MuonTime )

			itReste = lesPasMuons.erase(itReste) ;
		else
		{
			hits.push_back(*itReste);
			++itReste ;
		}		
	} 
	sort(hits.begin(), hits.end(), CompareTemps);
	hits.push_back(*itMuon);
	double Timestart = 0;

	for ( vector<MCContributionLocalisation>::iterator it=hits.begin(); it!= hits.end(); ++it)
	{
		
		const float* itPos=it->lehit->getStepPosition(it->indiceContribution);
		double Time=it->lehit->getTimeCont(it->indiceContribution);
		double distCarre=(MuonPos[0]-itPos[0])*(MuonPos[0]-itPos[0])+(MuonPos[1]-itPos[1])*(MuonPos[1]-itPos[1]);
		
		if (distCarre>4*rayonMort*rayonMort) continue;
		  
		if ( (Time - Timestart ) >= tempsMort ) Timestart = Time ;
			
	}
	vu= (Timestart == MuonTime);
	if (vu) ++nDetectes;
    }
    return nDetectes;
}


int EffetsDeBords::NombreMuonsDetectes(vector<MCContributionLocalisation>& lesMuons, vector<MCContributionLocalisation>& lesPasMuons)
{
   int nDetectes=0;
   double lastMuonTime=-1e9;
   for (vector<MCContributionLocalisation>::iterator itMuon=lesMuons.begin(); itMuon!= lesMuons.end(); ++itMuon)
   {
      double MuonTime=itMuon->lehit->getTimeCont(itMuon->indiceContribution);
      if (MuonTime>lastMuonTime) lastMuonTime=MuonTime;
   }
   vector<MCContributionLocalisation> hits;
   for ( vector<MCContributionLocalisation>::iterator itReste=lesPasMuons.begin(); itReste!= lesPasMuons.end(); ++itReste)
   {
      double ResteTime=itReste->lehit->getTimeCont(itReste->indiceContribution);
      if ( ResteTime > lastMuonTime ) continue ;
      else hits.push_back(*itReste);	
   }		
   sort(hits.begin(), hits.end(),CompareTemps);

   for ( vector<MCContributionLocalisation>::iterator it=hits.begin();it!=hits.end();++it)
   {
      const float* itPos=it->lehit->getStepPosition(it->indiceContribution);
      double Time=it->lehit->getTimeCont(it->indiceContribution);
      vector<MCContributionLocalisation>::iterator itBis=it;
      ++itBis;
      while (itBis != hits.end() )
      {
         const float* itBisPos=itBis->lehit->getStepPosition(itBis->indiceContribution);
         double itBisTime=itBis->lehit->getTimeCont(itBis->indiceContribution);
         double distCarre=(itBisPos[0]-itPos[0])*(itBisPos[0]-itPos[0])+(itBisPos[1]-itPos[1])*(itBisPos[1]-itPos[1]);
         if ( itBisTime-Time < tempsMort) 
         {  
            if (distCarre < rayonMort*rayonMort) itBis = hits.erase(itBis); 
            else ++itBis;
         }
         else break;
      }
   }
   for (vector<MCContributionLocalisation>::iterator itMuon=lesMuons.begin(); itMuon!= lesMuons.end(); ++itMuon)
   {
      bool vu=true;
      double MuonTime=itMuon->lehit->getTimeCont(itMuon->indiceContribution);
      const float* MuonPos=itMuon->lehit->getStepPosition(itMuon->indiceContribution);  
      for ( vector<MCContributionLocalisation>::iterator it=hits.begin();it!=hits.end();++it)
      {
        const float* itPos=it->lehit->getStepPosition(it->indiceContribution);
        double Time=it->lehit->getTimeCont(it->indiceContribution);
        double distCarre=(itPos[0]-MuonPos[0])*(itPos[0]-MuonPos[0])+(itPos[1]-MuonPos[1])*(itPos[1]-MuonPos[1]);
	if ( MuonTime-Time < tempsMort &&  distCarre < rayonMort*rayonMort) 
        {
           vu=false;
           break;
        }
      }	
      if (vu) ++nDetectes;    
    }
    return nDetectes;
}

map<double,double> calculeEff(map<double,string>& nomsDesFichiersROOT, ModeleTempsMort* modele)
{
   map<double,double> resultats;
   if (modele==NULL) return resultats;	
   for (map<double,string>::iterator itfilename=nomsDesFichiersROOT.begin(); itfilename!=nomsDesFichiersROOT.end(); ++itfilename)
   {
      
      TFile *f=TFile::Open(itfilename->second.c_str());
      TTree *tree=NULL;
      f->GetObject("LCIO",tree);
      if (NULL==tree) {cout << " fichier non trouve : " << (itfilename->second) << endl; continue;}
      DeadTime d(tree);
      d.setModeleDeTempsMort(modele);
      d.Loop();
    //  resultats[itfilename->first]=d.getEfficacite();      // Modele theorique
      resultats[d.getFlux()]=d.getEfficacite();              // Modele Reel
      f->Close();
   }
   return resultats;
} 


MODELE::MODELE()
{
  Mes_modeles[RTM]=new rayonTempsMort();
  Mes_modeles[BDM]=new BeaucoupDeMuons();
  Mes_modeles[NP] =new TempsMortNonParalysable();
  Mes_modeles[EDB]=new EffetsDeBords();
}

MODELE::~MODELE()
{
  for (int i=0; i<NMODELES; ++i) delete Mes_modeles[i];
}

ModeleTempsMort* MODELE::get(MODELE::VALEUR v)
{
   if (v>=NMODELES) {cout<< "MODELE MAX= " << NMODELES-1 << endl; return NULL;}
   return Mes_modeles[v];
}


void calculExemplePourModele(ModeleTempsMort* modele)
{
 map<double,string> fluxgamma1_nomFichiers;
  { 
  /*
   fluxgamma1_nomFichiers[5.3e4]="1_cm_5_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[1.06e5]="1_cm_10_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[2.65e5]="1_cm_25_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[5.3e5]="1_cm_50_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[1.06e6]="1_cm_100_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[2.12e6]="1_cm_200_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[3.18e6]="1_cm_300_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[4.24e6]="1_cm_400_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[5.3e6]="1_cm_500_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[6.36e6]="1_cm_600_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[7.42e6]="1_cm_700_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[8.48e6]="1_cm_800_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[9.54e6]="1_cm_900_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[1.06e7]="1_cm_1000_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[1.59e7]="1_cm_1500_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[2.12e7]="1_cm_2000_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[3.18e7]="1_cm_3000_muons_1000_runs_1e7.root"; */

   fluxgamma1_nomFichiers[2.12e5]="5_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[4.23e5]="10_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[1.06e6]="25_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[2.12e6]="50_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[4.23e6]="100_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[8.46e6]="200_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[1.27e7]="300_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[1.69e7]="400_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[2.12e7]="500_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[2.53e7]="600_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[2.96e7]="700_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[3.38e7]="800_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[3.80e7]="900_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[4.23e7]="1000_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[6.35e7]="1500_muons_1000_runs_1e7.root";
   fluxgamma1_nomFichiers[8.46e7]="2000_muons_1000_runs_1e7.root";

  }
       
  map<double,double> res1=calculeEff(fluxgamma1_nomFichiers,modele);
  TGraphErrors *gr1=new TGraphErrors(res1.size());
  int numero1=0;
  for (map<double,double>::iterator it=res1.begin(); it!=res1.end(); ++it)
  {
     gr1->SetPoint(numero1,it->first,it->second);
     gr1->SetPointError(numero1,0,3*sqrt((it->second)*(100-it->second)/1000));
     ++numero1;
  } 
  gr1->SetTitle("Courbe de l'efficacite pour 1000 runs");
  gr1->SetMarkerStyle(8);
  gr1->SetLineColor(kRed);
  gr1->SetLineWidth(2); 
  gr1->Fit("gaus");
 
 // TF1 *g1 = new TF1("g1","gaus+[3]/(pow(x,[4]))",0.,200000000.);
 // gr1->Fit ("g1","R");

    TGraphErrors *gr2 = new TGraphErrors(6);
    gr2->SetName("Graph1");
    gr2->SetTitle("");
    gr2->SetFillColor(1);
    gr2->SetLineColor(2);
    gr2->SetMarkerColor(2);
    gr2->SetMarkerStyle(3);
    gr2->SetMarkerSize(1.8);
    gr2->SetPoint(0,1500000,0);
    gr2->SetPointError(0,0,0);
    gr2->SetPoint(1,681818.2,0.0381356*100);
    gr2->SetPointError(1,0,0.0638412*100);
    gr2->SetPoint(2,217391.3,0.242259*100);
    gr2->SetPointError(2,0,0.0371513*100);
    gr2->SetPoint(3,68181.82,0.70679*100);
    gr2->SetPointError(3,0,0.0113702*100);
    gr2->SetPoint(4,326.0869,0.919075*100);
    gr2->SetPointError(4,0,0.021681*100);
    gr2->SetPoint(5,150,0.929634*100);
    gr2->SetPointError(5,0,0.00395216*100);

  TGraphErrors *gr3 = new TGraphErrors(6);
    gr3->SetName("Graph0");
    gr3->SetTitle("");
    gr3->SetFillColor(1);
    gr3->SetLineColor(4);
    gr3->SetMarkerColor(4);
    gr3->SetMarkerStyle(22);
    gr3->SetMarkerSize(1.8);
    gr3->SetPoint(0,1500000,0.192982*100);
    gr3->SetPointError(0,0,0.118988*100);
    gr3->SetPoint(1,681818.2,0.704545*100);
    gr3->SetPointError(1,0,0.0473106*100);
    gr3->SetPoint(2,217391.3,0.943028*100);
    gr3->SetPointError(2,0,0.009242*100);
    gr3->SetPoint(3,68181.82,0.973359*100);
    gr3->SetPointError(3,0,0.00320719*100);
    gr3->SetPoint(4,326.0869,0.967568*100);
    gr3->SetPointError(4,0,0.0132405*100);
    gr3->SetPoint(5,150,0.97559*100);
    gr3->SetPointError(5,0,0.00223768*100);




  TCanvas *c=new TCanvas;
  TMultiGraph *F=new TMultiGraph();
  F->Add(gr1,"AP");
  F->Add(gr2,"APL");
  F->Add(gr3,"APL");
  F->SetTitle("Efficacite de detection en fonction du flux de photons pour un rayon et temps mort donne");
  F->GetXaxis()->SetTitle("Flux de gammas (photons/cm^2/s)");
  F->GetYaxis()->SetTitle("Efficacite de detection (%)");
  F->GetXaxis()->CenterTitle(1);
  F->GetYaxis()->CenterTitle(1);
  F->GetXaxis()->SetRangeUser(0.,200000000.);
  F->GetYaxis()->SetRangeUser(0.,100.);
  F->Draw("A");
  gPad->SetLogx();
  c->BuildLegend();
}

void calculeExemple(double rayon, double temps,MODELE::VALEUR v)
{
   MODELE mf;
   ModeleTempsMort* modele=mf.get(v);
   if (NULL==modele) return;
   modele->setRayonMort(rayon);
   modele->setTempsMort(temps);
   calculExemplePourModele(modele);
}

