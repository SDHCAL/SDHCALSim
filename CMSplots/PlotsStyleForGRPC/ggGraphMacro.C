#include "tdrstyle.C"
#include "CMS_lumi.C"
#include "TH1.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TFrame.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include <map>

class DrawSomething
{
  int W,H;
  int H_ref;
  int W_ref;
  float T,B,L,R;

 
  int iPeriod,iPos;

  TString canvName;
  TCanvas *canv;
  TLegend *leg;

  float leg_xmin,leg_xmax,leg_ymin,leg_ymax;

  void setCanvName(const char *text);
  void makeCanva();
  void makeLegend(const char *header);
  void setGraphAxis(TGraph* gr);

public:
  DrawSomething();
  void setLegendCoord(float xmin,float ymin, float xmax, float ymax) {leg_xmin=xmin; leg_ymin=ymin; leg_xmax=xmax; leg_ymax=ymax;}
  void beginCanva(const char* text, const char* legendHeader, int Width=800, int Height=600);
  void drawFake(float xmax, float ymax);
  void addGraph(TGraph* gre,const char *legEntry, bool drawAxes=false); //works also for TGraphError
  void finalizeCanva();
};


DrawSomething::DrawSomething()
{
  H_ref = 600; 
  W_ref = 800; 
  // references for T, B, L, R
  T = 0.08*H_ref;
  B = 0.12*H_ref; 
  L = 0.12*W_ref;
  R = 0.04*W_ref;


  canv=NULL;  
  leg=NULL;

  //leg = new TLegend(
  leg_xmin=0.15;
  leg_ymin=0.5;
  leg_xmax=0.35;
  leg_ymax=0.75;


  //globalstuff (see CMS_lumi.h)
  writeExtraText = true;       // if extra text
  extraText  = "Preliminary";  // default extra text is "Preliminary"
  
  iPeriod = 2;    // 1=Emu = 150 GeV; 2=Nothing for the mometnt just fill with what we need
  
  // second parameter in example_plot is iPos, which drives the position of the CMS logo in the plot
  // iPos=11 : top-left, left-aligned
  // iPos=33 : top-right, right-aligned
  // iPos=22 : center, centered
  // mode generally : 
  //   iPos = 10*(alignement 1/2/3) + position (1/2/3 = left/center/right)

  iPos = 11;
  
}


void DrawSomething::beginCanva(const char* text, const char* legendHeader, int Width, int Height)
{
  W=Width;
  H=Height;
  setCanvName(text);
  makeCanva();
  makeLegend(legendHeader);
}

void DrawSomething::setCanvName(const char *text)
{
  canvName = text;
  canvName += "-";
  canvName += W;
  canvName += "-";
  canvName += H;
  canvName += "_";  
  canvName += iPeriod;
  if( writeExtraText ) canvName += "-prelim"; //writeExtraText = global variable from CMS_lumi
  if( iPos%10==0 ) canvName += "-out";
  else if( iPos%10==1 ) canvName += "-left";
  else if( iPos%10==2 )  canvName += "-center";
  else if( iPos%10==3 )  canvName += "-right";
}

void DrawSomething::makeCanva()
{
  canv = new TCanvas(canvName,canvName,50,50,W,H);
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
}

void DrawSomething::setGraphAxis(TGraph* gr)
{
   gr->GetXaxis()->SetTitle("High Voltage (V)");
   gr->GetXaxis()->CenterTitle(true);
   gr->GetXaxis()->SetLabelFont(42);
   gr->GetXaxis()->SetLabelSize(0.05);
   gr->GetXaxis()->SetTitleSize(0.05);
   gr->GetXaxis()->SetTitleFont(42);
   gr->GetXaxis()->SetTitleOffset(1.0);
   gr->GetYaxis()->SetTitle("Current (#muA)");
   gr->GetYaxis()->SetTitleOffset(1.0);
   gr->GetYaxis()->CenterTitle(true);
   gr->GetYaxis()->SetLabelFont(42);
   gr->GetYaxis()->SetLabelSize(0.05);
   gr->GetYaxis()->SetTitleSize(0.05);
   gr->GetYaxis()->SetTitleFont(42);
}


