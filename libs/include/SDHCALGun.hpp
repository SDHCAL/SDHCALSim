#pragma once

#include "G4ParticleGun.hh"

#include "json.hpp"

class SDHCALDetectorConstruction;

class SDHCALGun : public G4ParticleGun
{
public:
  SDHCALGun()=default;
  SDHCALGun(nlohmann::json json,SDHCALDetectorConstruction*) ;
  ~SDHCALGun() = default ;
  void generatePrimary(G4Event* event) ;

private:
  SDHCALDetectorConstruction* m_Detector{nullptr};
  void shootPosition();
  void shootMomentum();
  void shootForCosmic();
  void shootEnergy() ;
  G4String particleName{"pi-"};
  G4String gunOptionEnergyDistribution{"fixed"};

  G4double particleEnergy{30*CLHEP::GeV};
  G4double sigmaEnergy{0.1*CLHEP::GeV};
  G4double minEnergy{30*CLHEP::GeV};
  G4double maxEnergy{30*CLHEP::GeV};

  G4bool cosmicGun{false};

  G4String gunOptionPosition{"fixed"};

  G4double meanPositionX{0.0*CLHEP::mm};
  G4double meanPositionY{0.0*CLHEP::mm};
  G4double meanPositionZ{-20.0*CLHEP::mm};
  G4double uniformMaxPosition{0.0*CLHEP::mm}; //uniform position
  G4double sigmaPosition{1*CLHEP::mm}; //gaussian position

  G4String gunOptionMomentum{"fixed"};
  G4ThreeVector primaryMom{};

  G4double gaussianMomentumSigma{0.1};
  G4double momentumTheta{0};
  G4double momentumPhi{0};

  G4double m_Time{0*CLHEP::ns};
};
