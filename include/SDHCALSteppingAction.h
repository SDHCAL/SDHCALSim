#ifndef SDHCALSteppingAction_h
#define SDHCALSteppingAction_h

#include <G4Step.hh>
#include <G4UserSteppingAction.hh>
#include <G4ThreeVector.hh>
#include <G4RegionStore.hh>
#include <G4VPhysicalVolume.hh>
#include <G4AffineTransform.hh>
#include <globals.hh>

#include <vector>
#include <map>

class G4LogicalVolume ;
class G4Region ;

class LyonDetectorConstruction ;

struct StepInfo
{
	G4double energyDeposited ;
	G4double time ;
	G4ThreeVector preStepPoint ;
	G4ThreeVector postStepPoint ;
	G4double leakingEnergy ;
	G4StepStatus stepStatus ;
	G4int particleID ;
	G4bool isLeaving ;
} ;

class SDHCALSteppingAction : public G4UserSteppingAction
{
	//singleton stuff
		static SDHCALSteppingAction* instance ;
	protected :
		SDHCALSteppingAction() ;
		static void initInstance() { new SDHCALSteppingAction ; }
		virtual ~SDHCALSteppingAction() { instance = nullptr ; }
	public :
		static SDHCALSteppingAction* Instance()
		{
			if ( !instance )
				initInstance() ;
			return instance ;
		}



	public :

		virtual void UserSteppingAction(const G4Step* step) ;

		void reset() ;
		void processSteps() ;

		inline void setInterestedRegion(G4Region* region) { interestedRegion = region ; }
		inline void setPhysicalVolume(G4VPhysicalVolume* volume) { calorimeter = volume ; transform = G4AffineTransform( volume->GetObjectRotationValue() , volume->GetObjectTranslation() ) ; transform.Invert() ; }
		G4ThreeVector transformInCaloCoord(G4ThreeVector worldCoord) { return transform.TransformPoint(worldCoord) ; }

		G4double getDepositedEnergy() const { return depositedEnergy ; }
		G4double getLeakedEnergy() const { return leakedEnergy ; }

		G4double getEMFraction() const ;


		G4double GetSideLeakEnergy() const { return sideleakEnergy ; }
		G4double GetFrontLeakEnergy() const { return frontleakEnergy ; }
		G4double GetRearLeakEnergy() const { return rearleakEnergy ; }

		std::map<G4int , G4double> getDepositedEnergyPerParticleType() const { return depositedEnergyPerParticleType ; }
		std::map<G4int , G4double> getLeakedEnergyPerParticleType() const { return leakedEnergyPerParticleType ; }

		void PrintTableauEnergie() ;
		void PrintTableauLeakEnergie() ;

		inline G4double getLastStepTime() const { return lastStepTime ; }
		inline const std::vector<StepInfo>& getSteps() const { return steps ; }


	private :
		G4Region* interestedRegion ;
		G4VPhysicalVolume* calorimeter ;
		G4AffineTransform transform ;


		G4double depositedEnergy ;
		G4double leakedEnergy ;

		std::map<G4int , G4double> depositedEnergyPerParticleType ;
		std::map<G4int , G4double> leakedEnergyPerParticleType ;

		G4double leakEnergy ;
		G4double sideleakEnergy ;
		G4double frontleakEnergy ;
		G4double rearleakEnergy ;

		G4double lastStepTime ;

		std::vector<StepInfo> steps ;
} ;

#endif //SDHCALSteppingAction_h
