#include "SDHCALSteppingAction.h"
#include "SDHCALDetectorConstruction.h"

#include <G4Step.hh>
#include <G4RunManager.hh>
#include <G4UnitsTable.hh>
#include <vector>

#include <stdexcept>

#include <G4PhysicalVolumeStore.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4Box.hh>
#include <g4root.hh>

SDHCALSteppingAction* SDHCALSteppingAction::instance = 0 ;

SDHCALSteppingAction::SDHCALSteppingAction()
	: G4UserSteppingAction()
{
	if (instance)
		throw std::logic_error("SDHCALSteppingAction already exists") ;

	G4cout << "Create SDHCALSteppingAction" << G4endl ;

	instance = this ;
	depositedEnergy = 0.0 ;
	leakEnergy = 0.0 ;
	lastStepTime = 0.0 ;
	interestedRegion = nullptr ;
}

void SDHCALSteppingAction::UserSteppingAction(const G4Step* step)
{
	G4Region* stepRegion = step->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetRegion() ;
	if (stepRegion != interestedRegion)
		return ;

	StepInfo stepInfo ;
	stepInfo.particleID = step->GetTrack()->GetDefinition()->GetPDGEncoding() ;
	stepInfo.energyDeposited = step->GetTotalEnergyDeposit() ;
	stepInfo.time = step->GetPostStepPoint()->GetGlobalTime() ;
	stepInfo.preStepPoint = step->GetPreStepPoint()->GetPosition() ;
	stepInfo.postStepPoint = step->GetPostStepPoint()->GetPosition() ;
	stepInfo.stepStatus = step->GetPostStepPoint()->GetStepStatus() ;

	if ( step->GetPostStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetRegion() != interestedRegion )
	{
		stepInfo.isLeaving = true ;
		if ( stepInfo.particleID >= 1000000000 || stepInfo.particleID == 2112 || stepInfo.particleID == 2212 ) //nucleons and nuclei
			stepInfo.leakingEnergy = step->GetPostStepPoint()->GetKineticEnergy() ;
		else
			stepInfo.leakingEnergy = step->GetPostStepPoint()->GetTotalEnergy() ;
	}
	else
	{
		stepInfo.isLeaving = false ;
		stepInfo.leakingEnergy = 0.0 ;
	}

	steps.push_back( stepInfo ) ;
}

void SDHCALSteppingAction::processSteps()
{
	for ( std::vector<StepInfo>::iterator it = steps.begin() ; it != steps.end() ; ++it )
	{
		depositedEnergy += it->energyDeposited ;
		leakedEnergy += it->leakingEnergy ;

		depositedEnergyPerParticleType[it->particleID] += it->energyDeposited ;
		leakedEnergyPerParticleType[it->particleID] += it->leakingEnergy ;
	}
}
G4double SDHCALSteppingAction::getEMFraction() const
{
	G4double emDeposited = 0.0 ;

	int list[3] = {-11 , 11 , 22} ;
	for ( int i = 0 ; i < 3 ; i++ )
	{
		std::map<G4int , G4double>::const_iterator it = depositedEnergyPerParticleType.find( list[i] )  ;
		if ( it != depositedEnergyPerParticleType.end() )
			emDeposited += it->second ;
	}

	return emDeposited/depositedEnergy ;
}

void SDHCALSteppingAction::PrintTableauEnergie()
{
	for (std::map<G4int,G4double>::iterator it=depositedEnergyPerParticleType.begin() ; it != depositedEnergyPerParticleType.end() ; ++it)
		G4cout << it->first << " = " << it->second/CLHEP::MeV << " MeV" << G4endl ;
}

void SDHCALSteppingAction::PrintTableauLeakEnergie()
{
	for (std::map<G4int,G4double>::iterator it=leakedEnergyPerParticleType.begin() ; it != leakedEnergyPerParticleType.end() ; ++it)
		G4cout << it->first << " = " << it->second/CLHEP::MeV << " MeV" << G4endl ;
}

void SDHCALSteppingAction::reset()
{
	steps.clear() ;

	depositedEnergy = 0.0 ;
	leakedEnergy = 0.0 ;

	depositedEnergyPerParticleType.clear() ;
	leakedEnergyPerParticleType.clear() ;


	sideleakEnergy = 0.0 ;
	frontleakEnergy = 0.0 ;
	rearleakEnergy = 0.0 ;
	lastStepTime = 0.0 ;
}
