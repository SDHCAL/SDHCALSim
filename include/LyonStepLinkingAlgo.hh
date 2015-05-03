#ifndef LyonStepLinkingAlgo_h
#define LyonStepLinkingAlgo_h

#include <iostream>
#include <vector>
#include "LyonTrackHit.hh"
#include <set>

class LyonStepLinkingAlgo 
{
public:
  LyonStepLinkingAlgo(std::vector<LyonTrackHit*> &vec);
  void LinkStep();
  void MergeHitList(std::vector<LyonTrackHit*> &vec);
  inline std::vector<LyonTrackHit*> &getLinkedStepList(){return _outputStepList;}
  virtual ~LyonStepLinkingAlgo() {;}

private:
  std::vector<LyonTrackHit*> _inputStepList;
  std::vector<LyonTrackHit*> _outputStepList;

};

#endif
