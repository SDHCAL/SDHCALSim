#pragma once

#include "G4Step.hh"
#include "G4UserSteppingAction.hh"
#include "G4Region.hh"
#include "G4ThreeVector.hh"

#include <vector>
#include <map>

class StepInfo
{
public:
  G4double m_EnergyDeposited{0.};
  G4double m_Time{0.};
  G4ThreeVector m_PreStepPoint{0.,0.,0.};
  G4ThreeVector m_PostStepPoint{0.,0.,0.};
  G4double m_LeakingEnergy{0.};
  G4StepStatus m_StepStatus{};
  G4int m_ParticleID{0};
  G4bool m_IsLeaving{false};
};

class SDHCALSteppingAction : public G4UserSteppingAction
{
public:
  SDHCALSteppingAction();
  virtual ~SDHCALSteppingAction()=default;
  virtual void UserSteppingAction(const G4Step* step);

  void reset();
  void processSteps();

  G4double getDepositedEnergy() const { return depositedEnergy ; }
  G4double getLeakedEnergy() const { return leakedEnergy ; }

  G4double getEMFraction() const;
  std::map<G4int , G4double> getDepositedEnergyPerParticleType() const { return depositedEnergyPerParticleType; }
  std::map<G4int , G4double> getLeakedEnergyPerParticleType() const { return leakedEnergyPerParticleType ; }

  void PrintTableauEnergie();
  void PrintTableauLeakEnergie();

 // inline G4double getLastStepTime() const { return lastStepTime ; }
  inline const std::vector<StepInfo>& getSteps() const { return steps ; }

private:
  G4Region* m_InterestedRegion{nullptr};
  G4double depositedEnergy{0.};
  G4double leakedEnergy{0.};

  std::map<G4int , G4double> depositedEnergyPerParticleType;
  std::map<G4int , G4double> leakedEnergyPerParticleType;

  G4double leakEnergy{0.};

  std::vector<StepInfo> steps;
};
