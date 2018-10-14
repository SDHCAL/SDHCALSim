#include "SDHCALRunAction.h"
#include "SDHCALRun.h"
#include "SDHCALLcioWriter.h"

#include <globals.hh>
#include <ctime>

//SDHCALRunAction::SDHCALRunAction()
//	: G4UserRunAction()
//{
//}

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

