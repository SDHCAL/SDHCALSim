#include "SDHCALDetectorConstruction.h"
#include "SDHCALPrimaryGeneratorAction.h"
#include "SDHCALRunAction.h"
#include "SDHCALEventAction.h"


//#include <G4UserEventAction.hh>
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

#include <Randomize.hh>


//Same simulation but with custom RPC (with Scintillator)

int main(int argc , char** argv)
{
	std::string physList = "FTFP_BERT" ;
	G4int seed = 0 ;

	if ( argc >= 4 )
	{
		std::cout << "SEED : " << std::atoi(argv[2]) << std::endl ;
		std::cout << "PHYSLIST : " << std::string( argv[3] ) << std::endl ;
		seed = std::atoi( argv[2] ) ;
		physList = std::string( argv[3] ) ;
	}

	CLHEP::HepRandom::setTheSeed(seed) ;


	G4RunManager* runManager = new G4RunManager ;

	// Detector construction
	runManager->SetUserInitialization( new SDHCALDetectorConstruction( kWithScintillatorRPC ) ) ;

	// Physics list
	G4PhysListFactory* physFactory = new G4PhysListFactory() ;
	runManager->SetUserInitialization( physFactory->GetReferencePhysList(physList) ) ;

	// Primary generator action
	runManager->SetUserAction( new SDHCALPrimaryGeneratorAction() ) ;

	SDHCALRunAction* runAction = new SDHCALRunAction() ;

	runAction->setLcioFileName( "wScintillator.slcio" ) ;
	runAction->setRootFileName( "wScintillator.root" ) ;

	runManager->SetUserAction( runAction ) ;
	runManager->SetUserAction( new SDHCALEventAction(runAction) ) ;

	runManager->SetUserAction( SDHCALSteppingAction::Instance() ) ;

	// Initialize G4 kernel
	runManager->Initialize() ;

	G4UImanager* UI = G4UImanager::GetUIpointer() ;

	G4String command = "/control/execute " ;
	G4String fileName = argv[1] ;
	UI->ApplyCommand(command+fileName) ;

	delete runManager ;

	return 0 ;
}
