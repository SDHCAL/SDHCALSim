#include "SDHCALDetectorConstruction.hpp"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4SDManager.hh"

#include "SDHCALMaterials.hpp"
#include "SDHCALRPCSensitiveDetector.hpp"

void SDHCALDetectorConstruction::ConstructSDandField()
{
  for( G4int i = 0 ; i < m_NbrLayers ; ++i )
  {
     std::string sensName="RPC"+std::to_string(i);
     SDHCALRPCSensitiveDetector* sensitiveDetector = new SDHCALRPCSensitiveDetector(sensName,i);
     sensitiveDetector->setSizes(m_CaloSizeX,m_CaloSizeY,m_CaloSizeZ);
     sensitiveDetector->setCellXYSize(m_CellSizeX,m_CellSizeY);
     G4SDManager::GetSDMpointer()->AddNewDetector(sensitiveDetector);
     m_GasGap[i]->SetSensitiveDetector(sensitiveDetector);
  }
}

SDHCALDetectorConstruction::SDHCALDetectorConstruction(const nlohmann::json& json):m_Json(json)
{
  m_AbsorberTypes.setJsonFile(m_Json);
  m_RPCTypes.setJsonFile(m_Json);
  m_RPCTypes.buildTypes();
  m_AbsorberTypes.buildTypes();
  if(m_Json.count("detectorConfig"))
  {
    auto detectorConfig = m_Json.at("detectorConfig");
    m_NbrLayers=m_Json.at("detectorConfig").value("NumberOfLayers",48);
    m_NbrCellX=detectorConfig.value("NumberCellX",96);
    m_NbrCellY=detectorConfig.value("NumberCellY",96);
    m_CellSizeX=detectorConfig.value("CellSizeX",10.408)*CLHEP::mm;
    m_CellSizeY=detectorConfig.value("CellSizeY",10.408)*CLHEP::mm;
    if(detectorConfig.count("RPCType")) 
    {
      if(!m_RPCTypes.hasType(detectorConfig.at("RPCType").get<G4String>()))
      {
        G4cerr<<"Type "<<detectorConfig.at("RPCType").get<G4String>()<<" unknow"<<std::endl;
        std::exit(-1);
      }
      //FIXME FOR NOW RPCTYPE SHOULD BE THE SAME
      m_TypesRPC=detectorConfig.at("RPCType").get<G4String>();
    }
    else
    {
      G4cerr<<"Please provide RPCType"<<G4endl;
    }
    if(detectorConfig.count("AbsorberType")) 
    {
      if(!m_AbsorberTypes.hasType(detectorConfig.at("AbsorberType").get<G4String>()))
      {
        G4cerr<<"Type "<<detectorConfig.at("AbsorberType").get<G4String>()<<" unknow"<<std::endl;
        std::exit(-1);
      }
      //FIXME FOR NOW RPCTYPE SHOULD BE THE SAME
      m_TypesAbsorber=detectorConfig.at("AbsorberType").get<G4String>();
    }
    else
    {
      G4cerr<<"Please provide AbsorberType"<<G4endl;
    }
    
    
    
    if(detectorConfig.count("oldConfig"))oldConfig = detectorConfig.at("oldConfig").get<G4bool>();
  }
  m_GasGap.reserve(m_NbrLayers);
  m_LogicRPC.reserve(m_NbrLayers);
  for(G4int i=0;i!=m_NbrLayers;++i)
  {
    m_GasGap[i]=nullptr;
    m_LogicRPC[i]=nullptr;
  }
}

