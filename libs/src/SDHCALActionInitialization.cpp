#include "SDHCALActionInitialization.hpp"

#include "SDHCALPrimaryGeneratorAction.hpp"
#include "SDHCALRunAction.hpp"
#include "SDHCALEventAction.hpp"
#include "SDHCALStackingAction.hpp"
#include "SDHCALTrackingAction.hpp"
#include "SDHCALSteppingAction.hpp"
#include "SDHCALDetectorConstruction.hpp"

SDHCALActionInitialization::SDHCALActionInitialization(const nlohmann::json& json,SDHCALDetectorConstruction* detector): G4VUserActionInitialization(),m_Json(json),m_Detector(detector)
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
  SetUserAction(new SDHCALPrimaryGeneratorAction(m_Json,m_Detector));
  
  SDHCALRunAction* runAction = new SDHCALRunAction(m_Json);
  SetUserAction(runAction);
  
  SDHCALSteppingAction* steppingAction=new SDHCALSteppingAction();
  SetUserAction(steppingAction);
  SetUserAction ( SDHCALTrackingAction::Instance() ) ;
  
  SDHCALStackingAction* stackingAction=new SDHCALStackingAction(m_Json);
  SetUserAction(stackingAction);
  
  SetUserAction(new SDHCALEventAction(runAction,steppingAction,stackingAction));
}
