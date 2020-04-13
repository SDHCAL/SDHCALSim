#include "G4PhysListFactory.hh"

#include "SDHCALDetectorConstruction.hpp"
#include "SDHCALActionInitialization.hpp"

#ifdef G4MULTITHREADED
  #include "G4MTRunManager.hh"
#else
  #include "G4RunManager.hh"
#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#include "json.hpp"
#include <memory>

int main(int argc , char** argv)
{
  #ifdef G4UI_USE
  G4cout<<"Compile with G4UI_USE"<<G4endl;
  #endif
  #ifdef G4VIS_USE
  G4cout<<"Compile with G4VIS_USE"<<G4endl;
  #endif
  if(argc !=2)
  {
    G4cerr << "Please profile a json file as argument !" << G4endl ;
    std::exit(-1) ;
  }

  G4String jsonFileName = argv[1] ;
  if(jsonFileName == G4String(""))
  {
    G4cerr << "ERROR : no json file provided" << G4endl ;
    std::exit(-1) ;
  }
  std::ifstream file(jsonFileName.c_str()) ;
  nlohmann::json json{};
  try
  {
    json = nlohmann::json::parse(file);
  }
  catch (nlohmann::json::parse_error& e)
  {
    G4cerr <<  "message: " << e.what() << '\n'
              << "exception id: " << e.id << '\n'
              << "byte position of error: " << e.byte << G4endl ;
  }

  CLHEP::HepRandom::setTheSeed(json.value("seed",0)+1);

  G4PhysListFactory* physFactory = new G4PhysListFactory();
  SDHCALDetectorConstruction* detectorConstruction=new SDHCALDetectorConstruction(json);
  #ifdef G4MULTITHREADED
    if(json.value("MultiThread",false))
    {
      std::unique_ptr<G4MTRunManager> runManagerMT{std::make_unique<G4MTRunManager>()};
      runManagerMT->SetNumberOfThreads(json.value("NbrThreads",4));
      // Detector construction
      runManagerMT->SetUserInitialization(detectorConstruction);
      // Physics list
      runManagerMT->SetUserInitialization(physFactory->GetReferencePhysList(json.value("physicsList","FTFP_BERT")));
      runManagerMT->SetUserInitialization(new SDHCALActionInitialization(json,detectorConstruction));
      runManagerMT->Initialize() ;
      runManagerMT->BeamOn(json.value("nEvents",1));
    }
    else
    {
  #endif
      std::unique_ptr<G4RunManager> runManager{std::make_unique<G4RunManager>()};
      // Detector construction
      runManager->SetUserInitialization(detectorConstruction);
      // Physics list
      runManager->SetUserInitialization(physFactory->GetReferencePhysList(json.value("physicsList","FTFP_BERT")));
      runManager->SetUserInitialization(new SDHCALActionInitialization(json,detectorConstruction));
      runManager->Initialize() ;
      runManager->BeamOn(json.value("nEvents",1));
  #ifdef G4MULTITHREADED
    }
  #endif
  return 0 ;
}
