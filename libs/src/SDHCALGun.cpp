#include "SDHCALGun.h"

#include <G4ParticleTable.hh>

#include "SDHCALDetectorConstruction.h"

#include <iterator>

#include "MyRandom.h"

SDHCALGun::SDHCALGun()
	: G4ParticleGun(1) ,
	  options()
{
}

SDHCALGun::SDHCALGun(const SDHCALGunOptions& opt)
	: G4ParticleGun(1) ,
	  options(opt)
{
}

SDHCALGun::SDHCALGun(nlohmann::json json)
	: G4ParticleGun(1) ,
	  options()
{
	G4cout << "SDHCALGun::SDHCALGun()" << G4endl ;

	if ( json.count("particleName") )
		options.particleName = json.at("particleName").get<G4String>() ;

	if ( json.count("energy") )
	{
		auto energyParam = json.at("energy") ;


		if ( !energyParam.count("option") )
			G4cout << "WARNING : energy option not provided, assume fixed" << G4endl ;
		else
			options.gunOptionEnergyDistribution = energyParam.at("option").get<G4String>() ;


		if ( options.gunOptionEnergyDistribution == "uniform"
			 || options.gunOptionEnergyDistribution == "forNN" )
		{
			if ( !energyParam.count("min") || !energyParam.count("max") )
			{
				G4cout << "ERROR : min or max value not provided" << G4endl ;
				std::terminate() ;
			}

			options.minEnergy = energyParam.at("min").get<G4double>() * CLHEP::GeV ;
			options.maxEnergy = energyParam.at("max").get<G4double>() * CLHEP::GeV ;

			assert ( options.minEnergy>0 && options.maxEnergy>0 ) ;
			assert ( options.minEnergy < options.maxEnergy ) ;

		}
		else
		{
			if ( !energyParam.count("value") )
				G4cout << "WARNING : energy value not provided, assume 30GeV" << G4endl ;
			else
				options.particleEnergy = energyParam.at("value").get<G4double>() * CLHEP::GeV ;


			if ( options.gunOptionEnergyDistribution == "gaus" )
			{
				if ( !energyParam.count("sigma") )
					G4cout << "WARNING : energy sigma not provided, assume 0.1GeV" << G4endl ;

				options.sigmaEnergy = energyParam.at("sigma").get<G4double>() * CLHEP::GeV ;
			}
			else if ( options.gunOptionEnergyDistribution != "fixed" )
			{
				G4cout << "ERROR : energy option " << energyParam.at("option") << " unknown" << G4endl ;
				std::terminate() ;
			}
		}
	}


	if ( json.count("cosmic") )
	{
		options.cosmicGun = json.at("cosmic").get<G4bool>() ;

		if ( options.cosmicGun )
			return ;
	}

	if ( json.count("vertex") )
	{

		auto vertexParam = json.at("vertex") ;

		if ( vertexParam.count("time") )
			options.time = vertexParam.at("time").get<G4double>() * CLHEP::ns ;


		if ( !vertexParam.count("option") )
			G4cout << "WARNING : position option not provided, assume fixed" << G4endl ;
		else
			options.gunOptionPosition = vertexParam.at("option").get<G4String>() ;

		if ( vertexParam.count("position") )
		{
			auto pos = vertexParam.at("position") ;
			options.meanPositionX = pos.at("x").get<G4double>() * CLHEP::mm ;
			options.meanPositionY = pos.at("y").get<G4double>() * CLHEP::mm ;
			options.meanPositionZ = pos.at("z").get<G4double>() * CLHEP::mm ;
		}

		if ( options.gunOptionPosition == "gaus" )
		{
			if ( !vertexParam.count("sigma") )
				G4cout << "WARNING : position sigma not provided, assume 1mm" << G4endl ;

			options.sigmaPosition = vertexParam.at("sigma").get<G4double>() * CLHEP::mm ;
		}
		else if ( options.gunOptionPosition == "uniform" )
		{
			if ( !vertexParam.count("delta") )
				G4cout << "WARNING : position delta not provided, assume 0mm" << G4endl ;

			options.uniformMaxPosition = vertexParam.at("delta").get<G4double>() * CLHEP::mm ;
		}
		else if ( options.gunOptionPosition != "fixed" )
		{
			G4cout << "ERROR : energy option " << vertexParam.at("option") << " unknown" << G4endl ;
			std::terminate() ;
		}
	}

	if ( json.count("momentum") )
	{
		auto momentumParam = json.at("momentum") ;


		if ( !momentumParam.count("option") )
			G4cout << "WARNING : momentum option not provided, assume fixed" << G4endl ;
		else
			options.gunOptionMomentum = momentumParam.at("option").get<G4String>() ;

		if ( momentumParam.count("direction") )
		{
			auto pos = momentumParam.at("direction") ;
			options.momentumPhi = pos.at("phi").get<G4double>() ;
			options.momentumTheta = pos.at("theta").get<G4double>() ;
		}

		if ( options.gunOptionMomentum == "gaus" )
		{
			if ( !momentumParam.count("sigma") )
				G4cout << "WARNING : momentum sigma not provided, assume 0.1" << G4endl ;

			options.gaussianMomentumSigma = momentumParam.at("sigma").get<G4double>() ;
		}
		else if ( options.gunOptionMomentum != "fixed" )
		{
			G4cout << "ERROR : momentum option " << momentumParam.at("option") << " unknown" << G4endl ;
			std::terminate() ;
		}
	}
}

