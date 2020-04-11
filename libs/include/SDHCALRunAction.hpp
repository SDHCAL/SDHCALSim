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
		G4Run* GenerateRun() ;

		virtual void BeginOfRunAction(const G4Run*) ;
		virtual void EndOfRunAction(const G4Run*) ;

  inline SDHCALLcioWriter* getLcioWriter() const { return m_Lcio;}
  inline SDHCALRootWriter* getRootWriter() const { return m_Root;}


		SDHCALRunAction(const SDHCALRunAction&) = delete ;
		void operator=(const SDHCALRunAction&) = delete ;

private:
  SDHCALRunAction() = delete ;
  nlohmann::json m_Json{};
  SDHCALLcioWriter* m_Lcio{nullptr};
  SDHCALRootWriter* m_Root{nullptr};
};

