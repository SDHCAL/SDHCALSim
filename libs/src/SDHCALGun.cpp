#include "SDHCALGun.hpp"

#include "G4ParticleTable.hh"
#include "SDHCALDetectorConstruction.hpp"
#include "Randomize.hh"

SDHCALGun::SDHCALGun(nlohmann::json json,SDHCALDetectorConstruction* detector):G4ParticleGun(1),m_Detector(detector)
{
  m_Rand.setSeed(CLHEP::HepRandom::getTheSeed());
  G4cout << "SDHCALGun::SDHCALGun()" << G4endl;
  if( json.count("particleName") ) particleName = json.at("particleName").get<G4String>();
  if( json.count("energy") )
  {
    auto energyParam = json.at("energy");
    if( !energyParam.count("option") ) G4cout << "WARNING : energy option not provided, assume fixed" << G4endl;
    else gunOptionEnergyDistribution = energyParam.at("option").get<G4String>();

    if(gunOptionEnergyDistribution == "uniform"|| gunOptionEnergyDistribution == "forNN" )
    {
      if(!energyParam.count("min") || !energyParam.count("max") )
      {
        G4cerr << "ERROR : min or max value not provided" << G4endl;
        std::exit(-1);
      }

      minEnergy = energyParam.at("min").get<G4double>() * CLHEP::GeV;
      maxEnergy = energyParam.at("max").get<G4double>() * CLHEP::GeV;

      assert ( minEnergy>0 && maxEnergy>0 );
      assert ( minEnergy < maxEnergy );
    }
    else
    {
      if(!energyParam.count("value"))G4cout << "WARNING : energy value not provided, assume 30GeV" << G4endl;
      else particleEnergy = energyParam.at("value").get<G4double>() * CLHEP::GeV;

      if( gunOptionEnergyDistribution == "gaus" )
      {
        if(!energyParam.count("sigma"))G4cout << "WARNING : energy sigma not provided, assume 0.1GeV" << G4endl;
        sigmaEnergy = energyParam.at("sigma").get<G4double>() * CLHEP::GeV;
      }
      else if( gunOptionEnergyDistribution != "fixed" )
      {
        G4cerr<< "ERROR : energy option " << energyParam.at("option") << " unknown" << G4endl;
        std::exit(-1);
      }
    }
  }

  if( json.count("cosmic") )
  {
    cosmicGun = json.at("cosmic").get<G4bool>();
    if ( cosmicGun ) return;
  }

  if( json.count("vertex") )
  {
    auto vertexParam = json.at("vertex");
    if(vertexParam.count("time")) m_Time = vertexParam.at("time").get<G4double>()*CLHEP::ns;

    if(!vertexParam.count("option"))G4cout<< "WARNING : position option not provided, assume fixed"<<G4endl;
    else gunOptionPosition = vertexParam.at("option").get<G4String>() ;

    if( vertexParam.count("position") )
    {
      auto pos = vertexParam.at("position") ;
      meanPositionX = pos.at("x").get<G4double>() * CLHEP::mm ;
      meanPositionY = pos.at("y").get<G4double>() * CLHEP::mm ;
      meanPositionZ = pos.at("z").get<G4double>() * CLHEP::mm ;
    }

    if( gunOptionPosition == "gaus" )
    {
      if(!vertexParam.count("sigma")) G4cout << "WARNING : position sigma not provided, assume 1mm" <<G4endl;
      sigmaPosition = vertexParam.at("sigma").get<G4double>() * CLHEP::mm;
    }
    else if( gunOptionPosition == "uniform" )
    {
      if(!vertexParam.count("delta")) G4cout<< "WARNING : position delta not provided, assume 0mm" << G4endl;
      uniformMaxPosition = vertexParam.at("delta").get<G4double>()*CLHEP::mm;
    }
    else if ( gunOptionPosition != "fixed" )
    {
      G4cerr<< "ERROR : energy option " << vertexParam.at("option") << " unknown" <<G4endl;
      std::exit(-1);
    }
  }

  if( json.count("momentum") )
  {
    auto momentumParam = json.at("momentum") ;
    if(!momentumParam.count("option")) G4cout<<"WARNING : momentum option not provided, assume fixed"<<G4endl;
    else gunOptionMomentum = momentumParam.at("option").get<G4String>();

    if(momentumParam.count("direction"))
    {
      auto pos = momentumParam.at("direction");
      momentumPhi = pos.at("phi").get<G4double>();
      momentumTheta = pos.at("theta").get<G4double>();
    }

    if(gunOptionMomentum == "gaus" )
    {
      if(!momentumParam.count("sigma")) G4cout<<"WARNING : momentum sigma not provided, assume 0.1"<<G4endl;
      gaussianMomentumSigma = momentumParam.at("sigma").get<G4double>();
    }
    else if( gunOptionMomentum != "fixed" )
    {
      G4cerr << "ERROR : momentum option " << momentumParam.at("option") << " unknown" << G4endl;
      std::exit(-1);
    }
  }
}

