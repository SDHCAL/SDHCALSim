#include "SDHCALDetectorConstruction.hpp"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"

#include "SDHCALMaterials.hpp"
#include "SDHCALRPC.hpp"

SDHCALDetectorConstruction::SDHCALDetectorConstruction(const nlohmann::json& json):m_Json(json)
{
  if(m_Json.count("detectorConfig"))
  {
    auto detectorConfig = m_Json.at("detectorConfig");
    if(detectorConfig.count("rpcType"))
    {
      auto type = detectorConfig.at("rpcType").get<G4String>();
      if(type == "normal") rpcType = kNormalRPC;
      else if(type == "withScintillator")rpcType = kWithScintillatorRPC;
      else
      {
        G4cerr<< "ERROR : RPC Type unknown" <<G4endl;
        std::exit(-1);
      }
    }
    if(detectorConfig.count("oldConfig"))oldConfig = detectorConfig.at("oldConfig").get<G4bool>();
  }
}

G4VPhysicalVolume* SDHCALDetectorConstruction::Construct()
{
  m_CaloSizeX=m_NbrPadX*m_PadSizeX;
  m_CaloSizeY=m_NbrPadY*m_PadSizeY;
  G4double worldSize{10*CLHEP::m};
  buildSDHCALMaterials();
  std::cout<<"Detector"<<m_CaloSizeX<<"  "<<m_CaloSizeY<<std::endl;
  G4NistManager* man = G4NistManager::Instance();
  G4Material* defaultMaterial = man->FindOrBuildMaterial("G4_Galactic");
  G4Material* airMaterial = man->FindOrBuildMaterial("G4_AIR");
	G4Material* absorberMaterial = G4Material::GetMaterial("SDHCAL_Steel304L" , true);
  //to reproduce old results
  if(oldConfig) absorberMaterial = G4Material::GetMaterial("SDHCAL_Steel304L_Old" , true);

  // World
  G4Box* solidWorld = new G4Box("World", worldSize/2 , worldSize/2 , worldSize/2);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld , defaultMaterial , "World");
  G4VPhysicalVolume* physiWorld=new G4PVPlacement(nullptr,G4ThreeVector(),logicWorld,"World",nullptr,false,0,true);

  std::vector<SDHCALRPC> rpcVec ;
  for ( G4int i = 0 ; i < m_NbrLayers ; ++i )
  {
    rpcVec.push_back(SDHCALRPC(i));
    rpcVec[i].setSizes(m_CaloSizeX,m_CaloSizeY,0);
    rpcVec[i].setCellXYSize(m_PadSizeX,m_PadSizeY);
    if(rpcType == kNormalRPC)
    {
      if(oldConfig)rpcVec[i].buildOldStandardRPC();
      else rpcVec[i].buildStandardRPC();		
    }
    else if(rpcType == kWithScintillatorRPC) rpcVec[i].buildWithScintillatorRPC();
  }

  G4double RPCSizeZ = rpcVec.at(0).getSizeZ();
  G4double absorberStructureSizeZ = 15*CLHEP::mm;

  G4double airGapSizeZ = 1*CLHEP::mm;
  //to reproduce old results
  if(oldConfig) airGapSizeZ = 0*CLHEP::mm;

  m_CaloSizeZ = m_NbrLayers*(absorberStructureSizeZ + 2*airGapSizeZ + RPCSizeZ );
  //calorimeter volume
  G4Box* solidCalorimeter = new G4Box("Calorimeter",m_CaloSizeX/2,m_CaloSizeY/2,m_CaloSizeZ/2);
  G4LogicalVolume* logicCalorimeter = new G4LogicalVolume(solidCalorimeter,airMaterial,"Calorimeter");

  G4double currentPos = -m_CaloSizeZ/2;
  for ( G4int i = 0 ; i < m_NbrLayers ; ++i )
  {
    currentPos += absorberStructureSizeZ/2;
    G4Box* solidAbsorberStructure = new G4Box("AbsorberStructure",m_CaloSizeX/2,m_CaloSizeY/2,absorberStructureSizeZ/2);
    G4LogicalVolume* logicAbsorberStructure = new G4LogicalVolume(solidAbsorberStructure,absorberMaterial,"AbsorberStructure");
    new G4PVPlacement(nullptr,G4ThreeVector(0,0,currentPos),logicAbsorberStructure,"AbsorberStructure",logicCalorimeter,false,0,true);

    currentPos += absorberStructureSizeZ/2 + airGapSizeZ + RPCSizeZ/2;
    std::stringstream name ; name << "Cassette" << i;
    rpcVec.at(i).createPhysicalVolume(nullptr,G4ThreeVector(0,0,currentPos),logicCalorimeter);
    currentPos += RPCSizeZ/2 + airGapSizeZ;
  }

  G4VPhysicalVolume* calorimeter = new G4PVPlacement(nullptr,G4ThreeVector(0,0,0.5*m_CaloSizeZ),logicCalorimeter,"Calorimeter",logicWorld,false,0,true);

  //for stepping action
  G4Region* regionCalor = G4RegionStore::GetInstance()->FindOrCreateRegion("RegionCalorimeter");
  regionCalor->AddRootLogicalVolume(logicCalorimeter);
  calorimeter=calorimeter;
  return physiWorld;
}