void DrawSomething::drawFake(float xmax, float ymax)
{
  TGraph*  gr_fake=new TGraph(2);
  gr_fake->SetMarkerStyle(1);
  gr_fake->SetLineColor(0);
  gr_fake->SetMarkerColor(0);
  gr_fake->SetPoint(0,0,0);
  gr_fake->SetPoint(1,xmax,ymax);
  setGraphAxis(gr_fake);
  gr_fake->Draw("ap");
}

void DrawSomething::makeLegend(const char *header)
{
  leg = new TLegend(leg_xmin,leg_ymin,leg_xmax,leg_ymax);
  leg->SetTextSize(0.035);
  leg->SetLineColor(1);
  leg->SetLineStyle(1);
  leg->SetLineWidth(1);
  leg->SetFillColor(0);
  leg->SetBorderSize(0);
  leg->SetHeader(header);
}


void DrawSomething::addGraph(TGraph* gre,const char *legEntry, bool drawAxes)
{
  gre->SetFillColor(1);

  if (drawAxes) gre->Draw("ap"); 
  else gre->Draw("p"); 
  if (NULL != legEntry) leg->AddEntry(gre, legEntry, "p");
}

void DrawSomething::finalizeCanva()
{
  leg->Draw();

  // writing the lumi information and the CMS "logo"
  CMS_lumi( canv, iPeriod, iPos );
  
  canv->Update();
  canv->RedrawAxis();
  canv->GetFrame()->Draw();

  canv->Print(canvName+".pdf",".pdf");
  canv->Print(canvName+".png",".png");
}



//The various data
float HVgg[44]=
  { 200,  400,  600,  800, 1000, //  5
   1200, 1400, 1600, 1650, 1700, // 10 
   1750, 1800, 1850, 1900, 1950, // 15 
   2000, 2050, 2100, 2150, 2200, // 20 
   2250, 2300, 2400, 2500, 2600, // 25 
   2700, 2800, 2900, 3000, 3100, // 30 
   3200, 3300, 3400, 3500, 3600, // 35 
   3700, 3800, 3900, 4000, 4100, // 40 
   4200, 4300, 4400, 4500        // 44 
  };

float HVmax[30]=
  {2000, 2050, 2100, 2150, 2200, //  5
   2250, 2300, 2400, 2500, 2600, // 10
   2700, 2800, 2900, 3000, 3100, // 15
   3200, 3300, 3400, 3500, 3600, // 20
   3700, 3800, 3900, 4000, 4200, // 25
   4400, 4600, 4800, 5000, 5200  // 30
  };

float Ich1ggJune15[22]=
  {0  , 0  , 0  , 0  , 0  , //  5
   0  , 0  , 0.1, 0.2, 0.3, // 10
   0.5, 0.7, 0.9, 1.1, 1.5, // 15
   2.0, 2.4, 2.8, 3.4, 4.1, // 20
   4.6, 5.4                 // 22
  };

float Ich1maxJune15[15]=
  {1.6 , 1.9, 2.6, 3.2, 3.7, //  5
   4.4 , 4.9, 6.2, 8.0, 9.9, // 10
   11.7,13.8,15.9,18.5,20.8  // 15
  };

float Ich1ggJune16up[22]=
  {0  , 0  , 0  , 0  , 0  , //  5
   0  , 0  , 0.1, 0.1, 0.2, // 10
   0.3, 0.5, 0.7, 1.0, 1.3, // 15
   1.6, 2.0, 2.5, 3.0, 3.6, // 20
   4.2, 4.9                 // 22
  };

float Ich1ggJune16down[22]=
  {0  , 0  , 0  , 0  , 0  , //  5
   0  , 0  , 0  , 0  , 0  , // 10
   0.1, 0.2, 0.3, 0.7, 1.0, // 15
   1.4, 1.8, 2.3, 2.9, 3.4, // 20
   4.1, 4.9                 // 22
  };

