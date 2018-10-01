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

SDHCALGun::SDHCALGun(tinyxml2::XMLNode* node)
	: G4ParticleGun(1) ,
	  options()
{
	using namespace tinyxml2 ;
	XMLElement* param = node->FirstChildElement() ;

	while( param )
	{
		if ( param->Value() == std::string("pdgID") )
			options.particleName = param->GetText() ;

		if ( param->Value() == std::string("time") )
			options.time = std::atof( param->GetText() ) ;

		if ( param->Value() == std::string("position") )
		{
			std::string positionType = param->Attribute("type") ;

			if ( positionType == std::string("cosmic") )
			{
				options.gunOptionPosition = "cosmic" ;
			}
			else
			{
				std::istringstream iss( param->GetText() ) ;
				std::vector<std::string> result{ std::istream_iterator<std::string>(iss) , {} } ;

				options.meanPositionX = std::atof(result.at(0).c_str()) ;
				options.meanPositionY = std::atof(result.at(1).c_str()) ;
				options.meanPositionZ = std::atof(result.at(2).c_str()) ;

				if ( positionType == std::string("fixed") )
				{
					options.gunOptionPosition = "fixed" ;
				}
				if ( positionType == std::string("uniform") )
				{
					options.gunOptionPosition = "uniform" ;
					options.uniformMaxPosition = param->FloatAttribute("delta") ;
				}
				if ( positionType == std::string("gaus") )
				{
					options.gunOptionPosition = "gaus" ;
					options.sigmaPosition = param->FloatAttribute("sigma") ;
				}
			}
		}

		if ( param->Value() == std::string("momentum") )
		{
			std::string momentumType = param->Attribute("type") ;

			std::istringstream iss( param->GetText() ) ;
			std::vector<std::string> result{ std::istream_iterator<std::string>(iss) , {} } ;

			options.momentumPhi = std::atof(result.at(0).c_str()) ;
			options.momentumTheta = std::atof(result.at(1).c_str()) ;

			if ( momentumType == std::string("fixed") )
			{
				options.gunOptionMomentum = "fixed" ;
			}
			if ( momentumType == std::string("gaus") )
			{
				options.gunOptionMomentum = "gaus" ;
				options.gaussianMomentumSigma = param->FloatAttribute("sigma") ;
			}
		}


		if ( param->Value() == std::string("energy") )
		{
			std::string energyType = param->Attribute("type") ;

			if ( energyType == std::string("fixed") )
			{
				options.gunOptionEnergyDistribution = std::string("fixed") ;
				options.particleEnergy = std::atof( param->GetText() ) * CLHEP::GeV ;
			}
			if ( energyType == std::string("gaus") )
			{
				options.gunOptionEnergyDistribution = std::string("gaus") ;
				options.particleEnergy = std::atof( param->GetText() ) * CLHEP::GeV ;
				options.sigmaEnergy = param->FloatAttribute("sigma") * CLHEP::GeV ;
			}

			if ( energyType == std::string("uniform") || energyType == std::string("forNN") )
			{
				options.gunOptionEnergyDistribution = energyType ;

				G4double min = param->FloatAttribute("min") ;
				G4double max = param->FloatAttribute("max") ;

				assert ( min>0 && max>0 ) ;
				assert ( min < max ) ;

				options.minEnergy = min * CLHEP::GeV ;
				options.maxEnergy = max * CLHEP::GeV ;
			}
		}

		param = param->NextSiblingElement() ;
	}
}

void SDHCALGun::generatePrimary(G4Event* event)
{
	SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle( options.particleName ) ) ;

	if ( options.gunOptionPosition == G4String("cosmic") )
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
		G4cout << "EnergyDistribution option unknown : put to defaut (50 GeV)" << G4endl ;
	}

	SetParticleEnergy( shoot ) ;
}