G4VPhysicalVolume* SDHCALDetectorConstruction::Construct()
{
  m_CaloSizeX=m_NbrCellX*m_CellSizeX;
  m_CaloSizeY=m_NbrCellY*m_CellSizeY;
  G4double worldSize{10*CLHEP::m};
  buildSDHCALMaterials();
  G4NistManager* man = G4NistManager::Instance();
  
  
	G4Material* absorberMaterial = G4Material::GetMaterial("SDHCAL_Steel304L" , true);
  //to reproduce old results
  if(oldConfig) absorberMaterial = G4Material::GetMaterial("SDHCAL_Steel304L_Old" , true);

  // World
  G4Box* solidWorld = new G4Box("World", worldSize/2 , worldSize/2 , worldSize/2);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld ,G4Material::GetMaterial("G4_Galactic",true), "World");
  G4VPhysicalVolume* physiWorld=new G4PVPlacement(nullptr,G4ThreeVector(),logicWorld,"World",nullptr,false,0,true);

  //Calculate the size of the calorimeter
  for(G4int i = 0;i < m_NbrLayers ;++i)
  {
    //FIXME You will have to change this if you want different rpc or absorber per layer
    m_CaloSizeZ+=m_AbsorberTypes[m_TypesAbsorber].getWidth();
    m_CaloSizeZ+=m_RPCTypes[m_TypesRPC].getWidth();
  }
  
  //Build Calorimeter Volume 
  G4Box* solidCalorimeter = new G4Box("Calorimeter",m_CaloSizeX/2,m_CaloSizeY/2,m_CaloSizeZ/2);
  G4LogicalVolume* logicCalorimeter = new G4LogicalVolume(solidCalorimeter,G4Material::GetMaterial("G4_AIR",true),"Calorimeter");
  //Start at -m_CaloSizeZ/2 as the origin is at the center of the calorimeter box
  
  
  

  for( G4int i = 0 ; i < m_NbrLayers ; ++i)
  {
    G4String m_Name="Cassette"+std::to_string(i);
    //compute total RPC cassette width
    G4double cassetteThickness = m_RPCTypes[m_TypesRPC].getWidth();
    
    //create logic RPC volume (indefined placement)
    G4Box* solidCassette = new G4Box(m_Name , m_CaloSizeX/2, m_CaloSizeY/2, cassetteThickness/2);
    m_LogicRPC[i] = new G4LogicalVolume(solidCassette ,G4Material::GetMaterial("G4_Galactic",true),m_Name);
    
    //construct all the layers 
    G4double zPos = -cassetteThickness/2; //we start at front of the cassette
    for(G4int j=0;j!=m_RPCTypes[m_TypesRPC].getNbrLayers();++j)
    {
      std::cout<<m_RPCTypes[m_TypesRPC].getLayerName(j)<<std::endl;
      auto material = man->FindOrBuildMaterial(m_RPCTypes[m_TypesRPC].getLayerMaterial(j));
      if(!material)
      {
        G4cerr << "Error : material " << m_RPCTypes[m_TypesRPC].getLayerMaterial(j) << " not known" << G4endl;
        std::exit(-1);
      }
      //create logic layer volume (indefined placement)
      auto solid = new G4Box(m_RPCTypes[m_TypesRPC].getLayerName(j) , m_CaloSizeX/2 , m_CaloSizeY/2 , m_RPCTypes[m_TypesRPC].getLayerWidth(j)/2);
      auto logic = new G4LogicalVolume(solid , material ,m_RPCTypes[m_TypesRPC].getLayerName(j)+std::to_string(i));
      
      zPos += m_RPCTypes[m_TypesRPC].getLayerWidth(j)/2; //we are now at center of the current layer (where it has to be placed)
      
      //place the layer at zPos
      new G4PVPlacement(nullptr , G4ThreeVector(0,0,zPos) , logic , m_RPCTypes[m_TypesRPC].getLayerName(j)+std::to_string(i) , m_LogicRPC[i],false,0,true);	//m_LogicRPC is the mother volume
      
      zPos += m_RPCTypes[m_TypesRPC].getLayerWidth(j)/2; //we are now at the back of the current layer
      
      if(m_RPCTypes[m_TypesRPC].getLayerName(j) == "GasGap") //we keep trace of the gas gap
      {
        if(m_GasGap[i]!=nullptr) //we only want one gas gap
        {
          G4cerr << "Error : there is more than one gas gap in the RPC" << G4endl;
          std::exit(-1);
        }
        
        m_GasGap[i] = logic;
      }
    }
    
    if(!m_GasGap[i]) //if we don't have the gas gap we have a problem
    {
      G4cerr << "Error : no gas gap in the RPC" << G4endl;
      std::exit(-1);
    }
  }

  //G4double RPCSizeZ = rpcVec.at(0).getSizeZ();
  G4double absorberStructureSizeZ = 15*CLHEP::mm;

  G4double airGapSizeZ = 1*CLHEP::mm;
 // to reproduce old results
  if(oldConfig) airGapSizeZ = 0*CLHEP::mm;

  
  
  
  
  

  G4double currentPos = -m_CaloSizeZ/2;
  for ( G4int i = 0 ; i < m_NbrLayers ; ++i )
  {
    currentPos += absorberStructureSizeZ/2;
    G4Box* solidAbsorberStructure = new G4Box("AbsorberStructure",m_CaloSizeX/2,m_CaloSizeY/2,absorberStructureSizeZ/2);
    G4LogicalVolume* logicAbsorberStructure = new G4LogicalVolume(solidAbsorberStructure,absorberMaterial,"AbsorberStructure");
    new G4PVPlacement(nullptr,G4ThreeVector(0,0,currentPos),logicAbsorberStructure,"AbsorberStructure",logicCalorimeter,false,0,true);

    currentPos += absorberStructureSizeZ/2 + airGapSizeZ + m_RPCTypes[m_TypesRPC].getWidth()/2;
    new G4PVPlacement(nullptr,G4ThreeVector(0,0,currentPos), m_LogicRPC[i] ,"Cassette"+std::to_string(i) ,logicCalorimeter , false , 0 , true);
    
    currentPos += m_RPCTypes[m_TypesRPC].getWidth()/2 + airGapSizeZ;
  }

  new G4PVPlacement(nullptr,G4ThreeVector(0,0,0.5*m_CaloSizeZ),logicCalorimeter,"Calorimeter",logicWorld,false,0,true);

  //for stepping action
  G4Region* regionCalor = G4RegionStore::GetInstance()->FindOrCreateRegion("RegionCalorimeter");
  regionCalor->AddRootLogicalVolume(logicCalorimeter);
  return physiWorld;
}
