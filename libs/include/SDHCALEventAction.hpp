#pragma once

#include "G4UserEventAction.hh"

#include <ctime>

class SDHCALRunAction;
class SDHCALSteppingAction;
class SDHCALStackingAction;
class SDHCALTrackingAction;
class SDHCALPrimaryGeneratorAction;

class SDHCALEventAction : public G4UserEventAction
{
public :
  SDHCALEventAction(SDHCALRunAction*,SDHCALSteppingAction*,SDHCALStackingAction*,SDHCALTrackingAction*,SDHCALPrimaryGeneratorAction*);
  virtual ~SDHCALEventAction() = default ;

  virtual void BeginOfEventAction(const G4Event* event) ;
  virtual void EndOfEventAction(const G4Event* event) ;

private :
  clock_t beginClock{};
  double averageTime{0.};
  unsigned int nEventsProcessed{0};
  SDHCALRunAction* m_RunAction{nullptr};
  SDHCALSteppingAction* m_SteppingAction{nullptr};
  SDHCALStackingAction* m_StackingAction{nullptr};
  SDHCALTrackingAction* m_TrackingAction{nullptr};
  SDHCALPrimaryGeneratorAction* m_PrimaryGeneratorAction{nullptr};
};