void SDHCALGun::generatePrimary(G4Event* event)
{
	SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle( options.particleName ) ) ;

	if ( options.cosmicGun )
		shootForCosmic() ;
	else
	{
		shootPosition() ;
		shootMomentum() ;
	}
	shootEnergy() ;

	SetParticleTime( options.time ) ;

	GeneratePrimaryVertex(event) ;
}

void SDHCALGun::shootPosition()
{
	double X , Y , Z ;
	X = 0.0*CLHEP::m ;
	Y = 0.0*CLHEP::m ;
	Z = -20*CLHEP::mm ;

	MyRandom* rand = MyRandom::Instance() ;

	if ( options.gunOptionPosition == G4String("fixed") )
	{
		X = options.meanPositionX * CLHEP::mm ;
		Y = options.meanPositionY * CLHEP::mm ;
		Z = options.meanPositionZ * CLHEP::mm ;
	}
	else if ( options.gunOptionPosition == G4String("uniform") )
	{
		X = rand->Uniform(options.meanPositionX - options.uniformMaxPosition , options.meanPositionX + options.uniformMaxPosition) * CLHEP::mm ;
		Y = rand->Uniform(options.meanPositionY - options.uniformMaxPosition , options.meanPositionY + options.uniformMaxPosition) * CLHEP::mm ;
		Z = options.meanPositionZ * CLHEP::mm ;
	}
	else if ( options.gunOptionPosition == G4String("gaus") )
	{
		X = rand->Gaus(options.meanPositionX , options.sigmaPosition) * CLHEP::mm ;
		Y = rand->Gaus(options.meanPositionY , options.sigmaPosition) * CLHEP::mm ;
		Z = options.meanPositionZ * CLHEP::mm ;
	}
	else if ( options.gunOptionPosition == G4String("cosmic") )
	{
		G4cout << " ERROR : cosmic not in shootPosition()" << G4endl ;
		throw ;
	}
	else
	{
		G4cout << "Position option unknown : put to defaut (0,0)" << G4endl ;
	}

	SetParticlePosition( G4ThreeVector(X, Y, Z) ) ;
}

void SDHCALGun::shootMomentum()
{
	double X , Y , Z ;
	X = std::cos(options.momentumPhi)*std::sin(options.momentumTheta) ;
	Y = std::sin(options.momentumPhi)*std::sin(options.momentumTheta) ;
	Z = std::cos(options.momentumTheta) ;

	MyRandom* rand = MyRandom::Instance() ;
	if ( options.gunOptionMomentum == G4String("fixed") )
	{
	}
	else if ( options.gunOptionMomentum == G4String("gaus") )
	{
		double phi = rand->Uniform(0.0 , 2*M_PI) ;
		double theta = rand->Gaus(0.0 , options.gaussianMomentumSigma) ;
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

	G4ThreeVector momDir = G4ThreeVector(X, Y, Z) ;
	SetParticleMomentumDirection ( momDir /= momDir.mag() ) ;
}

void SDHCALGun::shootForCosmic() //HardCoded
{
	MyRandom* rand = MyRandom::Instance() ;

	double sizeZ = SDHCALDetectorConstruction::sizeZ ;
	double sizeX = SDHCALDetectorConstruction::sizeX ;

	double circleRadius = ( std::sqrt( sizeZ*sizeZ + 2.0*sizeX*sizeX ) + 10 ) * CLHEP::mm ;

	double aX = rand->Uniform(-0.5*sizeX , 0.5*sizeX) * CLHEP::mm ;
	double aY = rand->Uniform(-0.5*sizeX , 0.5*sizeX) * CLHEP::mm ;
	double aZ = rand->Uniform(0.0 , sizeZ) * CLHEP::mm ;

	double phi = rand->Uniform(0.0 , 2.0*M_PI) ;
	double theta = acos( rand->Uniform(-1.0 , 1.0) ) ;

	double X = aX + circleRadius*std::cos(phi)*std::sin(theta) ;
	double Y = aY + circleRadius*std::sin(phi)*std::sin(theta) ;
	double Z = aZ + circleRadius*std::cos(theta) ;

	G4ThreeVector position(X,Y,Z) ;
	G4ThreeVector momentum(aX-X , aY-Y , aZ-Z) ;
	momentum /= momentum.mag() ;

	SetParticlePosition( position ) ;
	SetParticleMomentumDirection( momentum ) ;
}

void SDHCALGun::shootEnergy()
{
	if ( options.gunOptionEnergyDistribution == G4String("fixed") )
	{
		SetParticleEnergy( options.particleEnergy ) ;
		return ;
	}

	double shoot = options.particleEnergy ;
	MyRandom* rand = MyRandom::Instance() ;
	if ( options.gunOptionEnergyDistribution == G4String("gaus") )
	{
		shoot = rand->Gaus(options.particleEnergy , options.sigmaEnergy) ;
		if ( shoot < 0 )
			shoot = -shoot ;
	}
	else if ( options.gunOptionEnergyDistribution == G4String("uniform") )
	{
		shoot = rand->Uniform(options.minEnergy , options.maxEnergy) ;
	}
	else if ( options.gunOptionEnergyDistribution == G4String("forNN") ) //shoot in 1/x distrbution
	{
		double I = std::log(options.maxEnergy/options.minEnergy) ;
		double x = rand->Uniform(0.0 , 1.0) ;
		shoot = options.minEnergy * std::exp(x*I) ;
	}
	else
	{
		G4cout << "EnergyDistribution option " << options.gunOptionEnergyDistribution << " unknown : put to defaut (50 GeV)" << G4endl ;
	}

	SetParticleEnergy( shoot ) ;
}
