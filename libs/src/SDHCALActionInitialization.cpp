#include "SDHCALActionInitialization.hpp"
#include "SDHCALPrimaryGeneratorAction.hpp"
#include "SDHCALRunAction.h"
#include "SDHCALEventAction.h"
#include "SDHCALStackingAction.h"
#include "SDHCALTrackingAction.h"
#include "SDHCALSteppingAction.h"

SDHCALActionInitialization::SDHCALActionInitialization(const nlohmann::json& json): G4VUserActionInitialization(),m_Json(json)
{}

SDHCALActionInitialization::~SDHCALActionInitialization()
{}

void SDHCALActionInitialization::BuildForMaster() const
{
    SDHCALRunAction* runAction = new SDHCALRunAction();
    runAction->setLcioFileName(m_Json.value("outputFileName","output")+G4String(".slcio"));
    runAction->setRootFileName(m_Json.value("outputFileName","output")+G4String(".root"));
    SetUserAction(runAction);
}

void SDHCALActionInitialization::Build() const
{
    // Primary generator action
    SetUserAction ( new SDHCALPrimaryGeneratorAction(m_Json));

    SDHCALRunAction* runAction = new SDHCALRunAction() ;

    runAction->setLcioFileName(m_Json.value("outputFileName","output") + G4String(".slcio"));
    runAction->setRootFileName(m_Json.value("outputFileName","output") + G4String(".root")) ;

    SetUserAction ( runAction ) ;
    SetUserAction ( new SDHCALEventAction(runAction) ) ;

    SetUserAction ( SDHCALSteppingAction::Instance() ) ;
    SetUserAction ( SDHCALTrackingAction::Instance() ) ;
    SetUserAction ( SDHCALStackingAction::Instance() ) ;

    SDHCALStackingAction::Instance()->setKillNeutrons(m_Json.value("killNeutrons",false));
}  
