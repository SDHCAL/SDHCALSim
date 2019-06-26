#include <TGraph.h>
#include <TMultiGraph.h>
#include "TCanvas.h"
#include "TLegend.h"


void RayonTempsmort ()
{
 TCanvas *c=new TCanvas;
 TGraph *gr4 = new TGraph(9);
    gr4->SetTitle("Low Resistive Glass");
    gr4->SetFillColor(1);
    gr4->SetLineColor(8);
    gr4->SetLineWidth(3); 
    gr4->SetMarkerColor(8);
    gr4->SetMarkerSize(2);
    gr4->SetPoint(0,1,10);
    gr4->SetPoint(1,1.5,4.5);
    gr4->SetPoint(2,2,2.5);
    gr4->SetPoint(3,2.5,1.7);
    gr4->SetPoint(4,3,0.95);
    gr4->SetPoint(5,3.5,0.8);
    gr4->SetPoint(6,4,0.65);
    gr4->SetPoint(7,4.5,0.55);
    gr4->SetPoint(8,5,0.4);

   TGraph *gr5 = new TGraph(9);
    gr5->SetTitle("Float Glass");
    gr5->SetFillColor(1);
    gr5->SetLineColor(6);
    gr5->SetLineWidth(3); 
    gr5->SetMarkerColor(6);
    gr5->SetMarkerSize(2);
    gr5->SetPoint(0,1,70);
    gr5->SetPoint(1,1.5,28);
    gr5->SetPoint(2,2,16);
    gr5->SetPoint(3,2.5,9.8);
    gr5->SetPoint(4,3,7.2);
    gr5->SetPoint(5,3.5,5.2);
    gr5->SetPoint(6,4,4);
    gr5->SetPoint(7,4.5,3.3);
    gr5->SetPoint(8,5,2.7);

 TMultiGraph *G=new TMultiGraph();
  G->Add(gr4,"AC*");
  G->Add(gr5,"AC*");
  G->SetTitle("Courbe d'evolution du temps mort en fonction du rayon mort ");
  G->GetXaxis()->SetTitle("Rayon mort (mm)");
  G->GetYaxis()->SetTitle("Temps mort (ms)");
  G->GetXaxis()->CenterTitle(1);
  G->GetYaxis()->CenterTitle(1);
  G->GetXaxis()->SetRangeUser(0.,5.);
  G->GetYaxis()->SetRangeUser(0.,70);
  G->Draw("A");
  c->BuildLegend();

return 0 ;
}



