#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
//#include "G4UIExecutive.hh"
#include "G4UItcsh.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#include "LyonDetectorConstruction.hh"
//#include "LyonPhysicsList.hh" deprecated from geant 4.9.4
#include "LyonPrimaryGeneratorAction.hh"
#include "LyonRunAction.hh"
#include "LyonDetectorFieldSetup.hh"
#include "FTFP_BERT.hh"

#include "Randomize.hh" //modif pour random seed

//for lcio file

//using std::string;

int main(int argc,char** argv)
{

  //modif pour random seed
  G4int seed=-1;
  if(argc > 2) seed=atoi(argv[2]);
  //if(seed<0)   seed=time(0);
  if(seed<0)   seed=1;
  G4cout << "Chosen seed is=" << seed << G4endl;
  CLHEP::HepRandom::setTheSeed(seed);

  // Construct the default run manager
  //
  G4RunManager* runManager = new G4RunManager;

  // set mandatory initialization classes
  //


  G4VUserDetectorConstruction* detector = new LyonDetectorConstruction;
  runManager->SetUserInitialization(detector);
  //
 

#ifdef G4VIS_USE
  // Visualization, if you choose to have it!
  //
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif

  G4VUserPhysicsList* physics = new FTFP_BERT;
  runManager->SetUserInitialization(physics);
  // set mandatory user action class
  //
  LyonPrimaryGeneratorAction* gen_action = new LyonPrimaryGeneratorAction();
  runManager->SetUserAction(gen_action);

  // set optional user action class
  //
  G4UserRunAction* run_action = new LyonRunAction;
  //LyonDigi *toto=new LyonDigi();
  //G4UserRunAction* run_action = new LyonRunAction(toto);//s'il y a plusieurs LyonRun utilisés
  runManager->SetUserAction(run_action);

  
  //  G4UIExecutive* UIExe=new G4UIExecutive(argc,argv);
  
  runManager->Initialize();

  // Initialize G4 kernel
  //
  // runManager->Initialize();

  // Get the pointer to the UI manager and set verbosities
  //
  G4UImanager* UI = G4UImanager::GetUIpointer();
  
  //UI->ApplyCommand("/run/verbose 1");
  //  UI->ApplyCommand("/event/verbose 1");
  //UI->ApplyCommand("/tracking/verbose 1");
  if(argc==1)  // Define (G)UI terminal for interactive mode
  {
    //    std::cout << "toto" << std::endl;
    // G4UIterminal is a (dumb) terminal
    //
    G4UIsession * session = 0;
#ifdef G4UI_USE_TCSH
      session = new G4UIterminal(new G4UItcsh);
#else
      session = new G4UIterminal();
#endif
      UI->ApplyCommand("/control/execute mac/vis.mac");
      session->SessionStart();
      //UIExe->SessionStart();
      delete session;
      //delete UIExe;
  }
  else   // Batch mode
  {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UI->ApplyCommand(command+fileName);
  }

  


  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !
  //
#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;

  return 0;
}