float Ich2ggJune15[43]=
  {0  , 0  , 0  , 0  , 0  , //  5
   0  , 0  , 0  , 0  , 0  , // 10
   0  , 0.1, 0.1, 0.1, 0.1, // 15
   0.2, 0.2, 0.3, 0.3, 0.5, // 20
   0.6, 0.7, 0.9, 1.1, 1.4, // 25
   1.6, 1.8, 2.0, 2.1, 2.3, // 30
   2.6, 2.8, 3.0, 3.2, 3.4, // 35
   3.6, 3.8, 4.1, 4.3, 4.5, // 40
   4.7, 4.9, 5.1            // 43
  };

float Ich2maxJune15[30]=
  {0.2, 0.2, 0.3, 0.3, 0.4, //  5
   0.6, 0.6, 0.8, 1.1, 1.3, // 10
   1.5, 1.7, 1.9, 2.1, 2.3, // 15
   2.5, 2.5, 3.0, 3.1, 3.3, // 20
   3.6, 3.8, 4.0, 4.2, 4.6, // 25
   5.1, 5.5, 5.7, 6.4, 6.8  // 30
  };

float Ich2ggJune16up[44]=
  {0  , 0  , 0  , 0  , 0  , //  5
   0  , 0  , 0  , 0  , 0.1, // 10
   0  , 0.1, 0.1, 0.1, 0.1, // 15
   0.2, 0.3, 0.3, 0.4, 0.5, // 20
   0.6, 0.7, 0.8, 1.1, 1.3, // 25
   1.5, 1.6, 1.8, 2.1, 2.3, // 30
   2.4, 2.6, 2.8, 3.1, 3.3, // 35
   3.4, 3.6, 3.8, 4.1, 4.3, // 40
   4.4, 4.6, 4.8, 5.1       // 44
  };

float Ich2ggJune16down[44]=
  {0  , 0  , 0  , 0  , 0  , //  5
   0  , 0  , 0  , 0  , 0  , // 10
   0  , 0  , 0  , 0  , 0  , // 15
   0  , 0.1, 0.2, 0.2, 0.3, // 20
   0.4, 0.5, 0.7, 0.8, 1.0, // 25
   1.2, 1.5, 1.7, 1.9, 2.0, // 30
   2.2, 2.5, 2.7, 2.9, 3.1, // 35
   3.3, 3.5, 3.7, 3.9, 4.1, // 40
   4.3, 4.5, 4.7, 5.1       // 44
  };

float Ich4ggJune15[38]=
  {0  , 0  , 0  , 0  , 0  , //  5
   0.1, 0.1, 0.1, 0.1, 0.1, // 10
   0.1, 0.1, 0.1, 0.2, 0.2, // 15
   0.3, 0.3, 0.3, 0.3, 0.4, // 20
   0.5, 0.5, 0.6, 0.8, 0.9, // 25
   1.1, 1.2, 1.4, 1.5, 1.8, // 30
   2.0, 2.3, 2.6, 3.0, 3.3, // 35
   3.8, 4.5, 5.4            // 38
  };

float Ich4maxJune15[29]=
  {0.3, 0.3, 0.4, 0.5, 0.5, //  5
   0.6, 0.6, 0.7, 0.8, 1.1, // 10
   1.2, 1.4, 1.5, 1.8, 2.0, // 15
   2.3, 2.5, 2.8, 3.2, 3.6, // 20
   4.0, 4.6, 5.5, 6.2, 8.8, // 25
   11.5,15.3,18.9,24.4      // 29
  };

float Ich4ggJune16up[38]=
  {0  , 0  , 0  , 0  , 0  , //  5
   0.1, 0.1, 0.1, 0.1, 0.1, // 10
   0.2, 0.3, 0.3, 0.3, 0.4, // 15
   0.4, 0.5, 0.6, 0.6, 0.7, // 20
   0.8, 0.8, 0.9, 1.1, 1.3, // 25
   1.4, 1.6, 1.8, 2.0, 2.2, // 30
   2.5, 2.8, 3.1, 3.5, 3.9, // 35
   4.3, 4.9, 5.6            // 38
  };

