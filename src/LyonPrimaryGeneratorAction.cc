#include "LyonPrimaryGeneratorAction.hh"
#include "LyonPrimaryGeneratorActionMessenger.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "TRandom.h"
#include "TF2.h"
#include "TMath.h"
#include <time.h>  
#include <iostream>
#include <fstream>

#define RANDOM_GUN
//#define SOLID_ANGLE
#define GAUSSIAN_GUN

LyonPrimaryGeneratorAction::LyonPrimaryGeneratorAction()
{
  G4int n_particle = 1;// on genere une particlue
  particleGun = new G4ParticleGun(n_particle);
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  particleGun->SetParticleDefinition(particleTable->FindParticle(particleName="mu+"));
  particleGun->SetParticleEnergy(1.0*CLHEP::GeV); 
  
  _thePrimaryGeneratorMessenger = new LyonPrimaryGeneratorActionMessenger(this);
  gunOptionPosition = std::string("default");// could be with primary_gun_action_messenger class
  gunOptionMomentum = std::string("normal"); // could be with primary_gun_action_messenger class
  randMaxPos=0.4;
  solidAngleX0 = 2.0*CLHEP::m; 
  solidAngleRad = 0.5*CLHEP::m; 
  gaussianMean = 0.0*CLHEP::m;
  gaussianSigma = 0.1*CLHEP::m;
  uniformParameter = 0.5;  

  _xPosSigma = 50.0;// mm
  _yPosSigma = 50.0;// mm

  _gunPosition_x_function=new TF1("func","gaus",-500,500);
  _gunPosition_y_function=new TF1("func","gaus",-500,500);
  //positionSigma = 50;
  //posfunc->SetParameters(1,0,positionSigma);
}

LyonPrimaryGeneratorAction::~LyonPrimaryGeneratorAction()
{
  PrintInfo();
  delete particleGun;
  delete _thePrimaryGeneratorMessenger;
  delete _gunPosition_x_function;
  delete _gunPosition_y_function;
}

void LyonPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  _primaryPos=G4ThreeVector(0.0*CLHEP::m, 0.0*CLHEP::m,-0.700*CLHEP::m);//default could be modified with primary_gun_action_messenger class
  G4ThreeVector v(0.0,0.0,1.0); // par default could be modified with primary_gun_action_messenger class
  if(gunOptionPosition==std::string("random")){
    float xo=2*randMaxPos*G4UniformRand()-randMaxPos;
    float yo=2*randMaxPos*G4UniformRand()-randMaxPos;
    _primaryPos=G4ThreeVector(xo*CLHEP::m, yo*CLHEP::m,-.700*CLHEP::m);
  }
  if(gunOptionPosition==std::string("cosmic")){
    float xo=2*randMaxPos*G4UniformRand()-randMaxPos;
    float zo=2*randMaxPos*G4UniformRand()-randMaxPos;
    _primaryPos=G4ThreeVector(xo*CLHEP::m,-.503*CLHEP::m,zo*CLHEP::m);
  }
  if(gunOptionPosition==std::string("beamtest")){
    _gunPosition_x_function->SetParameters(1.0,0.0,_xPosSigma);
    _gunPosition_y_function->SetParameters(1.0,0.0,_yPosSigma);
    float xo=_gunPosition_x_function->GetRandom();
    float yo=_gunPosition_y_function->GetRandom();
    float zo=-0.7;
    _primaryPos=G4ThreeVector(xo*CLHEP::mm,yo*CLHEP::mm,zo*CLHEP::m);
  }
  
  particleGun->SetParticlePosition(_primaryPos);
  
  if( gunOptionPosition==std::string("cosmic") ){
    if( gunOptionMomentum!=std::string("cosmic_gaus") && gunOptionMomentum!=std::string("cosmic_uniform") ){
      G4cout << " ERROR : wrong option : GunOptionPosition=cosmic should be used together with cosmic_gaus or cosmic_uniform as GunOptionMomentum" << G4endl;
      G4cout << " I PREFER KILL THE RUN" << G4endl;
      throw;
    }
  }
  
  if( gunOptionMomentum==std::string("cosmic_gaus") || gunOptionMomentum==std::string("cosmic_uniform") ){
    if( gunOptionPosition!=std::string("cosmic") ){
      G4cout << " ERROR : wrong option : GunOptionMomentum=cosmic_gaus or cosmic_uniform should be used together with cosmic GunOptionPosition" << G4endl;
      G4cout << " I PREFER KILL THE RUN" << G4endl;
      throw;
    }
  }

  if(gunOptionMomentum==std::string("solidAngle")){
    G4double R0 = std::sqrt(solidAngleRad*solidAngleRad/4+solidAngleRad*solidAngleRad/4);  
    G4double rndm1, rndm2;  
    G4double px, py, pz, projx, projy; 
    G4double MinTheta, MaxTheta, MinPhi, MaxPhi; 
    G4double Phi;
    
    MinTheta = 0.; 
    MaxTheta = std::atan(R0/solidAngleX0);   
    MinPhi = 0.; 
    MaxPhi = CLHEP::twopi; 		
    G4double sintheta, sinphi, costheta, cosphi, tantheta; 
    do{
      rndm1 = G4UniformRand();  
      costheta = std::cos(MinTheta) - rndm1 * (std::cos(MinTheta) - std::cos(MaxTheta));
      sintheta = std::sqrt(1. - costheta*costheta);  
      tantheta = sintheta/costheta;  
      rndm2 = G4UniformRand();    
      Phi = MinPhi + (MaxPhi - MinPhi) * rndm2;  
      sinphi = std::sin(Phi); 
      cosphi = std::cos(Phi); 
      px = sintheta * cosphi;  
      py = sintheta * sinphi;   
      pz = costheta; 
      projx = solidAngleX0*tantheta*cosphi;  
      projy = solidAngleX0*tantheta*sinphi;   
    }while(sqrt(projx*projx)>500*CLHEP::cm||sqrt(projy*projy)>500*CLHEP::cm);
    v=G4ThreeVector(px,py,pz);
  }
  else if(gunOptionMomentum==std::string("gaus")){
    TF1 *func=new TF1("func","gaus",-1,1);
    func->SetParameters(1,gaussianMean,gaussianSigma);
    G4double px=func->GetRandom();
    G4double py=func->GetRandom();
    G4double pz=1.0;
    v=G4ThreeVector(px,py,pz);
    v/=v.mag();
    delete func;
  }
  else if(gunOptionMomentum==std::string("uniform")){
    G4double px;
    G4double py;
    if(G4UniformRand()>uniformParameter)px=G4UniformRand();
    else px=-G4UniformRand();
    if(G4UniformRand()>uniformParameter) py=G4UniformRand();
    else py=-G4UniformRand();
    G4double pz=1.0;
    v=G4ThreeVector(px,py,pz);
    v/=v.mag();
  }
  else if(gunOptionMomentum==std::string("cosmic_gaus")){
    TF1 *func=new TF1("func","gaus",-1,1);
    func->SetParameters(1,gaussianMean,gaussianSigma);
    G4double px=func->GetRandom();
    G4double py=1.0;
    G4double pz=func->GetRandom();
    v=G4ThreeVector(px,py,pz);
    v/=v.mag();
    delete func;
  }
  else if(gunOptionMomentum==std::string("cosmic_uniform")){
    G4double px;
    G4double py=1.0;
    G4double pz;
    if(G4UniformRand()>uniformParameter)px=G4UniformRand();
    else px=-G4UniformRand();
    if(G4UniformRand()>uniformParameter) pz=G4UniformRand();
    else pz=-G4UniformRand();
    v=G4ThreeVector(px,py,pz);
    v/=v.mag();
  }

  particleGun->SetParticleMomentumDirection(v);
  particleGun->GeneratePrimaryVertex(anEvent);
  _primaryMom=v;
  //G4cout << "Gun position = " << pos << G4endl;
  //G4cout << "Gun momentum = " << v << G4endl;
}

void LyonPrimaryGeneratorAction::PrintInfo()
{
  G4cout << "USED OPTION : " << G4endl;
  G4cout << "GUN POSITION OPTION = " << gunOptionPosition << G4endl;
  if(gunOptionPosition==std::string("random"))
    G4cout << "randMaxPos = " << randMaxPos << G4endl;
  G4cout << "GUN MOMENTUM OPTION = " << gunOptionMomentum << G4endl;
  if(gunOptionMomentum==std::string("solidAngle"))
    G4cout << "SOLID ANGLE X0 = " << solidAngleX0 << "\t"
	  << "SOLID ANGLE RAD = " << solidAngleRad << G4endl;
  else if(gunOptionMomentum==std::string("gaus"))
    G4cout << "GAUSSIAN GUN MEAN = " << gaussianMean << "\t" 
	   << "GAUSSIAN GUN SIGMA = " << gaussianSigma << G4endl;
  else if(gunOptionMomentum==std::string("uniform"))
    G4cout << "UNIFORM GUN PARAMETER = " << uniformParameter<< G4endl;
}
