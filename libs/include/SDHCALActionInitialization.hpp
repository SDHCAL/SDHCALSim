#pragma once

#include "G4VUserActionInitialization.hh"
#include "json.hpp"
#include "SDHCALLcioWriter.h"
#include "SDHCALRootWriter.h"
#include <memory>

class SDHCALActionInitialization : public G4VUserActionInitialization
{
public:
  SDHCALActionInitialization(const nlohmann::json&);
  SDHCALActionInitialization()=delete;
  virtual ~SDHCALActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;
private:
  nlohmann::json m_Json{};
};

