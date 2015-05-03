#include "LyonStepLinkingAlgo.hh"

LyonStepLinkingAlgo::LyonStepLinkingAlgo(std::vector<LyonTrackHit*> &vec)
{
  _inputStepList=vec;
  LinkStep();
}

void LyonStepLinkingAlgo::LinkStep()
{
  std::set< std::vector<LyonTrackHit*>::iterator > antiDoubleComptSet;
  for(std::vector<LyonTrackHit*>::iterator it=_inputStepList.begin(); it!=_inputStepList.end(); ++it){
    if( antiDoubleComptSet.find(it)!=antiDoubleComptSet.end() )
      continue;
    std::vector<LyonTrackHit*> hitListToMerge;
    hitListToMerge.push_back(*it);
    for(std::vector<LyonTrackHit*>::iterator jt=it+1; jt!=_inputStepList.end(); ++jt){
      if( (*jt)->shouldBeLinkedTo(*it) ){
	hitListToMerge.push_back(*jt);
	antiDoubleComptSet.insert(jt);
      }
    }
    MergeHitList(hitListToMerge);
  }
}

void LyonStepLinkingAlgo::MergeHitList(std::vector<LyonTrackHit*> &vec)
{
  LyonTrackHit* hit=(*vec.begin());
  if( vec.size()>1 )
    for(std::vector<LyonTrackHit*>::iterator it=vec.begin()+1; it!=vec.end(); ++it)
      hit->UpdateWith(*it);
  _outputStepList.push_back(hit);
}
