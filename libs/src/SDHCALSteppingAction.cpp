#include "SDHCALSteppingAction.hpp"
#include "SDHCALDetectorConstruction.hpp"

#include "G4RegionStore.hh"
#include "G4Step.hh"

SDHCALSteppingAction::SDHCALSteppingAction():G4UserSteppingAction(),m_InterestedRegion(G4RegionStore::GetInstance()->FindOrCreateRegion("RegionCalorimeter"))
{
}

void SDHCALSteppingAction::UserSteppingAction(const G4Step* step)
{
  G4Region* stepRegion = step->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetRegion();
  if(stepRegion != m_InterestedRegion) return;

  StepInfo stepInfo ;
  stepInfo.m_ParticleID = step->GetTrack()->GetDefinition()->GetPDGEncoding();
  stepInfo.m_EnergyDeposited = step->GetTotalEnergyDeposit();
  stepInfo.m_Time = step->GetPostStepPoint()->GetGlobalTime();
  stepInfo.m_PreStepPoint = step->GetPreStepPoint()->GetPosition();
  stepInfo.m_PostStepPoint = step->GetPostStepPoint()->GetPosition();
  stepInfo.m_StepStatus = step->GetPostStepPoint()->GetStepStatus();

  if( step->GetPostStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetRegion() != m_InterestedRegion)
  {
    stepInfo.m_IsLeaving = true;
    if(stepInfo.m_ParticleID >= 1000000000 || stepInfo.m_ParticleID == 2112 || stepInfo.m_ParticleID == 2212)
    {
      //nucleons and nuclei
      stepInfo.m_LeakingEnergy = step->GetPostStepPoint()->GetKineticEnergy();
    }
    else stepInfo.m_LeakingEnergy = step->GetPostStepPoint()->GetTotalEnergy();
  }
  else
  {
    stepInfo.m_IsLeaving = false;
    stepInfo.m_LeakingEnergy = 0.0;
  }
  steps.push_back( stepInfo );
}

void SDHCALSteppingAction::processSteps()
{
  for(std::vector<StepInfo>::iterator it = steps.begin() ; it != steps.end() ; ++it )
  {
    depositedEnergy += it->m_EnergyDeposited ;
    leakedEnergy += it->m_LeakingEnergy ;

    depositedEnergyPerParticleType[it->m_ParticleID] += it->m_EnergyDeposited;
    leakedEnergyPerParticleType[it->m_ParticleID] += it->m_LeakingEnergy;
  }
}

G4double SDHCALSteppingAction::getEMFraction() const
{
  G4double emDeposited = 0.0 ;
  int list[3] = {-11 , 11 , 22} ;
  for ( int i = 0 ; i < 3 ; i++ )
  {
    std::map<G4int , G4double>::const_iterator it = depositedEnergyPerParticleType.find( list[i] )  ;
    if ( it != depositedEnergyPerParticleType.end() ) emDeposited += it->second ;
  }
  return emDeposited/depositedEnergy;
}

void SDHCALSteppingAction::PrintTableauEnergie()
{
  for(std::map<G4int,G4double>::iterator it=depositedEnergyPerParticleType.begin() ; it != depositedEnergyPerParticleType.end() ; ++it)
  G4cout << it->first << " = " << it->second/CLHEP::MeV << " MeV" << G4endl ;
}

void SDHCALSteppingAction::PrintTableauLeakEnergie()
{
  for(std::map<G4int,G4double>::iterator it=leakedEnergyPerParticleType.begin() ; it != leakedEnergyPerParticleType.end() ; ++it)
  G4cout << it->first << " = " << it->second/CLHEP::MeV << " MeV" << G4endl ;
}

void SDHCALSteppingAction::reset()
{
  steps.clear();
  depositedEnergy=0.0;
  leakedEnergy=0.0; 
  
  depositedEnergyPerParticleType.clear();
  leakedEnergyPerParticleType.clear();
}
