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

#include "tinyxml2.h"


struct Params
{
		G4int seed = 0 ;
		G4int nEvent = 1 ;
		G4String physicsList = "FTFP_BERT" ;
		G4String outputFileName = "output" ;
		RPCType rpcType = kNormalRPC ;
} ;

Params readXmlFile(std::string xmlFileName)
{
	Params params ;

	using namespace tinyxml2 ;
	XMLDocument doc ;

	XMLError status = doc.LoadFile( xmlFileName.c_str() ) ;

	if ( status != XML_SUCCESS )
	{
		if ( status == 2 )
			std::cout << "No recover XML file provided or file not existing" << std::endl ;
		else
			std::cerr << "Erreur lors du chargement" << std::endl ;
		throw ;
	}

	XMLHandle handle(&doc) ;
	XMLElement* root = handle.FirstChild().ToElement() ;

	XMLNode* node = root->FirstChild() ;

	while ( node )
	{
		if ( node->Value() == std::string("seed") )
		{
			params.seed = std::atoi( node->ToElement()->GetText() ) ;
		}
		if ( node->Value() == std::string("nEvent") )
		{
			params.nEvent = std::atoi( node->ToElement()->GetText() ) ;
		}
		if ( node->Value() == std::string("rpcType") )
		{
			std::string type = node->ToElement()->GetText() ;

			if ( type == std::string("withScintillator") )
				params.rpcType = kWithScintillatorRPC ;
		}
		if ( node->Value() == std::string("physicsList") )
		{
			params.physicsList = node->ToElement()->GetText() ;
		}
		if ( node->Value() == std::string("outputFileName") )
		{
			params.outputFileName = node->ToElement()->GetText() ;
		}

		node = node->NextSiblingElement() ;
	}

	return params ;
}


int main(int argc , char** argv)
{
	assert ( argc > 1 ) ;

	Params params = readXmlFile( argv[1] ) ;

	CLHEP::HepRandom::setTheSeed(params.seed) ;

	G4RunManager* runManager = new G4RunManager ;

	// Detector construction
	runManager->SetUserInitialization( new SDHCALDetectorConstruction(params.rpcType) ) ;

	// Physics list
	G4PhysListFactory* physFactory = new G4PhysListFactory() ;
	runManager->SetUserInitialization( physFactory->GetReferencePhysList(params.physicsList) ) ;

	// Primary generator action
	runManager->SetUserAction( new SDHCALPrimaryGeneratorAction( argv[1] ) ) ;

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
