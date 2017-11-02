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

#include "tinyxml2.h"

#include "MyRandom.h"


SDHCALPrimaryGeneratorAction::SDHCALPrimaryGeneratorAction()
	: G4VUserPrimaryGeneratorAction()
{
	messenger = new SDHCALPrimaryGeneratorActionMessenger(this) ;

	SDHCALGunOptions opt ;
	opt.particleName = "proton" ;
	gunVec.push_back(new SDHCALGun(opt)) ;
}

SDHCALPrimaryGeneratorAction::SDHCALPrimaryGeneratorAction( std::string xmlFileName )
{
	messenger = new SDHCALPrimaryGeneratorActionMessenger(this) ;

	G4cout << "SDHCALPrimaryGeneratorAction::SDHCALPrimaryGeneratorAction( '" << xmlFileName << "' )" << G4endl ;
	using namespace tinyxml2 ;
	XMLDocument doc ;

	XMLError status = doc.LoadFile( xmlFileName.c_str() ) ;

	if ( status != XML_SUCCESS )
	{
		if ( status == 2 )
			std::cout << "No recover XML file provided or file not existing" << std::endl ;
		else
			std::cerr << "Erreur lors du chargement" << std::endl ;
		throw ;
	}

	XMLHandle handle(&doc) ;
	XMLElement* root = handle.FirstChild().ToElement() ;

	XMLNode* node = root->FirstChild() ;

	while ( node )
	{
		if ( node->Value() != std::string("guns") )
		{
			node = node->NextSiblingElement() ;
			continue ;
		}

		XMLNode* type = node->FirstChild() ;
		while ( type )
		{
			if ( type->Value() == std::string("particle") )
				gunVec.push_back( new SDHCALGun(type) ) ;

			type = type->NextSiblingElement() ;
		}

		break ;
	}
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


