#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"

#include "SDHCALGun.hpp"

#include "json.hpp"

#include<memory>

class SDHCALPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  SDHCALPrimaryGeneratorAction(const nlohmann::json& json);
  virtual ~SDHCALPrimaryGeneratorAction()=default;
  void print() const;
  virtual void GeneratePrimaries(G4Event* event);
  const std::vector<std::shared_ptr<SDHCALGun>>& getGunVec() const {return m_GunVec;}

private:
  SDHCALPrimaryGeneratorAction()=default;
  std::vector<std::shared_ptr<SDHCALGun>> m_GunVec{};
  nlohmann::json m_Json{};
};
