#include "SDHCAL_Simu_EventAnalyser.h"

#include <iostream>

#include "TFile.h"
#include "TH1I.h"


SDHCAL_Simu_EventAnalyser::SDHCAL_Simu_EventAnalyser(const char* simHits, const char* recoHits,  const char* steps) :
  _recoHits(recoHits), 
  _simHits(simHits), 
  _steps(steps),
  _OKreco(false),
  _OKsim(false),
  _OKsteps(false),
  _totalEvents(0)
{}



void SDHCAL_Simu_EventAnalyser::newEvent(LCEvent *event)
{
  ++_totalEvents;
  _OKreco=_recoHits.load(event);
  _OKsim=_simHits.load(event);
  _OKsteps=_steps.load(event);

  if (_OKreco) _recoHitsStat.Add(_recoHits.vector());
  if (_OKsim) _simHitsStat.Add(_simHits.vector());
}


void SDHCAL_Simu_EventAnalyser::printStat()
{
  std::cout << "SDHCAL_Simu_EventAnalyser report" << std::endl;
  std::cout << "Have processed " << _totalEvents << " events." << std::endl;
  std::cout << "SimHits report :" << std::endl;
  _simHitsStat.printDistr();
  std::cout << "RecoHits report :" << std::endl;
  _recoHitsStat.printDistr();
}


void SDHCAL_Simu_EventAnalyser::writeROOTfile(const char* filename)
{
  TFile rootOutput(filename,"RECREATE");
  rootOutput.mkdir("SimHits");
  rootOutput.cd("SimHits");
  _simHitsStat.saveInHisto();
  rootOutput.mkdir("RecoHits");
  rootOutput.cd("RecoHits");
  _recoHitsStat.saveInHisto();
  rootOutput.Close();
}


void SDHCALmakeHisto(const char *name, const char* title,std::map<unsigned int,int>& data,unsigned int minNumberOfBin,unsigned int maxNumberOfBin)
{
  unsigned int maxDataValue=data.rbegin()->first;
  unsigned int nBin=maxDataValue+1;
  if (minNumberOfBin != 0 && minNumberOfBin>nBin) nBin=minNumberOfBin;
  if (maxNumberOfBin != 0 && maxNumberOfBin>minNumberOfBin && maxNumberOfBin<nBin) nBin=maxNumberOfBin;
  TH1I h(name,title,nBin,0,nBin);
  for (std::map<unsigned int,int>::iterator it=data.begin(); it != data.end(); ++it)
    h.Fill(it->first,it->second);
  h.Write();
}

void SDHCALmakeHisto(const char *name, const char* title,std::map<float,int>& data,unsigned int NBin,float maxValue)
{
  float max=1.1*data.rbegin()->first;
  if (maxValue!=0 && max>maxValue) max=maxValue;
  TH1F h(name,title,NBin,0,max);
  for (std::map<float,int>::iterator it=data.begin(); it != data.end(); ++it)
    h.Fill(it->first,it->second);
  h.Write();
}
