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

	std::vector<SDHCALGunOptions> optVec ;

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
			{
				SDHCALGunOptions option ;
				XMLElement* param = type->FirstChildElement() ;

				while( param )
				{
					if ( param->Value() == std::string("pdgID") )
						option.particleName = param->GetText() ;

					if ( param->Value() == std::string("time") )
						option.time = std::atof( param->GetText() ) ;

					if ( param->Value() == std::string("position") )
					{
						std::string positionType = param->Attribute("type") ;

						if ( positionType == std::string("cosmic") )
						{
							option.gunOptionPosition = "cosmic" ;
						}
						else
						{
							std::istringstream iss( param->GetText() ) ;
							std::vector<std::string> result{ std::istream_iterator<std::string>(iss) , {} } ;

							option.meanPositionX = std::atof(result.at(0).c_str()) ;
							option.meanPositionY = std::atof(result.at(1).c_str()) ;
							option.meanPositionZ = std::atof(result.at(2).c_str()) ;

							if ( positionType == std::string("fixed") )
							{
								option.gunOptionPosition = "fixed" ;
							}
							if ( positionType == std::string("uniform") )
							{
								option.gunOptionPosition = "uniform" ;
								option.uniformMaxPosition = param->FloatAttribute("delta") ;
							}
							if ( positionType == std::string("gaus") )
							{
								option.gunOptionPosition = "gaus" ;
								option.sigmaPosition = param->FloatAttribute("sigma") ;
							}
						}
					}

					if ( param->Value() == std::string("momentum") )
					{
						std::string momentumType = param->Attribute("type") ;

						std::istringstream iss( param->GetText() ) ;
						std::vector<std::string> result{ std::istream_iterator<std::string>(iss) , {} } ;

						option.momentumPhi = std::atof(result.at(0).c_str()) ;
						option.momentumTheta = std::atof(result.at(1).c_str()) ;

						if ( momentumType == std::string("fixed") )
						{
							option.gunOptionMomentum = "fixed" ;
						}
						if ( momentumType == std::string("gaus") )
						{
							option.gunOptionMomentum = "gaus" ;
							option.gaussianMomentumSigma = param->FloatAttribute("sigma") ;
						}
					}


					if ( param->Value() == std::string("energy") )
					{
						std::string energyType = param->Attribute("type") ;

						if ( energyType == std::string("fixed") )
						{
							option.gunOptionEnergyDistribution = std::string("fixed") ;
							option.particleEnergy = std::atof( param->GetText() ) * CLHEP::GeV ;
						}
						if ( energyType == std::string("gaus") )
						{
							option.gunOptionEnergyDistribution = std::string("gaus") ;
							option.particleEnergy = std::atof( param->GetText() ) * CLHEP::GeV ;
							option.sigmaEnergy = param->FloatAttribute("sigma") * CLHEP::GeV ;
						}

						if ( energyType == std::string("uniform") || energyType == std::string("forNN") )
						{
							option.gunOptionEnergyDistribution = energyType ;

							G4double min = param->FloatAttribute("min") ;
							G4double max = param->FloatAttribute("max") ;

							assert ( min>0 && max>0 ) ;
							assert ( min < max ) ;

							option.minEnergy = min * CLHEP::GeV ;
							option.maxEnergy = max * CLHEP::GeV ;
						}
					}

					param = param->NextSiblingElement() ;
				}

				optVec.push_back(option) ;
			}

			type = type->NextSiblingElement() ;
		}

		break ;
	}

	for ( const auto& option : optVec )
		gunVec.push_back( new SDHCALGun(option) ) ;
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


