#include "SDHCALRunAction.hpp"
#include "SDHCALRun.hpp"
#include "SDHCALLcioWriter.hpp"

SDHCALRunAction::SDHCALRunAction(const nlohmann::json& json):G4UserRunAction(),m_Json(json)
{
}

G4Run* SDHCALRunAction::GenerateRun()
{
	return new SDHCALRun ;
}

void SDHCALRunAction::BeginOfRunAction(const G4Run*)
{
  m_Lcio = new SDHCALLcioWriter();
  m_Lcio->setFileName(m_Json.value("outputFileName","output")+G4String(".slcio"));
  m_Lcio->openFile();
  m_Root = new SDHCALRootWriter();
  m_Root->setFileName(m_Json.value("outputFileName","output")+G4String(".root"));
  m_Root->openFile();
}

void SDHCALRunAction::EndOfRunAction(const G4Run*)
{
  m_Root->closeFile() ;
  if(m_Root) delete m_Root;
  m_Root=nullptr;
  m_Lcio->closeFile() ;
  if(m_Lcio) delete m_Lcio;
  m_Lcio=nullptr;
}

