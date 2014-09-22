#include "LyonPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "TRandom.h"
#include "TF1.h"
#include <time.h>  
#include <iostream>

#define RANDOM_GUN
#define SOLID_ANGLE
LyonPrimaryGeneratorAction::LyonPrimaryGeneratorAction()
{
  G4int n_particle = 1;// on genere une particlue
  particleGun = new G4ParticleGun(n_particle);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  particleGun->SetParticleDefinition(particleTable->FindParticle(particleName="mu+"));
  particleGun->SetParticleEnergy(1.0*GeV); 
}

LyonPrimaryGeneratorAction::~LyonPrimaryGeneratorAction()
{
  delete particleGun;
}

void LyonPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
#ifdef RANDOM_GUN
  float max=0.4;
  float xo=2*max*G4UniformRand()-max;
  float yo=2*max*G4UniformRand()-max;
  G4ThreeVector pos(xo*m, yo*m,-2.500*m);
  particleGun->SetParticlePosition(pos);//par default
#endif
#ifndef RANDOM_GUN
  G4ThreeVector pos(0.0*m, 0.0*m,-0.700*m);
  particleGun->SetParticlePosition(pos);//par default
#endif

#ifdef SOLID_ANGLE
  //distribution uniform in solid angle

  G4double X0 = 2.0*m; 
  G4double a = 0.05*m;   
  G4double b = 0.05*m;   
  G4double R0 = std::sqrt(a*a/4+b*b/4);  
  G4double rndm1, rndm2;  
  G4double px, py, pz, projx, projy; 
  G4double MinTheta, MaxTheta, MinPhi, MaxPhi; 
  G4double Phi;
  
  MinTheta = 0.; 
  MaxTheta = std::atan(R0/X0);   
  MinPhi = 0.; 
  MaxPhi = twopi; 		
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
  
     projx = X0*tantheta*cosphi;  
     projy = X0*tantheta*sinphi;   
   
  
  
  }while(sqrt(projx*projx)>500*cm||sqrt(projy*projy)>500*cm);
  G4ThreeVector v(px,py,pz);
  particleGun->SetParticleMomentumDirection(v);
  particleGun->GeneratePrimaryVertex(anEvent);
  G4cout << "Gun position = " << pos << G4endl;
  G4cout << "Gun momentum = " << v << G4endl;
#endif
#ifndef SOLID_ANGLE
  G4ThreeVector v(0.0,0.0,1.0); // par default
  particleGun->SetParticleMomentumDirection(v);
  particleGun->GeneratePrimaryVertex(anEvent);
  G4cout << "Gun position = " << pos << G4endl;
  G4cout << "Gun momentum = " << v << G4endl;
#endif


}
