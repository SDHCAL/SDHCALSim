#pragma once

#include "G4VUserDetectorConstruction.hh"
#include "CLHEP/Units/SystemOfUnits.h"

#include "json.hpp"

#include "PatternTypes.hpp"

class SDHCALDetectorConstruction : public G4VUserDetectorConstruction
{
public :
  SDHCALDetectorConstruction(const nlohmann::json& json);
  ~SDHCALDetectorConstruction() = default;
  G4VPhysicalVolume* Construct();
  virtual void 	ConstructSDandField();
  G4double getCaloSizeX(){return m_CaloSizeX;}
  G4double getCaloSizeY(){return m_CaloSizeY;}
  G4double getCaloSizeZ(){return m_CaloSizeZ;}

private:
  G4String m_TypesRPC{""};
  G4String m_TypesAbsorber{""};
  std::vector<G4LogicalVolume*> m_GasGap;
  std::vector<G4LogicalVolume*> m_LogicRPC;
  G4bool oldConfig = false ;
  nlohmann::json m_Json{};
  G4int m_NbrLayers{0};
  G4int m_NbrCellX{0};
  G4int m_NbrCellY{0};
  G4double m_CellSizeX{10.408*CLHEP::mm};
  G4double m_CellSizeY{10.408*CLHEP::mm};
  G4double m_CaloSizeX{0.};
  G4double m_CaloSizeY{0.};
  G4double m_CaloSizeZ{0.};
  PatternTypes m_RPCTypes{PatternTypes("RPC")};
  PatternTypes m_AbsorberTypes{PatternTypes("Absorber")};
};

