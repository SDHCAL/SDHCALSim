#include "SDHCAL_Simu_EventAnalyser.h"

#include <iostream>


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
