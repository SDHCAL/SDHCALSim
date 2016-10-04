#ifndef LyonPrimaryGeneratorAction_h
#define LyonPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4String.hh"
#include "globals.hh"
#include "iostream"
#include "vector"
#include "G4ThreeVector.hh"
#include "LyonPrimaryGeneratorActionMessenger.hh"

class G4GeneralParticleSource;//genere un faisceau de particules dont on determine le nombre
class G4Event; //
class G4SingleParticleSource;

class LyonPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  LyonPrimaryGeneratorAction();
  ~LyonPrimaryGeneratorAction();

public:
  void GeneratePrimaries(G4Event* anEvent); //methode de G4VUserPrimaryGeneratorAction  
  G4ThreeVector GetPrimaryGeneratorMomentum() const;
  void SetGammaNumber(G4int N){NumberOfGamma = N;};
  void SetMuonNumber(G4int N){NumberOfMuon = N;};
  void SetTimeScale(G4double t){TimeScale = t;};
private:
  G4GeneralParticleSource* particleGun;
  LyonPrimaryGeneratorActionMessenger* _thePrimaryGeneratorActionMessenger;
  G4int NumberOfMuon;
  G4int NumberOfGamma;
  G4int GPCounter;
  G4double TimeScale;
  // G4SingleParticleSource* GammaModel;
  //  G4SingleParticleSource* MuonModel;
};

#endif
