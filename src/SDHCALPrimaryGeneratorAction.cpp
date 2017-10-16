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

#include "MyRandom.h"


SDHCALPrimaryGeneratorAction::SDHCALPrimaryGeneratorAction()
	: G4VUserPrimaryGeneratorAction()
{
	messenger = new SDHCALPrimaryGeneratorActionMessenger(this) ;

	G4int nParticle = 1 ;

	particleGunVec.push_back( new G4ParticleGun(nParticle) ) ;
	particleGun = particleGunVec.at(0) ;

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable() ;
	G4String particleName = "pi-" ;
	particleDefinition = particleTable->FindParticle(particleName) ;


	gunOptionPosition = std::string("fixed") ;
	gunOptionMomentum = std::string("fixed") ;
	gunOptionEnergyDistribution = std::string("fixed") ;

	meanPositionX = 0.0 * CLHEP::mm ;
	meanPositionY = 0.0 * CLHEP::mm ;
	uniformMaxPosition = 0.0 * CLHEP::mm ;
	sigmaPosition = 1 * CLHEP::mm ;

	gaussianMomentumSigma = 0.1 ;
	momentumTheta = 0 ;
	momentumPhi = 0 ;

	particleEnergy = 30 * CLHEP::GeV ;
	sigmaEnergy = 0.1 * CLHEP::GeV ;
	minEnergy = 50 * CLHEP::GeV ;
	maxEnergy = 50 * CLHEP::GeV ;

	primaryPos = G4ThreeVector(0 , 0 , -20*CLHEP::mm) ;
	primaryMom = G4ThreeVector(0 , 0 , 1) ;
	primaryEnergy = particleEnergy ;
}


SDHCALPrimaryGeneratorAction::~SDHCALPrimaryGeneratorAction()
{
	delete messenger ;
	delete particleGun ;
}

void SDHCALPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
	if ( gunOptionPosition == G4String("cosmic") )
		shootForCosmic() ;
	else
	{
		shootPosition() ;
		shootMomentum() ;
	}
	shootEnergy() ;

	particleGun->SetParticleDefinition( particleDefinition ) ;
	particleGun->SetParticlePosition( primaryPos ) ;
	particleGun->SetParticleMomentumDirection( primaryMom ) ;
	particleGun->SetParticleEnergy( primaryEnergy ) ;

	particleGun->GeneratePrimaryVertex(event) ;
}

void SDHCALPrimaryGeneratorAction::shootPosition()
{
	double X , Y , Z ;
	X = 0.0*CLHEP::m ;
	Y = 0.0*CLHEP::m ;
	Z = -20*CLHEP::mm ;

	MyRandom* rand = MyRandom::Instance() ;

	if ( gunOptionPosition == G4String("fixed") )
	{
		X = meanPositionX * CLHEP::mm ;
		Y = meanPositionY * CLHEP::mm ;
		Z = meanPositionZ * CLHEP::mm ;
	}
	else if ( gunOptionPosition == G4String("uniform") )
	{
		X = rand->Uniform(meanPositionX - uniformMaxPosition , meanPositionX + uniformMaxPosition) * CLHEP::mm ;
		Y = rand->Uniform(meanPositionY - uniformMaxPosition , meanPositionY + uniformMaxPosition) * CLHEP::mm ;
	}
	else if ( gunOptionPosition == G4String("gaus") )
	{
		X = rand->Gaus(meanPositionX , sigmaPosition) * CLHEP::mm ;
		Y = rand->Gaus(meanPositionY , sigmaPosition) * CLHEP::mm ;
	}
	else if ( gunOptionPosition == G4String("cosmic") )
	{
		G4cout << " ERROR : cosmic not in shootPosition()" << G4endl ;
		throw ;
	}
	else
	{
		G4cout << "Position option unknown : put to defaut (0,0)" << G4endl ;
	}

	primaryPos = G4ThreeVector(X, Y, Z) ;
}

