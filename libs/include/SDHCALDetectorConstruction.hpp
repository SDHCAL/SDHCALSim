#pragma once

#include "G4VUserDetectorConstruction.hh"

#include "json.hpp"
#include "CLHEP/Units/SystemOfUnits.h"

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
  G4double getCaloSizeX(){return caloSizeX;}
  G4double getCaloSizeY(){return caloSizeY;}
  G4double getCaloSizeZ(){return caloSizeZ;}

protected :
  RPCType rpcType = kNormalRPC ;
  G4bool oldConfig = false ;
  nlohmann::json m_Json{};
  G4int nLayers = 48 ;
  G4int nPadX = 96 ;
  G4int nPadY = 96 ;
  G4double padSizeX = 10.408*CLHEP::mm ;
  G4double padSizeY = 10.408*CLHEP::mm ;
  G4double caloSizeX = {nPadX*padSizeX} ;
  G4double caloSizeY = {nPadY*padSizeY} ;
  G4double caloSizeZ ={0.0};
};

