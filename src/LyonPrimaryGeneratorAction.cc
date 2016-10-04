#include "LyonPrimaryGeneratorAction.hh"
#include "LyonPrimaryGeneratorActionMessenger.hh"

#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "TRandom.h"
#include "TF1.h"
#include "TF2.h"
#include "TMath.h"
#include <time.h>  
#include <iostream>

//#define RANDOM_GUN
//#define SOLID_ANGLE
//#define GAUSSIAN_GUN

#include "LyonDetectorConstruction.hh"

class G4SingleParticleSource;

LyonPrimaryGeneratorAction::LyonPrimaryGeneratorAction()
{  
  particleGun = new G4GeneralParticleSource();
  _thePrimaryGeneratorActionMessenger = new LyonPrimaryGeneratorActionMessenger(this);
  NumberOfMuon=1;
  NumberOfGamma=1;
  TimeScale=1e6;
  GPCounter=0;
  //GammaModel = new G4SingleParticleSource();
  // MuonModel = new G4SingleParticleSource();
}

LyonPrimaryGeneratorAction::~LyonPrimaryGeneratorAction()
{
  delete particleGun;
  delete _thePrimaryGeneratorActionMessenger;
  // delete GammaModel;
  //delete MuonModel;
}

void LyonPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  particleGun->SetCurrentSourceto(0);
  
  for(G4int i=0; i<NumberOfGamma; i++)
    {
      // G4cout<< "Sending "<<particleGun->GetParticleDefinition()->GetParticleName()<<" to vertex"<<G4endl;
      particleGun->SetParticleTime(G4UniformRand()*TimeScale);
      particleGun->GetCurrentSource()->GeneratePrimaryVertex(anEvent);
    }

  particleGun->SetCurrentSourceto(1);

  for(G4int i=0; i<NumberOfMuon; i++)
    {
      // G4cout<< "Sending "<<particleGun->GetParticleDefinition()->GetParticleName()<<" to vertex"<<G4endl;
      particleGun->SetParticleTime((G4UniformRand()+1.)*0.5*TimeScale);
      particleGun->GetCurrentSource()->GeneratePrimaryVertex(anEvent);
    }


  
  /* G4int NBS = particleGun->GetNumberofSource();
  if(NBS == 2)
    {
      particleGun->SetCurrentSourceto(0);
      
      }*/
  

  
  /* if(GPCounter==0)
    {
      NBS = particleGun->GetNumberofSource();
      G4cout<<"NBS = "<<NBS<<G4endl;
      for(G4int i = 0; i<NBS; i++)
	{
	  particleGun->SetCurrentSourceto(i);
	  G4cout<< "I'm "<<particleGun->GetParticleDefinition()->GetParticleName()<<G4endl;
	}
      G4cout<<"******************************************************************"<<G4endl;
      particleGun->SetCurrentSourceto(0);    
      *GammaModel = *(particleGun->GetCurrentSource());

      particleGun->SetCurrentSourceto(1);
      G4cout<< "I'm "<<particleGun->GetParticleDefinition()->GetParticleName()<<G4endl;
      *MuonModel = *(particleGun->GetCurrentSource());

      particleGun->ClearAll();

      for(G4int i=0; i<NumberOfGamma; i++)
	{
	  particleGun->AddaSource(1.);
	  *(particleGun->GetCurrentSource())= *GammaModel;
	  // particleGun->SetParticleTime(G4UniformRand()*1000000.);
	}
 
      for(G4int i=0; i<NumberOfMuon; i++)
	{
	  particleGun->AddaSource(1.);
	  *(particleGun->GetCurrentSource())= *MuonModel;
	  //      particleGun->SetParticleTime(G4UniformRand()*1000000.);
	}
      G4cout<< "I'm "<<particleGun->GetParticleDefinition()->GetParticleName()<<G4endl;

      NBS = particleGun->GetNumberofSource();
      G4cout<<"NBS = "<<NBS<<G4endl;
      if (NBS != (NumberOfGamma+NumberOfMuon))
	{
	  G4cout<<"*************************** Something is wrong with the number ************************************"<<G4endl;
	  return;
	}
      else
	{
	  for( G4int i = 0; i < NumberOfGamma; i++)
	    {
	      particleGun->SetCurrentSourceto(i);
	      if( particleGun->GetParticleDefinition()->GetParticleName() != "gamma")
		{
		  G4cout<<"*************************** Something is wrong with gamma number ************************************"<<G4endl;
		  return;
		}
	    }
	  for( G4int i = NumberOfGamma; i < NBS; i++)
	    {
	      particleGun->SetCurrentSourceto(i);
	      G4cout<< "I'm "<<particleGun->GetParticleDefinition()->GetParticleName()<<G4endl;
	      if( particleGun->GetParticleDefinition()->GetParticleName() != "mu+")
		{
		  G4cout<<"*************************** Something is wrong with primary muon number ************************************"<<G4endl;
		  return;
		}
	    }
	}
    }
 
  GPCounter++;
  G4cout<<"***********The GeneratePrimaries methode is called for "<<GPCounter<<" times*******"<<G4endl;
   

  for( G4int i=0; i<NumberOfGamma; i++)
    {
      particleGun->SetCurrentSourceto(i);
      particleGun->SetParticleTime(G4UniformRand()*TimeScale);
    }
  NBS=NumberOfGamma+NumberOfMuon;
  for( G4int i=NumberOfGamma; i<NBS; i++)
    {
      particleGun->SetCurrentSourceto(i);
      G4cout<< "I'm "<<particleGun->GetParticleDefinition()->GetParticleName()<<G4endl;
      G4double t = ((G4UniformRand()+1.)*0.5*TimeScale);
      G4cout<<" My time is :"<< t<<G4endl;
      particleGun->SetParticleTime(t);
    }
  //if(particleGun->GetNumberofSource()==0)  particleGun->AddaSource(1.);

  particleGun->GeneratePrimaryVertex(anEvent); */

  
  
}

G4ThreeVector LyonPrimaryGeneratorAction::GetPrimaryGeneratorMomentum() const
{
	return particleGun->GetParticleMomentumDirection();
}

