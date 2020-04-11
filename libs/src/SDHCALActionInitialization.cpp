#include "SDHCALActionInitialization.hpp"

#include "SDHCALPrimaryGeneratorAction.hpp"
#include "SDHCALRunAction.hpp"
#include "SDHCALEventAction.hpp"
#include "SDHCALStackingAction.hpp"
#include "SDHCALTrackingAction.hpp"
#include "SDHCALSteppingAction.hpp"

SDHCALActionInitialization::SDHCALActionInitialization(const nlohmann::json& json): G4VUserActionInitialization(),m_Json(json)
{}

SDHCALActionInitialization::~SDHCALActionInitialization()
{}

void SDHCALActionInitialization::BuildForMaster() const
{
  SetUserAction(new SDHCALRunAction(m_Json));
}

void SDHCALActionInitialization::Build() const
{
  // Primary generator action
  SetUserAction(new SDHCALPrimaryGeneratorAction(m_Json));
  SDHCALRunAction* runAction = new SDHCALRunAction(m_Json);
  runAction->setLcioFileName(m_Json.value("outputFileName","output") + G4String(".slcio"));
  runAction->setRootFileName(m_Json.value("outputFileName","output") + G4String(".root")) ;

  SetUserAction(runAction);
  SetUserAction ( new SDHCALEventAction(runAction) ) ;

  SetUserAction ( SDHCALSteppingAction::Instance() ) ;
  SetUserAction ( SDHCALTrackingAction::Instance() ) ;
  SetUserAction ( SDHCALStackingAction::Instance() ) ;

  SDHCALStackingAction::Instance()->setKillNeutrons(m_Json.value("killNeutrons",false));
}
