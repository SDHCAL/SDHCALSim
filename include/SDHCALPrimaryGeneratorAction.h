#ifndef SDHCALPrimaryGeneratorAction_h
#define SDHCALPrimaryGeneratorAction_h

#include <G4ParticleGun.hh>
#include <G4VUserPrimaryGeneratorAction.hh>

#include <G4ParticleTable.hh>

#include "SDHCALPrimaryGeneratorActionMessenger.h"
#include "SDHCALGun.h"

class SDHCALPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
	public :
		SDHCALPrimaryGeneratorAction() ;
		SDHCALPrimaryGeneratorAction( std::string xmlFileName ) ;
		virtual ~SDHCALPrimaryGeneratorAction() ;

		void setParticleDefinition(std::string particleName)
		{
			G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable() ;
			particleDefinition = particleTable->FindParticle( particleName ) ;
		}

		inline void setGunOptionPosition(std::string option) { gunOptionPosition = option ; }
		inline void setGunOptionMomentum(std::string option) { gunOptionMomentum = option ; }
		inline void setGunOptionEnergy(std::string option) { gunOptionEnergyDistribution = option ; }

		inline void setMeanPositionX(G4double position) { meanPositionX = position ; }
		inline void setMeanPositionY(G4double position) { meanPositionY = position ; }
		inline void setMeanPositionZ(G4double position) { meanPositionZ = position ; }
		inline void setUniformMaxPosition(G4double maxPosition) { uniformMaxPosition = maxPosition ; }
		inline void setSigmaPosition(G4double sigma) { sigmaPosition = sigma ; }

		inline void SetMomentumTheta(G4double theta) { momentumTheta = theta ; }
		inline void SetMomentumPhi(G4double phi) { momentumPhi = phi ; }
		inline void setGaussianMomentumSigma(G4double momentumSigma) { gaussianMomentumSigma = momentumSigma ; }

		inline void setParticleEnergy(G4double energy) { particleEnergy = energy * CLHEP::GeV ; }
		inline void setSigmaEnergy(G4double energy) { sigmaEnergy = energy * CLHEP::GeV ; }
		inline void setMinEnergy(G4double energy) { minEnergy = energy * CLHEP::GeV ; }
		inline void setMaxEnergy(G4double energy) { maxEnergy = energy * CLHEP::GeV ; }





		inline const G4ThreeVector& getPrimaryPos() const { return primaryPos ; }
		inline const G4ThreeVector& getPrimaryMom() const { return primaryMom ; }
		inline G4double getPrimaryEnergy() const { return primaryEnergy ; }
		inline const G4ParticleDefinition* getParticleDefinition() const { return particleDefinition ; }

		void print() const ;


		virtual void GeneratePrimaries(G4Event* event) ;

		const std::vector<SDHCALGun*>& getGunVec() const { return gunVec ; }

	protected :
		SDHCALPrimaryGeneratorActionMessenger* messenger ;

		std::vector<SDHCALGun*> gunVec = {} ;

		SDHCALGunOptions opt ;

		std::vector<G4ParticleGun*> particleGunVec = {} ;


		G4ParticleDefinition* particleDefinition ;
		G4ThreeVector primaryMom ;
		G4ThreeVector primaryPos ;
		G4double primaryEnergy ;

		G4String gunOptionPosition ;
		G4String gunOptionMomentum ;
		G4String gunOptionEnergyDistribution ;


		G4double meanPositionX ;
		G4double meanPositionY ;
		G4double meanPositionZ ;
		G4double uniformMaxPosition ; //uniform position
		G4double sigmaPosition ; //gaussian position


		G4double gaussianMomentumSigma ;
		G4double momentumTheta ;
		G4double momentumPhi ;

		G4double particleEnergy ;
		G4double sigmaEnergy ;
		G4double minEnergy ;
		G4double maxEnergy ;

		G4double caloFrontZ ;

} ;


#endif //SDHCALPrimaryGeneratorAction_h
