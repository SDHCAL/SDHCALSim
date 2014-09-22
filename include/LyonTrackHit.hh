#ifndef LyonTrackHit_h
#define LyonTrackHit_h

#include "G4VHit.hh"
#include "G4ThreeVector.hh"

class G4Step;

class LyonTrackHit : public G4VHit
{
public:
  LyonTrackHit(const G4Step* aStep,int replicaCountLevel);
  virtual ~LyonTrackHit() {;}
  // virtual void Draw();
  // virtual void Print();

private:
  G4double _energyDeposited;
  G4ThreeVector _entrancePoint;
  G4ThreeVector _exitPoint;
  G4int _replicaCopyNumber;
  G4ThreeVector _deltaPosition;
  G4bool _enteringStep;
  G4bool _leavingStep;
  G4int _pdgID;
  G4int _charge;
  G4double _time;
  G4int _trackid;//by rhan
  G4String processName;//by rhan
  G4int _parentid;//by rhan
  G4ThreeVector _momentum;//Anais
public:
  G4double energyDeposited() const {return _energyDeposited;} 
  const G4ThreeVector& entrancePoint() const {return _entrancePoint;}
  const G4ThreeVector& exitPoint() const {return _exitPoint;}
  const G4ThreeVector& deltaPosition() const {return _deltaPosition;}
  G4int replicaCopyNumber() const {return _replicaCopyNumber;}
  G4bool isEntering() const {return _enteringStep;}
  G4bool isLeaving() const {return _leavingStep;}
  G4int pdgID() const {return _pdgID; }
  G4int charge() const {return _charge; }
  G4double time() const {return _time; }
  G4int trackid() const{ return _trackid;}//by rhan
  G4int parentid() const{ return _parentid;}//by rhan
  const G4ThreeVector& momentum() const {return _momentum;}//Anais
};

#include "G4THitsCollection.hh"
typedef G4THitsCollection<LyonTrackHit> LyonTrackHitsCollection;

#endif