void SDHCALGun::generatePrimary(G4Event* event)
{
  SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle( particleName ) );
  if ( cosmicGun ) shootForCosmic();
  else
  {
    shootPosition();
    shootMomentum();
  }
  shootEnergy();

  SetParticleTime( m_Time );

  GeneratePrimaryVertex(event);
}

void SDHCALGun::shootPosition()
{
  double X , Y , Z ;
  X = 0.0*CLHEP::m ;
  Y = 0.0*CLHEP::m ;
  Z = -20*CLHEP::mm ;

  if ( gunOptionPosition == G4String("fixed") )
  {
    X = meanPositionX * CLHEP::mm ;
    Y = meanPositionY * CLHEP::mm ;
    Z = meanPositionZ * CLHEP::mm ;
  }
  else if ( gunOptionPosition == G4String("uniform") )
  {
    X = m_Rand.Uniform(meanPositionX - uniformMaxPosition , meanPositionX + uniformMaxPosition) * CLHEP::mm ;
    Y = m_Rand.Uniform(meanPositionY - uniformMaxPosition , meanPositionY + uniformMaxPosition) * CLHEP::mm ;
    Z = meanPositionZ * CLHEP::mm ;
  }
  else if ( gunOptionPosition == G4String("gaus") )
  {
    X = m_Rand.Gaus(meanPositionX , sigmaPosition) * CLHEP::mm ;
    Y = m_Rand.Gaus(meanPositionY , sigmaPosition) * CLHEP::mm ;
    Z = meanPositionZ * CLHEP::mm ;
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

  SetParticlePosition( G4ThreeVector(X, Y, Z) ) ;
}

void SDHCALGun::shootMomentum()
{
  double X , Y , Z ;
  X = std::cos(momentumPhi)*std::sin(momentumTheta) ;
  Y = std::sin(momentumPhi)*std::sin(momentumTheta) ;
  Z = std::cos(momentumTheta) ;
  
  if (gunOptionMomentum == G4String("fixed") )
  {
  }
  else if (gunOptionMomentum == G4String("gaus") )
  {
    double phi = m_Rand.Uniform(0.0 , 2*M_PI) ;
    double theta = m_Rand.Gaus(0.0 , gaussianMomentumSigma) ;
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
  double sizeZ = m_Detector->getCaloSizeZ() ;
  double sizeX = m_Detector->getCaloSizeX() ;
  double sizeY = m_Detector->getCaloSizeY() ;
  double circleRadius = ( std::sqrt( sizeZ*sizeZ + 2.0*sizeX*sizeX ) + 10 ) * CLHEP::mm ;

  double aX = m_Rand.Uniform(-0.5*sizeX , 0.5*sizeX) * CLHEP::mm ;
  double aY = m_Rand.Uniform(-0.5*sizeY , 0.5*sizeY) * CLHEP::mm ;
  double aZ = m_Rand.Uniform(0.0 , sizeZ) * CLHEP::mm ;

  double phi = m_Rand.Uniform(0.0 , 2.0*M_PI) ;
  double theta = acos( m_Rand.Uniform(-1.0 , 1.0) ) ;

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
  if ( gunOptionEnergyDistribution == G4String("fixed") )
  {
    SetParticleEnergy( particleEnergy ) ;
    return ;
  }

  double shoot = particleEnergy ;
  if ( gunOptionEnergyDistribution == G4String("gaus") )
  {
    shoot = m_Rand.Gaus(particleEnergy , sigmaEnergy) ;
    if ( shoot < 0 ) shoot = -shoot ;
  }
  else if ( gunOptionEnergyDistribution == G4String("uniform") )
  {
    shoot = m_Rand.Uniform(minEnergy , maxEnergy) ;
  }
  else if ( gunOptionEnergyDistribution == G4String("forNN") ) //shoot in 1/x distrbution
  {
    double I = std::log(maxEnergy/minEnergy) ;
    double x = m_Rand.Uniform(0.0 , 1.0) ;
    shoot = minEnergy * std::exp(x*I) ;
  }
  else
  {
    G4cout<<"EnergyDistribution option "<<gunOptionEnergyDistribution<<" unknown : put to defaut (50 GeV)"<<G4endl;
  }

  SetParticleEnergy( shoot ) ;
}