float Ich4ggJune16down[38]=
  {0  , 0  , 0  , 0  , 0  , //  5
   0  , 0  , 0  , 0  , 0  , // 10
   0  , 0.1, 0.1, 0.2, 0.2, // 15
   0.2, 0.2, 0.3, 0.3, 0.4, // 20
   0.5, 0.5, 0.6, 0.7, 0.9, // 25
   1.0, 1.2, 1.3, 1.5, 1.7, // 30
   2.0, 2.3, 2.6, 2.9, 3.3, // 35
   3.8, 4.5, 5.5            // 38
  };


float Ich5ggJune15[32]=
  {0  , 0  , 0  , 0  , 0  , //  5
   0  , 0  , 0.1, 0.1, 0.2, // 10
   0.2, 0.3, 0.3, 0.3, 0.4, // 15
   0.5, 0.5, 0.7, 0.8, 0.9, // 20
   1.0, 1.1, 1.4, 1.6, 2.0, // 25
   2.3, 2.8, 3.3, 3.7, 4.3, // 30
   4.9, 5.6                 // 32
  };

float Ich5maxJune15[27]=
  {0.5, 0.5, 0.7, 0.8, 0.8, //  5
   1.0, 1.1, 1.3, 1.5, 1.9, // 10
   2.3, 2.7, 3.2, 3.7, 4.3, // 15
   5.0, 5.7, 6.3, 7.1, 7.9, // 20
   8.9, 9.9,11.0,12.1,14.6, // 25
   17.7,21.1                // 27
  };

float Ich5ggJune16up[32]=
  {0  , 0  , 0  , 0  , 0  , //  5
   0  , 0  , 0.1, 0.1, 0.2, // 10
   0.2, 0.3, 0.3, 0.3, 0.4, // 15
   0.5, 0.5, 0.7, 0.8, 0.8, // 20
   1.0, 1.0, 1.3, 1.5, 1.9, // 25
   2.2, 2.6, 3.1, 3.6, 4.2, // 30
   4.9, 5.6                 // 32
  };

float Ich5ggJune16down[32]=
  {0  , 0  , 0  , 0  , 0  , //  5
   0  , 0  , 0  , 0  , 0.1, // 10
   0.1, 0.1, 0.2, 0.2, 0.3, // 15
   0.4, 0.4, 0.5, 0.7, 0.7, // 20
   0.9, 0.9, 1.2, 1.5, 1.8, // 25
   2.1, 2.6, 3.0, 3.5, 4.1, // 30
   4.7, 5.5                 // 32
  };


struct measurements
{
  int june15_markerStyle;
  int june15max_markerStyle;
  int june16ggup_markerStyle;
  int june16ggdown_markerStyle;
  Int_t chamber1_ColorIndex;
  Int_t chamber2_ColorIndex;
  Int_t chamber4_ColorIndex;
  Int_t chamber5_ColorIndex;


  TGraph* gr_Ich1ggJune15;
  TGraph* gr_Ich2ggJune15;
  TGraph* gr_Ich4ggJune15;
  TGraph* gr_Ich5ggJune15;

  TGraph* gr_Ich1maxJune15;
  TGraph* gr_Ich2maxJune15;
  TGraph* gr_Ich4maxJune15;
  TGraph* gr_Ich5maxJune15;

  TGraph* gr_Ich1ggJune16up;
  TGraph* gr_Ich1ggJune16down;
  TGraph* gr_Ich2ggJune16up;
  TGraph* gr_Ich2ggJune16down;
  TGraph* gr_Ich4ggJune16up;
  TGraph* gr_Ich4ggJune16down;
  TGraph* gr_Ich5ggJune16up;
  TGraph* gr_Ich5ggJune16down;
  measurements(); 
};

