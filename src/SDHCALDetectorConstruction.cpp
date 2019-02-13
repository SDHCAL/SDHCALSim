#include "SDHCALDetectorConstruction.h"

#include <G4NistManager.hh>
#include <G4Box.hh>

#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4RotationMatrix.hh>

#include <G4Region.hh>
#include <G4RegionStore.hh>

#include "json.hpp"

#include "SDHCALMaterials.h"
#include "SDHCALRPC.h"
#include "SDHCALRPCWithScintillator.h"


G4double SDHCALDetectorConstruction::sizeX ;
G4double SDHCALDetectorConstruction::sizeZ ;

SDHCALDetectorConstruction::SDHCALDetectorConstruction(G4String jsonFileName)
{
	rpcType = kNormalRPC ;
	if ( jsonFileName == G4String("") )
	{
		std::cout << "ERROR : no json file provided" << std::endl ;
		std::terminate() ;
	}

	std::ifstream file(jsonFileName) ;
	auto json = nlohmann::json::parse(file) ;

	if ( json.count("detectorConfig") )
	{
		auto detectorConfig = json.at("detectorConfig") ;

		if ( detectorConfig.count("rpcType") )
		{
			auto type = detectorConfig.at("rpcType").get<G4String>() ;

			if ( type == "normal")
				rpcType = kNormalRPC ;
			else if ( type == "withScintillator")
				rpcType = kWithScintillatorRPC ;
			else
			{
				std::cout << "ERROR : RPC Type unknown" << std::endl ;
				std::terminate() ;
			}
		}

		if ( detectorConfig.count("oldConfig") )
			oldConfig = detectorConfig.at("oldConfig").get<G4bool>() ;
	}
}

SDHCALDetectorConstruction::~SDHCALDetectorConstruction()
{

}

G4VPhysicalVolume* SDHCALDetectorConstruction::Construct()
{
	G4int nLayers = 48 ;
	G4int nPadX = 96 ;
	G4int nPadY = 96 ;
	G4double padSize = 10.408*CLHEP::mm ;

	G4double caloSizeX = nPadX*padSize ;
	G4double caloSizeY = nPadY*padSize ;

	G4double worldSize = 10*CLHEP::m ;
	buildSDHCALMaterials() ;

	G4NistManager* man = G4NistManager::Instance() ;
	G4Material* defaultMaterial = man->FindOrBuildMaterial("G4_Galactic") ;
	G4Material* airMaterial = man->FindOrBuildMaterial("G4_AIR") ;
	G4Material* absorberMaterial = G4Material::GetMaterial("SDHCAL_Steel304L" , true) ;
	if ( oldConfig ) //to reproduce old results
		absorberMaterial = G4Material::GetMaterial("SDHCAL_Steel304L_Old" , true) ;


	// World
	G4Box* solidWorld = new G4Box("World", worldSize/2 , worldSize/2 , worldSize/2) ;
	G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld , defaultMaterial , "World") ;
	G4VPhysicalVolume* physiWorld = new G4PVPlacement(0 , G4ThreeVector() ,  logicWorld , "World" , 0 , false , 0 , true) ;

	std::vector<SDHCALRPC*> rpcVec ;
	for ( G4int i = 0 ; i < nLayers ; ++i )
	{
		if ( rpcType == kNormalRPC )
			rpcVec.push_back( new SDHCALRPC(i , nPadX , nPadY , padSize , oldConfig) ) ;
		else if ( rpcType == kWithScintillatorRPC )
			rpcVec.push_back( new SDHCALRPCWithScintillator(i , nPadX , nPadY , padSize) ) ;
	}

	G4double RPCSizeZ = rpcVec.at(0)->getSizeZ() ;

	G4double absorberStructureSizeZ = 15*CLHEP::mm ;

	G4double airGapSizeZ = 1*CLHEP::mm ;
	if ( oldConfig ) //to reproduce old results
		airGapSizeZ = 0*CLHEP::mm ;

	G4double caloSizeZ = nLayers*( absorberStructureSizeZ + 2*airGapSizeZ + RPCSizeZ ) ;

	sizeX = caloSizeX ;
	sizeZ = caloSizeZ ;

	//calorimeter volume
	G4Box* solidCalorimeter = new G4Box("Calorimeter" , caloSizeX/2 , caloSizeY/2 , caloSizeZ/2) ;
	G4LogicalVolume* logicCalorimeter = new G4LogicalVolume(solidCalorimeter , airMaterial , "Calorimeter") ;

	G4double currentPos = -caloSizeZ/2 ;
	for ( unsigned int i = 0 ; i < static_cast<unsigned int>( nLayers ) ; ++i )
	{
		currentPos += absorberStructureSizeZ/2 ;
		G4Box* solidAbsorberStructure = new G4Box("AbsorberStructure" , caloSizeX/2 , caloSizeY/2 , absorberStructureSizeZ/2) ;
		G4LogicalVolume* logicAbsorberStructure = new G4LogicalVolume(solidAbsorberStructure , absorberMaterial , "AbsorberStructure") ;
		new G4PVPlacement(0 , G4ThreeVector(0,0,currentPos) , logicAbsorberStructure , "AbsorberStructure" , logicCalorimeter , false , 0 , true) ;

		currentPos += absorberStructureSizeZ/2 + airGapSizeZ + RPCSizeZ/2 ;

		std::stringstream name ; name << "Cassette" << i ;
		rpcVec.at(i)->createPhysicalVolume(0 , G4ThreeVector(0,0,currentPos) , logicCalorimeter) ;

		currentPos += RPCSizeZ/2 + airGapSizeZ ;
	}

	G4VPhysicalVolume* calorimeter = new G4PVPlacement( NULL , G4ThreeVector(0 , 0 , 0.5*caloSizeZ) , logicCalorimeter , "Calorimeter" , logicWorld , false , 0 , true) ;

	//for stepping action
	G4Region* regionCalor = G4RegionStore::GetInstance()->FindOrCreateRegion("RegionCalorimeter") ;
	regionCalor->AddRootLogicalVolume(logicCalorimeter) ;

	SDHCALSteppingAction::Instance()->setInterestedRegion(regionCalor) ;
	SDHCALSteppingAction::Instance()->setPhysicalVolume(calorimeter) ;

	return physiWorld ;
}
