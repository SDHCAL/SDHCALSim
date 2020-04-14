#pragma once

#include <G4ThreeVector.hh>
#include <G4UserTrackingAction.hh>
#include <globals.hh>

#include <map>

class SDHCALTrackingAction : public G4UserTrackingAction
{
public:
  SDHCALTrackingAction();
  virtual ~SDHCALTrackingAction()=default;
  struct PrimaryParticleInfo
  {
    G4int id{0};
    G4int pdg{0};
    G4ThreeVector momentum;
  };

  virtual void PreUserTrackingAction(const G4Track* track);
  G4int getPrimaryParent(const G4int& trackID) const;
  PrimaryParticleInfo getPrimaryParticleInfo(G4int id) const;
  void reset() ;
  
private:
  std::map<G4int,PrimaryParticleInfo> primaryTrackMap;
  std::map<G4int,G4int> linkMap;
};
