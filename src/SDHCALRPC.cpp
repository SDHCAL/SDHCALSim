#include "SDHCALRPC.h"

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

std::set<SDHCALRPC*> SDHCALRPC::allTheRPC ;

SDHCALRPC::SDHCALRPC(G4int _id , G4int _nPadX , G4int _nPadY, G4double _cellSize , bool old)
{
	oldConfig = old ;
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

	SDHCALRPC::getMaterials() ;
	SDHCALRPC::build() ;

	allTheRPC.insert(this) ;
}
SDHCALRPC::~SDHCALRPC()
{
	std::set<SDHCALRPC*>::iterator it = allTheRPC.find(this) ;
	allTheRPC.erase(it) ;
}

void SDHCALRPC::build()
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
	layers.push_back( {"FrontCassetteAbsorber" , 2.500*mm , absorberMaterial , false} ) ;
	layers.push_back( {"Mask"                  , 1.600*mm , maskMaterial     , false} ) ;	
	layers.push_back( {"PCB"                   , 1.200*mm , PCBMaterial      , false} ) ;	
	layers.push_back( {"MylarAnode"            , 0.050*mm , mylarMaterial    , false} ) ;	
	layers.push_back( {"GraphiteAnode"         , 0.050*mm , graphiteMaterial , false} ) ;	
	layers.push_back( {"ThinGlass"             , 0.700*mm , glassMaterial    , false} ) ;	
	layers.push_back( {"GasGap"                , 1.200*mm , gasGapMaterial   , true } ) ; //we want to keep trace of this layer
	layers.push_back( {"ThickGlass"            , 1.100*mm , glassMaterial    , false} ) ;
	layers.push_back( {"GraphiteCathode"       , 0.050*mm , graphiteMaterial , false} ) ;	
	layers.push_back( {"MylarCathode"          , 0.180*mm , mylarMaterial    , false} ) ;
	layers.push_back( {"BackCassetteAbsorber"  , 2.500*mm , absorberMaterial , false} ) ;

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

G4VPhysicalVolume* SDHCALRPC::createPhysicalVolume(G4RotationMatrix* rot , G4ThreeVector trans , G4LogicalVolume* motherLogic)
{
	physicRPC = new G4PVPlacement(rot , trans , logicRPC , name , motherLogic , false , 0 , true) ;
	return physicRPC ;
}


void SDHCALRPC::getMaterials()
{
	G4NistManager* man = G4NistManager::Instance() ;

	defaultMaterial =  man->FindOrBuildMaterial("G4_Galactic") ;

	absorberMaterial = G4Material::GetMaterial("SDHCAL_Steel316L" , true) ;
	maskMaterial = G4Material::GetMaterial("SDHCAL_epoxy" , true) ;
	PCBMaterial = G4Material::GetMaterial("SDHCAL_g10" , true) ;

	if ( oldConfig ) //to reproduce old results
	{
		absorberMaterial = G4Material::GetMaterial("SDHCAL_Steel316L_Old" , true) ;
		maskMaterial = G4Material::GetMaterial("G4_Galactic" , true) ;
		PCBMaterial = G4Material::GetMaterial("G4_Galactic" , true) ;
	}

	mylarMaterial = man->FindOrBuildMaterial("G4_MYLAR") ;
	graphiteMaterial = man->FindOrBuildMaterial("G4_GRAPHITE") ;
	glassMaterial = man->FindOrBuildMaterial("G4_Pyrex_Glass") ;
	gasGapMaterial = G4Material::GetMaterial("SDHCAL_RPCGaz" , true) ;
}

void SDHCALRPC::setCoordTransform(G4AffineTransform trans)
{
	globalToRpcTransform = trans ;
	rpcToGlobalTransform = globalToRpcTransform.Inverse() ;
	transformComputed = true ;
}

const G4ThreeVector SDHCALRPC::globalToRpcCoordTransform(G4ThreeVector globalCoord) const
{
	return globalToRpcTransform.TransformPoint(globalCoord) ;
}
const G4ThreeVector SDHCALRPC::rpcToGlobalCoordTransform(G4ThreeVector localCoord) const
{
	return rpcToGlobalTransform.TransformPoint(localCoord) ;
}
std::vector<int> SDHCALRPC::localCoordToIJ(G4ThreeVector localCoord) const
{
	localCoord += G4ThreeVector(0.5*sizeX , 0.5*sizeY ,0) ;

	std::vector<int> ij ;
	ij.push_back( static_cast<G4int>( localCoord.x() / cellSize ) ) ;
	ij.push_back( static_cast<G4int>( localCoord.y() / cellSize ) ) ;

	return ij ;
}

const G4ThreeVector SDHCALRPC::IJToLocalCoord(G4int I , G4int J) const
{
	return G4ThreeVector( -0.5*sizeX + (I+0.5)*cellSize , -0.5*sizeY + (J+0.5)*cellSize , 0) ;
}


G4ThreeVector SDHCALRPC::getGlobalCoord(G4int I , G4int J) const
{
	G4ThreeVector localCoord( -0.5*sizeX + (I+0.5)*cellSize , -0.5*sizeY + (J+0.5)*cellSize , 0) ;
	return rpcToGlobalTransform.TransformPoint(localCoord) ;
}
