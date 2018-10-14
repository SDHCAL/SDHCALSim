#include "SDHCALPrimaryGeneratorAction.h"

#include "SDHCALDetectorConstruction.h"

#include <G4Event.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4PrimaryParticle.hh>
#include <G4PrimaryVertex.hh>
#include <G4SystemOfUnits.hh>
#include <G4ios.hh>

#include <G4String.hh>

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iterator>

#include "json.hpp"

#include "MyRandom.h"


SDHCALPrimaryGeneratorAction::SDHCALPrimaryGeneratorAction()
	: G4VUserPrimaryGeneratorAction()
{
	messenger = new SDHCALPrimaryGeneratorActionMessenger(this) ;

	SDHCALGunOptions opt ;
	opt.particleName = "proton" ;
	gunVec.push_back(new SDHCALGun(opt)) ;
}

SDHCALPrimaryGeneratorAction::SDHCALPrimaryGeneratorAction( G4String jsonFileName )
{
	messenger = new SDHCALPrimaryGeneratorActionMessenger(this) ;

	G4cout << "SDHCALPrimaryGeneratorAction::SDHCALPrimaryGeneratorAction( '" << jsonFileName << "' )" << G4endl ;

	if ( jsonFileName == G4String("") )
	{
		std::cout << "ERROR : no json file provided" << std::endl ;
		std::terminate() ;
	}

	std::ifstream file(jsonFileName) ;
	auto json = nlohmann::json::parse(file) ;

	if ( !json.count("particuleGuns") )
	{
		G4cout << "ERROR : no gun provided" << G4endl ;
		std::terminate() ;
	}

	auto gunList = json.at("particuleGuns") ;

	for ( const auto& gun : gunList )
		gunVec.push_back( new SDHCALGun(gun) ) ;
}

SDHCALPrimaryGeneratorAction::~SDHCALPrimaryGeneratorAction()
{
	delete messenger ;

	for ( const auto& gun : gunVec )
		delete gun ;
}

void SDHCALPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
	for ( const auto& gun : gunVec )
		gun->generatePrimary(event) ;
}

void SDHCALPrimaryGeneratorAction::print() const
{
	G4cout << "Primary particles : " << G4endl ;
	for ( const auto& gun : gunVec )
	{
		G4cout << G4endl ;
		G4cout << "    pdgID : " << gun->GetParticleDefinition()->GetParticleName() << G4endl ;
		G4cout << "     Time : " << gun->GetParticleTime() << " ns" << G4endl ;
		G4cout << "      Pos : " << gun->GetParticlePosition() << G4endl ;
		G4cout << "      Mom : " << gun->GetParticleMomentumDirection() << G4endl ;
		G4cout << "   Energy : " << gun->GetParticleEnergy()/CLHEP::GeV << " GeV" << G4endl ;
	}
}


