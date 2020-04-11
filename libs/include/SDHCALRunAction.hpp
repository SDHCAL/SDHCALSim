#pragma once

#include <globals.hh>
#include <G4UserRunAction.hh>

#include "SDHCALLcioWriter.hpp"
#include "SDHCALRootWriter.hpp"

#include <string>
#include "json.hpp"

class G4Run ;

class SDHCALRunAction : public G4UserRunAction
{
public:
  SDHCALRunAction(const nlohmann::json& json);
  virtual ~SDHCALRunAction() = default ;
  inline void setLcioFileName(const G4String& name) {lcioFileName = name;}
  inline void setRootFileName(const G4String& name) {rootFileName = name;}
		G4Run* GenerateRun() ;

		virtual void BeginOfRunAction(const G4Run*) ;
		virtual void EndOfRunAction(const G4Run*) ;

		inline SDHCALLcioWriter* getWriter() const { return writer ; }



		SDHCALRunAction(const SDHCALRunAction&) = delete ;
		void operator=(const SDHCALRunAction&) = delete ;

private:
  SDHCALRunAction() = delete ;
  nlohmann::json m_Json{};
		SDHCALLcioWriter* writer = nullptr ;

		G4String lcioFileName = "test.slcio" ;
		G4String rootFileName = "test.root" ;
};

