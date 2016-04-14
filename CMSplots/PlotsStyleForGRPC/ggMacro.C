#include "tdrstyle.C"
#include "CMS_lumi.C"
#include "TH1.h"
#include "TH1F.h"
#include "TFile.h"
#include "TH1I.h"
#include "TAxis.h"
 


void ggMacro(TString filename);

// iPeriod : 1=Emu = 150 GeV; 2=Nothing for the mometnt just fill with what we need
  // parameter iPos drives the position of the CMS logo in the plot
  // iPos=11 : top-left, left-aligned
  // iPos=33 : top-right, right-aligned
  // iPos=22 : center, centered
  // mode generally : 
  //   iPos = 10*(alignement 1/2/3) + position (1/2/3 = left/center/right)

TCanvas* makeCanva(int iPos,  int iPeriod=1, bool writeExtraText=true);
void myDrawHist(TH1* h);

void ggMacro()
{
  ggMacro("../../test.000_1M_7layers_0deg_g10PCB.slcio.root");
}


TH1F* createScaledHisto(TString fileName)
{
  TFile *f=TFile::Open(fileName);
  f->cd("SimHits");
  TH1I *hist=(TH1I*) gDirectory->GetObjectChecked("NumeroPlan","TH1I");
  //hist->GetXaxis()->SetTitle("CMS GRPC Chamber");
  //hist->GetYaxis()->SetTitle("Number of hits");
  //hist->Draw();
 
  TH1F *hScale=new TH1F("NumeroPlanScale","Fraction of source gamma giving hit", 7, 0, 7);
  for (int i=1; i<=hist->GetNbinsX(); i++)
    {
      hScale->SetBinContent(i,hist->GetBinContent(i)/1e6);
      hScale->SetBinError(i,sqrt(hist->GetBinContent(i))/1e6);
    }
  hScale->GetXaxis()->SetTitle("CMS GRPC Chamber");
  hScale->GetYaxis()->SetTitle("Fraction of source gammas giving a hit");

  //f->Close();
  return hScale;
}

void ggMacro(TString fileName)
{
  setTDRStyle();
  
  TH1F* hScale=createScaledHisto(fileName);

  //add extraText
  writeExtraText = true;       // if extra text
  extraText  = "Simulation";  // default extra text is "Preliminary"
  cmsText = "GRPC-GIF++";

  Int_t iPos=22; Int_t iPeriod=1;
  TCanvas *can=makeCanva(iPos,iPeriod,true);
  myDrawHist(hScale,"#0000ff",21,0,"GIF++ CMS-GRPC setup")->Draw();

  // writing the lumi information and the CMS "logo"
  CMS_lumi( can, iPeriod, iPos );

  can->Update();
  can->RedrawAxis();
  can->GetFrame()->Draw();

  can->Print(TString(can->GetName())+".pdf",".pdf");
  can->Print(TString(can->GetName())+".png",".png");

}
    
void ggMultiPlot()
{
  setTDRStyle();
  TH1F* hScaleNormalPCB=createScaledHisto("../../test.000_1M_7layers_0deg_g10PCB.slcio.root");
  TH1F* hScaleNormalAir=createScaledHisto("../../digitised_test.000_1M_7layers.slcio.root");
}



TCanvas* makeCanva(int iPos, int iPeriod, bool writeExtraText)
{
  int W = 800;
  int H = 600;

  int H_ref = 600; 
  int W_ref = 800; 

  // references for T, B, L, R
  float T = 0.08*H_ref;
  float B = 0.12*H_ref; 
  float L = 0.12*W_ref;
  float R = 0.04*W_ref;

  TString canvName = "FigExample_";
  canvName += W;
  canvName += "-";
  canvName += H;
  canvName += "_";  
  canvName += iPeriod;
  if( writeExtraText ) {canvName += "-"; canvName+=extraText;}
  if( iPos%10==0 ) canvName += "-out";
  else if( iPos%10==1 ) canvName += "-left";
  else if( iPos%10==2 )  canvName += "-center";
  else if( iPos%10==3 )  canvName += "-right";

  TCanvas* canv = new TCanvas(canvName,canvName,50,50,W,H);
  canv->SetFillColor(0);
  canv->SetBorderMode(0);
  canv->SetFrameFillStyle(0);
  canv->SetFrameBorderMode(0);
  canv->SetLeftMargin( L/W );
  canv->SetRightMargin( R/W );
  canv->SetTopMargin( T/H );
  canv->SetBottomMargin( B/H );
  canv->SetTickx(0);
  canv->SetTicky(0);

  return canv;
}

TLegend* myDrawHist(TH1* h,TString markercol,Int_t markerstyle,TLegend *leg,TString legend)
{
  Int_t ci;      // for color index setting
  TColor *color; // for color definition with alpha
  //ci = TColor::GetColor("#0000ff");
  ci = TColor::GetColor(markercol);
  h->SetMarkerColor(ci);
  //h->SetMarkerStyle(21);
  h->SetMarkerStyle(markerstyle);
  
  //h->SetDirectory(0);
  h->SetStats(0);

  ci = TColor::GetColor("#000099");
  h->SetLineColor(ci);
  h->GetXaxis()->CenterTitle(true);
  h->GetXaxis()->SetLabelFont(42);
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.05);
  h->GetXaxis()->SetTitleFont(42);
  h->GetXaxis()->SetTitleOffset(1.0);
  
  h->GetYaxis()->SetTitleOffset(1.0);
  h->GetYaxis()->CenterTitle(true);
  h->GetYaxis()->SetLabelFont(42);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetTitleSize(0.05);
  h->GetYaxis()->SetTitleFont(42);

  TGaxis::SetMaxDigits(2);
  h->GetYaxis()->SetNoExponent(false);
  
  h->GetZaxis()->SetLabelFont(42);
  h->GetZaxis()->SetLabelSize(0.035);
  h->GetZaxis()->SetTitleSize(0.035);
  h->GetZaxis()->SetTitleFont(42);
  
  h->Draw();

  if (leg==0) leg = new TLegend(0.55,0.55,0.70,0.70);
  leg->SetTextSize(0.035);
  leg->SetLineColor(1);
  leg->SetLineStyle(1);
  leg->SetLineWidth(1);
  leg->SetFillColor(0);
  leg->SetBorderSize(0);
  leg->SetHeader("GEANT4 simulation");
  leg->AddEntry(h, legend, "p");
  //leg->Draw();
  return leg;
}



