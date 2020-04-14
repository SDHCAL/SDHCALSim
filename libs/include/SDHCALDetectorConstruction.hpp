#pragma once

#include "G4VUserDetectorConstruction.hh"
#include "CLHEP/Units/SystemOfUnits.h"

#include "json.hpp"

enum RPCType
{
	kNormalRPC,
	kWithScintillatorRPC
} ;

class SDHCALDetectorConstruction : public G4VUserDetectorConstruction
{
public :
  SDHCALDetectorConstruction(const nlohmann::json& json);
  ~SDHCALDetectorConstruction() = default;
  G4VPhysicalVolume* Construct();
  G4double getCaloSizeX(){return m_CaloSizeX;}
  G4double getCaloSizeY(){return m_CaloSizeY;}
  G4double getCaloSizeZ(){return m_CaloSizeZ;}

private:
  RPCType rpcType = kNormalRPC ;
  G4bool oldConfig = false ;
  nlohmann::json m_Json{};
  G4int m_NbrLayers{48};
  G4int m_NbrPadX{96};
  G4int m_NbrPadY{96};
  G4double m_PadSizeX{10.408*CLHEP::mm};
  G4double m_PadSizeY{10.408*CLHEP::mm};
  G4double m_CaloSizeX{m_NbrPadX*m_PadSizeX};
  G4double m_CaloSizeY{m_NbrPadY*m_PadSizeY};
  G4double m_CaloSizeZ{0.0};
};

