#ifndef LyonRunAction_h
#define LyonRunAction_h 1

#include "LyonRunActionMessenger.hh"
#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;
class LyonRunActionMessenger;

class LyonRunAction : public G4UserRunAction
{
  public:
    LyonRunAction();
   ~LyonRunAction();
  double threshold;
  void SetThreshold(double val){threshold = val;}
  
public:
  G4Run* GenerateRun();
  
  LyonRunActionMessenger* runMessenger;
  //virtual void BeginOfRunAction(const G4Run*);
  //virtual void EndOfRunAction(const G4Run*);
};


#endif
