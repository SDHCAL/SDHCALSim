#include "LyonRunAction.hh"
#include "LyonRun.hh"
#include "LyonRunActionMessenger.hh"

LyonRunAction::LyonRunAction()
{
 
  runMessenger = new LyonRunActionMessenger(this);
  
 

  
}

LyonRunAction::~LyonRunAction()
{
delete runMessenger;
}

G4Run* LyonRunAction::GenerateRun()
{ 
  return new LyonRun();

}

