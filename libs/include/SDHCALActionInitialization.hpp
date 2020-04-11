#pragma once

#include "G4VUserActionInitialization.hh"

#include "SDHCALLcioWriter.hpp"
#include "SDHCALRootWriter.hpp"

#include "json.hpp"

class SDHCALActionInitialization : public G4VUserActionInitialization
{
public:
  SDHCALActionInitialization(const nlohmann::json&);
  virtual void BuildForMaster() const;
  virtual void Build() const;
private:
  SDHCALActionInitialization()=delete;
  virtual ~SDHCALActionInitialization();
  nlohmann::json m_Json{};
};

