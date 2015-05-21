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
    if( !(*it)->isLeaving() )
      for(std::vector<LyonTrackHit*>::iterator jt=it+1; jt!=_inputStepList.end(); ++jt){
	if( (*jt)->shouldBeLinkedTo(*it) ){
	  hitListToMerge.push_back(*jt);
	  antiDoubleComptSet.insert(jt);
	  if( (*jt)->isLeaving() )break;
	}
      }
    MergeHitList(hitListToMerge);
  }
}

//#define DEBUG
void LyonStepLinkingAlgo::MergeHitList(std::vector<LyonTrackHit*> &vec)
{
  LyonTrackHit* hit=(*vec.begin());
  if( vec.size()>1 ){
#ifdef DEBUG
    G4cout << "FIRST \t _replicaCopyNumber = " << hit->replicaCopyNumber() << "\t"
	   << "_enteringStep = " << hit->isEntering() << "\t" 
	   << "_leavingStep = " << hit->isLeaving() << "\t"
	   << "_trackID = " << hit->trackid() << "\t"
	   << "entrancePoint() = " << hit->entrancePoint() << "\t" 
	   << "exitPoint() = " << hit->exitPoint() << G4endl;
    if( !hit->isEntering() && !hit->isLeaving() )
      G4cout << hit->trackVertexVolumeName() << G4endl;
#endif
    for(std::vector<LyonTrackHit*>::iterator it=vec.begin()+1; it!=vec.end(); ++it){
#ifdef DEBUG
      G4cout << "_replicaCopyNumber = " << (*it)->replicaCopyNumber() << "\t"
	     << "_enteringStep = " << (*it)->isEntering() << "\t" 
	     << "_leavingStep = " << (*it)->isLeaving() << "\t" 
	     << "_trackID = " << (*it)->trackid() << " " 
	     << "entrancePoint() = " << (*it)->entrancePoint() << " " 
	     << "exitPoint() = " << (*it)->exitPoint() << G4endl;
#endif
      hit->UpdateWith(*it);
    }
  }
  _outputStepList.push_back(hit);
}
