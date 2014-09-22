#ifndef LyonSensitiveDetector_h
#define LyonSensitiveDetector_h

#include "G4VSensitiveDetector.hh"
#include "LyonTrackHit.hh"
class G4Step;
class G4HCofThisEvent;

class LyonSensitiveDetector : public G4VSensitiveDetector
{
public:
  LyonSensitiveDetector(G4String name,int replicaCountLevel);
  virtual ~LyonSensitiveDetector() {;}
  virtual void Initialize(G4HCofThisEvent*);
  virtual G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  virtual void EndOfEvent(G4HCofThisEvent*){;}

  //add by rhan
  virtual G4bool Interested(const G4Step* aStep) const;
private:
  LyonTrackHitsCollection * hitsCollection;
  G4int collectionID;
  G4int _replicaCountLevel;
  //add by rhan
  G4String processName;

};

#endif
