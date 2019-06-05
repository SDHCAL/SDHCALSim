#include "SDHCALRPCWithScintillator.h"

#include <fstream>
#include <iostream>

#include <vector>
#include <sstream>

#include <G4Material.hh>
#include <G4NistManager.hh>

#include <G4Box.hh>

#include <G4LogicalVolume.hh>
#include <G4ThreeVector.hh>
#include <G4PVPlacement.hh>

#include <globals.hh>
#include <G4SDManager.hh>

#include <G4GeometryManager.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4SolidStore.hh>
#include <G4RegionStore.hh>
#include <G4Region.hh>

#include "SDHCALRPCSensitiveDetector.h"

SDHCALRPCWithScintillator::SDHCALRPCWithScintillator(G4int _id , G4int _nPadX , G4int _nPadY, G4double _cellSize )
	: SDHCALRPC()
{
	id = _id ;

	nPadX = _nPadX ;
	nPadY = _nPadY ;
	cellSize = _cellSize ;

	G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance() ;
	std::stringstream sname ; sname << "Cassette" << id ;
	name = sname.str() ;
	if ( store->GetVolume(name , false) )
		G4cout << "WARNING : RPC with ID=" << id << " already existing" << G4endl ;

	sizeX = _nPadX*_cellSize ;
	sizeY = _nPadY*_cellSize ;

	transformComputed = false ;

	logicRPC = nullptr ;
	physicRPC = nullptr ;
	sensitiveDetector = nullptr ;
	physiGasGap = nullptr ;

	SDHCALRPCWithScintillator::getMaterials() ;
	SDHCALRPCWithScintillator::build() ;

	allTheRPC.insert(this) ;
}


void SDHCALRPCWithScintillator::build()
{
	struct LayerInRPC
	{
		std::string name ;
		G4double width ;
		G4Material* material ;
		G4bool isGasGap ;
	} ;

	using CLHEP::mm ;

 	//layers of the RPC ordered from front to back
	std::vector<LayerInRPC> layers ;
	layers.push_back( {"FrontCassetteAbsorber" , 2.500*mm , absorberMaterial     , false} ) ;
	layers.push_back( {"MylarAnode"            , 0.015*mm , mylarMaterial        , false} ) ;	
	layers.push_back( {"GraphiteAnode"         , 0.050*mm , graphiteMaterial     , false} ) ;	
	layers.push_back( {"ThinGlass"             , 0.700*mm , glassMaterial        , false} ) ;	
	layers.push_back( {"GasGap"                , 1.200*mm , gasGapMaterial       , true } ) ; //we want to keep trace of this later
	layers.push_back( {"ThickGlass"            , 1.100*mm , glassMaterial        , false} ) ;
	layers.push_back( {"GraphiteCathode"       , 0.050*mm , graphiteMaterial     , false} ) ;	
	layers.push_back( {"MylarCathode"          , 0.015*mm , mylarMaterial        , false} ) ;
	layers.push_back( {"Scintillator"          , 3.000*mm , scintillatorMaterial , false} ) ;
	layers.push_back( {"BackCassetteAbsorber"  , 2.500*mm , absorberMaterial     , false} ) ;

	//compute total RPC cassette width
	G4double cassetteThickness = 0.0 ;
	for ( const auto& layer : layers )
		cassetteThickness += layer.width ;

	sizeZ = cassetteThickness ;

	//create logic RPC volume (indefined placement)
	G4Box* solidCassette = new G4Box(name , sizeX/2, sizeY/2, cassetteThickness/2) ;
	G4LogicalVolume* logicCassette = new G4LogicalVolume(solidCassette ,  defaultMaterial , name ) ;

	G4LogicalVolume* logicGap = nullptr ;

	//construct all the layers 
	G4double zPos = -cassetteThickness/2 ; //we start at front of the cassette
	for ( const auto& layer : layers )
	{
		//create logic layer volume (indefined placement)
		auto solid = new G4Box(layer.name , sizeX/2 , sizeY/2 , layer.width/2) ;
		auto logic = new G4LogicalVolume(solid , layer.material , layer.name) ;

		zPos += layer.width/2 ; //we are now at center of the current layer (where it has to be placed)

		//place the layer at zPos
		G4VPhysicalVolume* volume = new G4PVPlacement(0 , G4ThreeVector(0,0,zPos) , logic , layer.name, logicCassette , false , 0 , true) ;	//logicCassette is the mother volume

		zPos += layer.width/2 ; //we are now at the back of the current layer

		if ( layer.isGasGap ) //we keep trace of the gas gap
		{
			logicGap = logic ;
			physiGasGap = volume ;
		}
	}

	assert ( logicGap ) ; //if we don't have the gas gap we have a problem

	std::stringstream sensName ; sensName << "RPC" << id ;
	sensitiveDetector = new SDHCALRPCSensitiveDetector(sensName.str() , this ) ;
	G4SDManager::GetSDMpointer()->AddNewDetector(sensitiveDetector) ;
	logicGap->SetSensitiveDetector(sensitiveDetector) ;

	logicRPC = logicCassette ;
}


void SDHCALRPCWithScintillator::getMaterials()
{
	SDHCALRPC::getMaterials() ;

	G4NistManager* man = G4NistManager::Instance() ;
	scintillatorMaterial = man->FindOrBuildMaterial("G4_POLYSTYRENE") ;
}


