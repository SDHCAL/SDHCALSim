#include "SDHCALDetectorConstruction.h"
#include "SDHCALPrimaryGeneratorAction.h"
#include "SDHCALRunAction.h"
#include "SDHCALEventAction.h"
#include "SDHCALTrackingAction.h"

#include <G4PhysListFactory.hh>

#include "G4RunManager.hh"
#include "G4UImanager.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include <string>

#include "json.hpp"


struct Params
{
		G4int seed = 0 ;
		G4int nEvent = 1 ;
		G4String physicsList = "FTFP_BERT" ;
		G4String outputFileName = "output" ;
} ;

Params readJsonFile(G4String jsonFileName)
{
	Params params ;

	if ( jsonFileName == G4String("") )
	{
		std::cout << "ERROR : no json file provided" << std::endl ;
		std::terminate() ;
	}

	std::ifstream file(jsonFileName) ;
	auto json = nlohmann::json::parse(file) ;

	if ( json.count("outputFileName") )
		params.outputFileName = json.at("outputFileName").get<G4String>() ;
	if ( json.count("physicsList") )
		params.physicsList = json.at("physicsList").get<G4String>() ;
	if ( json.count("nEvents") )
		params.nEvent = json.at("nEvents").get<G4int>() ;
	if ( json.count("seed") )
		params.seed = json.at("seed").get<G4int>()  + 1 ;

	return params ;
}

int main(int argc , char** argv)
{
	assert ( argc > 1 ) ;
	G4String jsonFileName = argv[1] ;

	Params params = readJsonFile( jsonFileName ) ;

	CLHEP::HepRandom::setTheSeed(params.seed) ;

	G4RunManager* runManager = new G4RunManager ;

	// Detector construction
	runManager->SetUserInitialization( new SDHCALDetectorConstruction(jsonFileName) ) ;

	// Physics list
	G4PhysListFactory* physFactory = new G4PhysListFactory() ;
	runManager->SetUserInitialization( physFactory->GetReferencePhysList(params.physicsList) ) ;

	// Primary generator action
	runManager->SetUserAction( new SDHCALPrimaryGeneratorAction( jsonFileName ) ) ;

	SDHCALRunAction* runAction = new SDHCALRunAction() ;

	runAction->setLcioFileName( params.outputFileName + G4String(".slcio") ) ;
	runAction->setRootFileName( params.outputFileName + G4String(".root") ) ;

	runManager->SetUserAction( runAction ) ;
	runManager->SetUserAction( new SDHCALEventAction(runAction) ) ;

	runManager->SetUserAction( SDHCALSteppingAction::Instance() ) ;
	runManager->SetUserAction( SDHCALTrackingAction::Instance() ) ;

	runManager->Initialize() ;
	runManager->BeamOn(params.nEvent) ;

	delete runManager ;

	return 0 ;
}