measurements::measurements()
{
  june15_markerStyle=25; // open square
  june15max_markerStyle=24; // open circle
  june16ggup_markerStyle=26; // open triangle up
  june16ggdown_markerStyle=28; // open cross
  chamber1_ColorIndex=TColor::GetColor("#0000ff");
  chamber2_ColorIndex=TColor::GetColor("#00ff00");
  chamber4_ColorIndex=TColor::GetColor("#ff0000");
  chamber5_ColorIndex=TColor::GetColor("#00ffff");



  gr_Ich1ggJune15=new TGraph(22);
  gr_Ich1ggJune15->SetName("Graph1ggJune15");
  gr_Ich1ggJune15->SetTitle("");
  gr_Ich1ggJune15->SetLineColor(chamber1_ColorIndex);
  gr_Ich1ggJune15->SetMarkerColor(chamber1_ColorIndex);
  gr_Ich1ggJune15->SetMarkerStyle(june15_markerStyle);
  for (int i=0; i<22; ++i) gr_Ich1ggJune15->SetPoint(i,HVgg[i],Ich1ggJune15[i]);

  gr_Ich1ggJune16up=new TGraph(22);
  gr_Ich1ggJune16up->SetName("Graph1ggJune16up");
  gr_Ich1ggJune16up->SetTitle("");
  gr_Ich1ggJune16up->SetLineColor(chamber1_ColorIndex);
  gr_Ich1ggJune16up->SetMarkerColor(chamber1_ColorIndex);
  gr_Ich1ggJune16up->SetMarkerStyle(june16ggup_markerStyle);
  for (int i=0; i<22; ++i) gr_Ich1ggJune16up->SetPoint(i,HVgg[i],Ich1ggJune16up[i]);

  gr_Ich1ggJune16down=new TGraph(22);
  gr_Ich1ggJune16down->SetName("Graph1ggJune16down");
  gr_Ich1ggJune16down->SetTitle("");
  gr_Ich1ggJune16down->SetLineColor(chamber1_ColorIndex);
  gr_Ich1ggJune16down->SetMarkerColor(chamber1_ColorIndex);
  gr_Ich1ggJune16down->SetMarkerStyle(june16ggdown_markerStyle);
  for (int i=0; i<22; ++i) gr_Ich1ggJune16down->SetPoint(i,HVgg[i],Ich1ggJune16down[i]);


  gr_Ich2ggJune15=new TGraph(43);
  gr_Ich2ggJune15->SetName("Graph2ggJune15");
  gr_Ich2ggJune15->SetTitle("");
  gr_Ich2ggJune15->SetLineColor(chamber2_ColorIndex);
  gr_Ich2ggJune15->SetMarkerColor(chamber2_ColorIndex);
  gr_Ich2ggJune15->SetMarkerStyle(june15_markerStyle);
  for (int i=0; i<43; ++i) gr_Ich2ggJune15->SetPoint(i,HVgg[i],Ich2ggJune15[i]);

  gr_Ich2ggJune16up=new TGraph(44);
  gr_Ich2ggJune16up->SetName("Graph2ggJune16up");
  gr_Ich2ggJune16up->SetTitle("");
  gr_Ich2ggJune16up->SetLineColor(chamber2_ColorIndex);
  gr_Ich2ggJune16up->SetMarkerColor(chamber2_ColorIndex);
  gr_Ich2ggJune16up->SetMarkerStyle(june16ggup_markerStyle);
  for (int i=0; i<44; ++i) gr_Ich2ggJune16up->SetPoint(i,HVgg[i],Ich2ggJune16up[i]);

  gr_Ich2ggJune16down=new TGraph(44);
  gr_Ich2ggJune16down->SetName("Graph2ggJune16down");
  gr_Ich2ggJune16down->SetTitle("");
  gr_Ich2ggJune16down->SetLineColor(chamber2_ColorIndex);
  gr_Ich2ggJune16down->SetMarkerColor(chamber2_ColorIndex);
  gr_Ich2ggJune16down->SetMarkerStyle(june16ggdown_markerStyle);
  for (int i=0; i<44; ++i) gr_Ich2ggJune16down->SetPoint(i,HVgg[i],Ich2ggJune16down[i]);

  gr_Ich4ggJune15=new TGraph(38);
  gr_Ich4ggJune15->SetName("Graph4ggJune15");
  gr_Ich4ggJune15->SetTitle("");
  gr_Ich4ggJune15->SetLineColor(chamber4_ColorIndex);
  gr_Ich4ggJune15->SetMarkerColor(chamber4_ColorIndex);
  gr_Ich4ggJune15->SetMarkerStyle(june15_markerStyle);
  for (int i=0; i<38; ++i) gr_Ich4ggJune15->SetPoint(i,HVgg[i],Ich4ggJune15[i]);

  gr_Ich4ggJune16up=new TGraph(38);
  gr_Ich4ggJune16up->SetName("Graph4ggJune16up");
  gr_Ich4ggJune16up->SetTitle("");
  gr_Ich4ggJune16up->SetLineColor(chamber4_ColorIndex);
  gr_Ich4ggJune16up->SetMarkerColor(chamber4_ColorIndex);
  gr_Ich4ggJune16up->SetMarkerStyle(june16ggup_markerStyle);
  for (int i=0; i<38; ++i) gr_Ich4ggJune16up->SetPoint(i,HVgg[i],Ich4ggJune16up[i]);

  gr_Ich4ggJune16down=new TGraph(38);
  gr_Ich4ggJune16down->SetName("Graph4ggJune16down");
  gr_Ich4ggJune16down->SetTitle("");
  gr_Ich4ggJune16down->SetLineColor(chamber4_ColorIndex);
  gr_Ich4ggJune16down->SetMarkerColor(chamber4_ColorIndex);
  gr_Ich4ggJune16down->SetMarkerStyle(june16ggdown_markerStyle);
  for (int i=0; i<38; ++i) gr_Ich4ggJune16down->SetPoint(i,HVgg[i],Ich4ggJune16down[i]);

  gr_Ich5ggJune15=new TGraph(32);
  gr_Ich5ggJune15->SetName("Graph5ggJune15");
  gr_Ich5ggJune15->SetTitle("");
  gr_Ich5ggJune15->SetLineColor(chamber5_ColorIndex);
  gr_Ich5ggJune15->SetMarkerColor(chamber5_ColorIndex);
  gr_Ich5ggJune15->SetMarkerStyle(june15_markerStyle);
  for (int i=0; i<32; ++i) gr_Ich5ggJune15->SetPoint(i,HVgg[i],Ich5ggJune15[i]);

  gr_Ich5ggJune16up=new TGraph(32);
  gr_Ich5ggJune16up->SetName("Graph5ggJune16up");
  gr_Ich5ggJune16up->SetTitle("");
  gr_Ich5ggJune16up->SetLineColor(chamber5_ColorIndex);
  gr_Ich5ggJune16up->SetMarkerColor(chamber5_ColorIndex);
  gr_Ich5ggJune16up->SetMarkerStyle(june16ggup_markerStyle);
  for (int i=0; i<32; ++i) gr_Ich5ggJune16up->SetPoint(i,HVgg[i],Ich5ggJune16up[i]);

  gr_Ich5ggJune16down=new TGraph(32);
  gr_Ich5ggJune16down->SetName("Graph5ggJune16down");
  gr_Ich5ggJune16down->SetTitle("");
  gr_Ich5ggJune16down->SetLineColor(chamber5_ColorIndex);
  gr_Ich5ggJune16down->SetMarkerColor(chamber5_ColorIndex);
  gr_Ich5ggJune16down->SetMarkerStyle(june16ggdown_markerStyle);
  for (int i=0; i<32; ++i) gr_Ich5ggJune16down->SetPoint(i,HVgg[i],Ich5ggJune16down[i]);

  gr_Ich1maxJune15=new TGraph(15);
  gr_Ich1maxJune15->SetName("Graph1maxJune15");
  gr_Ich1maxJune15->SetTitle("");
  gr_Ich1maxJune15->SetLineColor(chamber1_ColorIndex);
  gr_Ich1maxJune15->SetMarkerColor(chamber1_ColorIndex);
  gr_Ich1maxJune15->SetMarkerStyle(june15max_markerStyle);
  for (int i=0; i<15; ++i) gr_Ich1maxJune15->SetPoint(i,HVmax[i],Ich1maxJune15[i]);

  gr_Ich2maxJune15=new TGraph(30);
  gr_Ich2maxJune15->SetName("Graph2maxJune15");
  gr_Ich2maxJune15->SetTitle("");
  gr_Ich2maxJune15->SetLineColor(chamber2_ColorIndex);
  gr_Ich2maxJune15->SetMarkerColor(chamber2_ColorIndex);
  gr_Ich2maxJune15->SetMarkerStyle(june15max_markerStyle);
  for (int i=0; i<30; ++i) gr_Ich2maxJune15->SetPoint(i,HVmax[i],Ich2maxJune15[i]);

  gr_Ich4maxJune15=new TGraph(29);
  gr_Ich4maxJune15->SetName("Graph4maxJune15");
  gr_Ich4maxJune15->SetTitle("");
  gr_Ich4maxJune15->SetLineColor(chamber4_ColorIndex);
  gr_Ich4maxJune15->SetMarkerColor(chamber4_ColorIndex);
  gr_Ich4maxJune15->SetMarkerStyle(june15max_markerStyle);
  for (int i=0; i<29; ++i) gr_Ich4maxJune15->SetPoint(i,HVmax[i],Ich4maxJune15[i]);

  gr_Ich5maxJune15=new TGraph(27);
  gr_Ich5maxJune15->SetName("Graph5maxJune15");
  gr_Ich5maxJune15->SetTitle("");
  gr_Ich5maxJune15->SetLineColor(chamber5_ColorIndex);
  gr_Ich5maxJune15->SetMarkerColor(chamber5_ColorIndex);
  gr_Ich5maxJune15->SetMarkerStyle(june15max_markerStyle);
  for (int i=0; i<27; ++i) gr_Ich5maxJune15->SetPoint(i,HVmax[i],Ich5maxJune15[i]);

 }


 