void SDHCALPrimaryGeneratorAction::shootMomentum()
{
	double X , Y , Z ;
	X = std::cos(momentumPhi)*std::sin(momentumTheta) ;
	Y = std::sin(momentumPhi)*std::sin(momentumTheta) ;
	Z = std::cos(momentumTheta) ;

	MyRandom* rand = MyRandom::Instance() ;
	if ( gunOptionMomentum == G4String("fixed") )
	{
	}
	else if ( gunOptionMomentum == G4String("gaus") )
	{
		double phi = rand->Uniform(0 , 2*M_PI) ;
		double theta = rand->Gaus(0 , gaussianMomentumSigma) ;
		if (theta < 0)
		{
			theta = -theta ;
			phi = -phi ;
		}

		double XX = std::cos(phi)*std::sin(theta) ;
		double YY = std::sin(phi)*std::sin(theta) ;
		double ZZ = std::cos(theta) ;

		X += XX ;
		Y += YY ;
		Z += ZZ ;
	}
	else
	{
		G4cout << "Position option unknown : put to defaut theta = 0" << G4endl ;
	}

	primaryMom = G4ThreeVector(X, Y, Z) ;
	primaryMom /= primaryMom.mag() ;
}

void SDHCALPrimaryGeneratorAction::shootForCosmic() //HardCoded
{
	MyRandom* rand = MyRandom::Instance() ;

	double sizeZ = SDHCALDetectorConstruction::sizeZ ;
	double sizeX = SDHCALDetectorConstruction::sizeX ;

	double circleRadius = ( std::sqrt( sizeZ*sizeZ + 2.0*sizeX*sizeX ) + 10 ) * CLHEP::mm ;

	double aX = rand->Uniform(-0.5*sizeX , 0.5*sizeX) * CLHEP::mm ;
	double aY = rand->Uniform(-0.5*sizeX , 0.5*sizeX) * CLHEP::mm ;
	double aZ = rand->Uniform(0 , sizeZ) * CLHEP::mm ;

	double phi = rand->Uniform(0, 2.0*M_PI) ;
	double theta = acos( rand->Uniform(-1 , 1) ) ;

	double X = aX + circleRadius*std::cos(phi)*std::sin(theta) ;
	double Y = aY + circleRadius*std::sin(phi)*std::sin(theta) ;
	double Z = aZ + circleRadius*std::cos(theta) ;

	G4ThreeVector position(X,Y,Z) ;
	G4ThreeVector momentum(aX-X , aY-Y , aZ-Z) ;
	momentum /= momentum.mag() ;

	primaryPos = position ;
	primaryMom = momentum ;
}

void SDHCALPrimaryGeneratorAction::shootEnergy()
{
	if ( gunOptionEnergyDistribution == G4String("fixed") )
	{
		primaryEnergy = particleEnergy ;
		return ;
	}

	double shoot = particleEnergy ;
	MyRandom* rand = MyRandom::Instance() ;
	if ( gunOptionEnergyDistribution == G4String("gaus") )
	{
		shoot = rand->Gaus(particleEnergy , sigmaEnergy) ;
		if ( shoot < 0 )
			shoot = -shoot ;
	}
	else if ( gunOptionEnergyDistribution == G4String("uniform") )
	{
		shoot = rand->Uniform(minEnergy , maxEnergy) ;
	}
	else if ( gunOptionEnergyDistribution == G4String("forNN") ) //shoot in 1/x distrbution
	{
		double I = std::log(maxEnergy/minEnergy) ;
		double x = rand->Uniform(0,1) ;
		shoot = minEnergy * std::exp(x*I) ;
	}
	else
	{
		G4cout << "EnergyDistribution option unknown : put to defaut (50 GeV)" << G4endl ;
	}

	primaryEnergy = shoot ;
}

void SDHCALPrimaryGeneratorAction::print() const
{
	G4cout << "Primary particles : " << G4endl ;
	for ( const auto& gun : particleGunVec )
	{
		G4cout << G4endl ;
		G4cout << "    pdgID : " << gun->GetParticleDefinition()->GetParticleName() << G4endl ;
		G4cout << "      Pos : " << gun->GetParticlePosition() << G4endl ;
		G4cout << "      Mom : " << gun->GetParticleMomentumDirection() << G4endl ;
		G4cout << "   Energy : " << gun->GetParticleEnergy()/CLHEP::GeV << " GeV" << G4endl ;
	}
}


