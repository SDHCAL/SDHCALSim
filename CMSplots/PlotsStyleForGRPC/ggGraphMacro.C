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
  enum CHAMBER {CH1,CH2,CH4,CH5, NCHAMBERS};
  enum MEASURE {June15gg, June15max, June16up, June16down, NMEASURES};

  inline unsigned int I(unsigned int chamber,unsigned int measure) {return NCHAMBERS*chamber+measure;}
  int  measure_markerstyle[NMEASURES];
  Int_t chamber_ColorIndex[NCHAMBERS];
  TGraph* allGraphes[NCHAMBERS*NMEASURES];
  TGraph* graph(unsigned int chamber,unsigned int measure) {return allGraphes[I(chamber,measure)];}
  
  TGraph* createGraph(const char *name, unsigned int chamber, unsigned int measure, int N, float *Ipoint, float *HVpoint);

  measurements(); 
};

TGraph*  measurements::createGraph(const char *name, unsigned int chamber, unsigned int measure, int N, float *Ipoint, float *HVpoint)
{
  TGraph *gr = allGraphes[I(chamber,measure)] = new TGraph(N);
  gr->SetName(name);
  gr->SetTitle("");
  gr->SetLineColor(chamber_ColorIndex[chamber]);
  gr->SetMarkerColor(chamber_ColorIndex[chamber]);
  gr->SetMarkerStyle(measure_markerstyle[measure]);
  for (int i=0; i<N; ++i) gr->SetPoint(i,HVpoint[i],Ipoint[i]);
  return gr;
}

measurements::measurements()
{
  measure_markerstyle[June15gg]=25;   // open square
  measure_markerstyle[June15max]=24;  // open circle
  measure_markerstyle[June16up]=26;   // open triangle up
  measure_markerstyle[June16down]=28; // open cross
  chamber_ColorIndex[CH1]=TColor::GetColor("#0000ff");
  chamber_ColorIndex[CH2]=TColor::GetColor("#00ff00");
  chamber_ColorIndex[CH4]=TColor::GetColor("#ff0000");
  chamber_ColorIndex[CH5]=TColor::GetColor("#00ffff");



  createGraph("Graph1ggJune15",CH1,June15gg,22,Ich1ggJune15,HVgg);
  createGraph("Graph1ggJune16up",CH1,June16up,22,Ich1ggJune16up,HVgg);
  createGraph("Graph1ggJune16down",CH1,June16down,22,Ich1ggJune16down,HVgg);

  createGraph("Graph2ggJune15",CH2,June15gg,43,Ich2ggJune15,HVgg);
  createGraph("Graph2ggJune16up",CH2,June16up,44,Ich2ggJune16up,HVgg);
  createGraph("Graph2ggJune16down",CH2,June16down,44,Ich2ggJune16down,HVgg);

  createGraph("Graph4ggJune15",CH4,June15gg,38,Ich4ggJune15,HVgg);
  createGraph("Graph4ggJune16up",CH4,June16up,38,Ich4ggJune16up,HVgg);
  createGraph("Graph4ggJune16down",CH4,June16down,38,Ich4ggJune16down,HVgg);

  createGraph("Graph5ggJune15",CH5,June15gg,32,Ich5ggJune15,HVgg);
  createGraph("Graph5ggJune16up",CH5,June16up,32,Ich5ggJune16up,HVgg);
  createGraph("Graph5ggJune16down",CH5,June16down,32,Ich5ggJune16down,HVgg);

  createGraph("Graph1maxJune15",CH1,June15max,15,Ich1maxJune15,HVmax);
  createGraph("Graph2maxJune15",CH2,June15max,30,Ich2maxJune15,HVmax);
  createGraph("Graph4maxJune15",CH4,June15max,29,Ich4maxJune15,HVmax);
  createGraph("Graph5maxJune15",CH5,June15max,27,Ich5maxJune15,HVmax);
 }


 
void myMacro()
{

  setTDRStyle();

  measurements A;
  
  DrawSomething D;
  D.beginCanva("Argon_current_june15","Argon current june 15");
  D.drawFake(5300,26);
  D.addGraph(A.graph(measurements::CH1,measurements::June15gg),"chamber 1, June 15 7 pm");
  D.addGraph(A.graph(measurements::CH1,measurements::June15max),"chamber 1, June 15 11 pm");
  D.addGraph(A.graph(measurements::CH2,measurements::June15gg),"chamber 2, June 15 7 pm");
  D.addGraph(A.graph(measurements::CH2,measurements::June15max),"chamber 2, June 15 11 pm");
  D.addGraph(A.graph(measurements::CH4,measurements::June15gg),"chamber 4, June 15 7 pm");
  D.addGraph(A.graph(measurements::CH4,measurements::June15max),"chamber 4, June 15 11 pm");
  D.addGraph(A.graph(measurements::CH5,measurements::June15gg),"chamber 5, June 15 7 pm");
  D.addGraph(A.graph(measurements::CH5,measurements::June15max),"chamber 5, June 15 11 pm");
  D.finalizeCanva();

  D.beginCanva("Argon_current_chamber_1","Argon current chamber 1");
  D.drawFake(2500,6);
  D.addGraph(A.graph(measurements::CH1,measurements::June15gg),"June 15 7 pm, HV going up");
  D.addGraph(A.graph(measurements::CH1,measurements::June16up),"June 16 11 am, HV going up");
  D.addGraph(A.graph(measurements::CH1,measurements::June16down),"June 16 11 am, HV going down");
  D.finalizeCanva();

  D.beginCanva("Argon_current_chamber_2","Argon current chamber 2");
  D.drawFake(4600,6);
  D.addGraph(A.graph(measurements::CH2,measurements::June15gg),"June 15 7 pm, HV going up");
  D.addGraph(A.graph(measurements::CH2,measurements::June16up),"June 16 11 am, HV going up");
  D.addGraph(A.graph(measurements::CH2,measurements::June16down),"June 16 11 am, HV going down");
  D.finalizeCanva();

  D.beginCanva("Argon_current_chamber_4","Argon current chamber 4");
  D.drawFake(4000,6);
  D.addGraph(A.graph(measurements::CH4,measurements::June15gg),"June 15 7 pm, HV going up");
  D.addGraph(A.graph(measurements::CH4,measurements::June16up),"June 16 11 am, HV going up");
  D.addGraph(A.graph(measurements::CH4,measurements::June16down),"June 16 11 am, HV going down");
  D.finalizeCanva();

  D.beginCanva("Argon_current_chamber_5","Argon current chamber 5");
  D.drawFake(3400,6);
  D.addGraph(A.graph(measurements::CH5,measurements::June15gg),"June 15 7 pm, HV going up");
  D.addGraph(A.graph(measurements::CH5,measurements::June16up),"June 16 11 am, HV going up");
  D.addGraph(A.graph(measurements::CH5,measurements::June16down),"June 16 11 am, HV going down");
  D.finalizeCanva();


}

