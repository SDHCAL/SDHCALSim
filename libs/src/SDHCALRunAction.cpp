#include "SDHCALRunAction.hpp"
#include "SDHCALRun.hpp"
#include "SDHCALLcioWriter.hpp"

SDHCALRunAction::SDHCALRunAction(const nlohmann::json& json):G4UserRunAction(),m_Json(json)
{
  setLcioFileName(m_Json.value("outputFileName","output")+G4String(".slcio"));
  setRootFileName(m_Json.value("outputFileName","output")+G4String(".root"));
}

G4Run* SDHCALRunAction::GenerateRun()
{
	return new SDHCALRun ;
}

void SDHCALRunAction::BeginOfRunAction(const G4Run*)
{
	writer = new SDHCALLcioWriter( lcioFileName ) ;

	SDHCALRootWriter* rootWriter = SDHCALRootWriter::Instance() ;
	rootWriter->createRootFile( rootFileName ) ;
}

void SDHCALRunAction::EndOfRunAction(const G4Run*)
{
	SDHCALRootWriter* rootWriter = SDHCALRootWriter::Instance() ;
	rootWriter->closeRootFile() ;

	SDHCALRootWriter::deleteInstance() ;

	if (writer)
		delete writer ;

	writer = nullptr ;
}

