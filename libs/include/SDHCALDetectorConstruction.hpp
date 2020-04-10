#pragma once

#include "G4VUserDetectorConstruction.hh"
#include "SDHCALSteppingAction.h"

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
  ~SDHCALDetectorConstruction() = default ;

  G4VPhysicalVolume* Construct() ;

  static G4double sizeX ;
  static G4double sizeZ ;

protected :
  RPCType rpcType = kNormalRPC ;
  G4bool oldConfig = false ;
  nlohmann::json m_Json{};
};

