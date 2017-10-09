#include "SDHCALDetectorConstruction.h"
#include "SDHCALPrimaryGeneratorAction.h"
#include "SDHCALRunAction.h"
#include "SDHCALEventAction.h"
#include "SDHCALTrackingAction.h"


//#include <G4UserEventAction.hh>
#include <G4PhysListFactory.hh>

#include "G4RunManager.hh"
#include "G4UImanager.hh"



#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#include "G4TrajectoryDrawByParticleID.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include <string>

#include <Randomize.hh>


int main(int argc , char** argv)
{
//	std::string physList = "FTFP_BERT" ;
	std::string physList = "QGSP_BERT" ;
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
	runManager->SetUserInitialization( new SDHCALDetectorConstruction() ) ;

	// Physics list
	G4PhysListFactory* physFactory = new G4PhysListFactory() ;
	runManager->SetUserInitialization( physFactory->GetReferencePhysList(physList) ) ;

	// Primary generator action
	runManager->SetUserAction( new SDHCALPrimaryGeneratorAction() ) ;

	SDHCALRunAction* runAction = new SDHCALRunAction() ;

	runManager->SetUserAction( runAction ) ;
	runManager->SetUserAction( new SDHCALEventAction(runAction) ) ;

	runManager->SetUserAction( SDHCALSteppingAction::Instance() ) ;
	runManager->SetUserAction( SDHCALTrackingAction::Instance() ) ;

	// Initialize G4 kernel
	runManager->Initialize() ;

//	G4UImanager* UI = G4UImanager::GetUIpointer() ;

//	G4String command = "/control/execute " ;
//	G4String fileName = argv[1] ;
//	UI->ApplyCommand(command+fileName) ;


#ifdef G4VIS_USE
	// Initialize visualization
	G4VisManager* visManager = new G4VisExecutive ;
	// G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
	// G4VisManager* visManager = new G4VisExecutive("Quiet");
	visManager->Initialize() ;

	G4TrajectoryDrawByParticleID* model = new G4TrajectoryDrawByParticleID ;

	 model->SetDefault( G4Colour(1 , 1 , 1 , 0) ) ;
	 model->Set("e+", "red") ;
	 model->Set("e-", "red") ;
	 model->Set("pi0", "red") ;
	 model->Set("gamma", G4Colour(1 , 0 , 0 , 0.05) ) ;

	 model->Set("pi-", "green" ) ;
	 model->Set("pi+", "green" ) ;
	 model->Set("proton", "green" ) ;
	 model->Set("kaon+", "green" ) ;
	 model->Set("kaon-", "green" ) ;
	 model->Set("neutron", G4Colour(0 , 1 , 0 , 0.05) ) ;

	 model->Set("mu+", "blue" ) ;
	 model->Set("mu-", "blue" ) ;

	 visManager->RegisterModel(model) ;

	 visManager->SelectTrajectoryModel( model->Name() ) ;

#endif

	// Get the pointer to the User Interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer() ;

	if (argc != 1)
	{
		// batch mode
		G4String command = "/control/execute " ;
		G4String fileName = argv[1] ;
		UImanager->ApplyCommand(command+fileName) ;
	}
	else
	{
		// interactive mode : define UI session

#ifdef G4UI_USE
		G4UIExecutive* ui = new G4UIExecutive(argc, argv) ;
#ifdef G4VIS_USE
		UImanager->ApplyCommand("/control/execute init_vis.mac") ;
#else
		UImanager->ApplyCommand("/control/execute init.mac");
#endif
		if ( ui->IsGUI() )
		{
			UImanager->ApplyCommand("/control/execute icons.mac") ;
		}
		ui->SessionStart() ;
		delete ui ;
#endif

	}

	// Job termination
	// Free the store: user actions, physics_list and detector_description are
	// owned and deleted by the run manager, so they should not be deleted
	// in the main() program !

#ifdef G4VIS_USE
	delete visManager ;
#endif


	delete runManager ;

	return 0 ;
}
