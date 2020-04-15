#pragma once

#include "G4VSensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"

#include "SDHCALHit.hpp"

class SDHCALRPCSensitiveDetector : public G4VSensitiveDetector
{
public:
  void setSizes(const G4double& X,const G4double& Y, const G4double& Z)
  {
    m_SizeX=X;
    m_SizeY=Y;
    m_SizeZ=Z;
  }
  void setCellXYSize(const G4double& X,const G4double& Y)
  {
    m_CellSizeX=X;
    m_CellSizeY=Y;
  }
  SDHCALRPCSensitiveDetector(G4String name ,const G4int& id);
  virtual ~SDHCALRPCSensitiveDetector()=default;
  virtual void Initialize(G4HCofThisEvent*);
  virtual G4bool ProcessHits(G4Step* step , G4TouchableHistory*);
  virtual void EndOfEvent(G4HCofThisEvent*);
  virtual G4bool Interested(const G4Step* step) const;
  void finalizeLastHit();

private:
  SDHCALHitCollection* hitsCollection {} ;
  G4int m_ID{0};
  SDHCALHit* currentHit {};
  G4double m_SizeX{0.};
  G4double m_SizeY{0.};
  G4double m_SizeZ{0.};
  G4double m_CellSizeX{0.};
  G4double m_CellSizeY{0.};
};