void myMacro()
{

  setTDRStyle();

  measurements A;
  
  DrawSomething D;
  D.beginCanva("Argon_current_june15","Argon current june 15");
  D.drawFake(5300,26);
  D.addGraph(A.gr_Ich1ggJune15,"chamber 1, June 15 7 pm");
  D.addGraph(A.gr_Ich1maxJune15,"chamber 1, June 15 11 pm");
  D.addGraph(A.gr_Ich2ggJune15,"chamber 2, June 15 7 pm");
  D.addGraph(A.gr_Ich2maxJune15,"chamber 2, June 15 11 pm");
  D.addGraph(A.gr_Ich4ggJune15,"chamber 4, June 15 7 pm");
  D.addGraph(A.gr_Ich4maxJune15,"chamber 4, June 15 11 pm");
  D.addGraph(A.gr_Ich5ggJune15,"chamber 5, June 15 7 pm");
  D.addGraph(A.gr_Ich5maxJune15,"chamber 5, June 15 11 pm");
  D.finalizeCanva();

  D.beginCanva("Argon_current_chamber_1","Argon current chamber 1");
  D.drawFake(2500,6);
  D.addGraph(A.gr_Ich1ggJune15,"June 15 7 pm, HV going up");
  D.addGraph(A.gr_Ich1ggJune16up,"June 16 11 am, HV going up");
  D.addGraph(A.gr_Ich1ggJune16down,"June 16 11 am, HV going down");
  D.finalizeCanva();

  D.beginCanva("Argon_current_chamber_2","Argon current chamber 2");
  D.drawFake(4600,6);
  D.addGraph(A.gr_Ich2ggJune15,"June 15 7 pm, HV going up");
  D.addGraph(A.gr_Ich2ggJune16up,"June 16 11 am, HV going up");
  D.addGraph(A.gr_Ich2ggJune16down,"June 16 11 am, HV going down");
  D.finalizeCanva();

  D.beginCanva("Argon_current_chamber_4","Argon current chamber 4");
  D.drawFake(4000,6);
  D.addGraph(A.gr_Ich4ggJune15,"June 15 7 pm, HV going up");
  D.addGraph(A.gr_Ich4ggJune16up,"June 16 11 am, HV going up");
  D.addGraph(A.gr_Ich4ggJune16down,"June 16 11 am, HV going down");
  D.finalizeCanva();

  D.beginCanva("Argon_current_chamber_5","Argon current chamber 5");
  D.drawFake(3400,6);
  D.addGraph(A.gr_Ich5ggJune15,"June 15 7 pm, HV going up");
  D.addGraph(A.gr_Ich5ggJune16up,"June 16 11 am, HV going up");
  D.addGraph(A.gr_Ich5ggJune16down,"June 16 11 am, HV going down");
  D.finalizeCanva();


}

