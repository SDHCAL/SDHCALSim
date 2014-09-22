#ifndef LyonPrimaryGeneratorAction_h
#define LyonPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;//genere un faisceau de particules dont on determine le nombre
class G4Event; //

class LyonPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    LyonPrimaryGeneratorAction();
    ~LyonPrimaryGeneratorAction();

  public:
  void GeneratePrimaries(G4Event* anEvent); //methode de G4VUserPrimaryGeneratorAction

  private:
  G4ParticleGun* particleGun;
};

#endif


