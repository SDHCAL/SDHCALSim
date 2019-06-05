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
	G4double CassetteAbsorber_Thickness    = 2.500*CLHEP::mm ;

//	G4double RPC_ChipPackage_Thickness     = 1.600*CLHEP::mm ;
//	G4double RPC_PCB_Thickness             = 1.200*CLHEP::mm ;
	G4double Scintillator_Thickness        = 3.000*CLHEP::mm ;
	G4double RPC_Mylar_Thickness           = 0.015*CLHEP::mm ;
	G4double RPC_Graphite_Thickness        = 0.050*CLHEP::mm ;
	G4double RPC_ThinGlass                 = 0.700*CLHEP::mm ;
	G4double RPC_Gap_Thickness             = 1.200*CLHEP::mm ;
	G4double RPC_ThickGlass                = 1.100*CLHEP::mm ;

	G4double RPC_Thickness = Scintillator_Thickness
							 + 2.0 * RPC_Mylar_Thickness
							 + 2.0 * RPC_Graphite_Thickness
							 + RPC_ThinGlass
							 + RPC_Gap_Thickness
							 + RPC_ThickGlass ;

	G4double CassetteThickness = RPC_Thickness + 2.0*CassetteAbsorber_Thickness ;
	sizeZ = CassetteThickness ;


	G4double CassetteAbsorberFrontZ    = -CassetteThickness/2         + CassetteAbsorber_Thickness/2 ;
	G4double RPC_MylarAnodePosZ        = CassetteAbsorberFrontZ       + CassetteAbsorber_Thickness/2    + RPC_Mylar_Thickness/2 ;
	G4double RPC_GraphiteAnodePosZ     = RPC_MylarAnodePosZ           + RPC_Mylar_Thickness/2           + RPC_Graphite_Thickness/2 ;
	G4double RPC_ThinGlassPosZ         = RPC_GraphiteAnodePosZ        + RPC_Graphite_Thickness/2        + RPC_ThinGlass/2 ;
	G4double RPC_GapPosZ               = RPC_ThinGlassPosZ            + RPC_ThinGlass/2                 + RPC_Gap_Thickness/2 ;
	G4double RPC_ThickGlassPosZ        = RPC_GapPosZ                  + RPC_Gap_Thickness/2             + RPC_ThickGlass/2 ;
	G4double RPC_GraphiteCathodePosZ   = RPC_ThickGlassPosZ           + RPC_ThickGlass/2                + RPC_Graphite_Thickness/2 ;
	G4double RPC_MylarCathodePosZ      = RPC_GraphiteCathodePosZ      + RPC_Graphite_Thickness/2        + RPC_Mylar_Thickness/2 ;
	G4double ScintillatorPosZ          = RPC_MylarCathodePosZ         + RPC_Mylar_Thickness/2           + Scintillator_Thickness/2 ;
	G4double CassetteAbsorberBackZ     = ScintillatorPosZ             + Scintillator_Thickness/2        + CassetteAbsorber_Thickness/2 ;


	G4Box* solidCassette = new G4Box(name , sizeX/2, sizeY/2, CassetteThickness/2) ;
	G4LogicalVolume* logicCassette = new G4LogicalVolume(solidCassette ,  defaultMaterial , name ) ;

	//Inside Cassette -------------------------------------------------------

	//mylar anode
	G4Box* solidRPCmylarAnode = new G4Box("MylarAnode" , sizeX/2 , sizeY/2 , RPC_Mylar_Thickness/2) ;
	G4LogicalVolume* logicRPCmylarAnode = new G4LogicalVolume(solidRPCmylarAnode , mylarMaterial , "MylarAnode") ;
	new G4PVPlacement(0 , G4ThreeVector(0,0,RPC_MylarAnodePosZ) , logicRPCmylarAnode , "MylarAnode" , logicCassette , false , 0 , true) ;

	//mylar cathode
	G4Box* solidRPCmylarCathode = new G4Box("MylarCathode" , sizeX/2 , sizeY/2 , RPC_Mylar_Thickness/2) ;
	G4LogicalVolume* logicRPCmylarCathode = new G4LogicalVolume(solidRPCmylarCathode , mylarMaterial , "MylarCathode") ;
	new G4PVPlacement(0 , G4ThreeVector(0,0,RPC_MylarCathodePosZ) , logicRPCmylarCathode , "MylarCathode" , logicCassette , false , 0 , true) ;

	//Graphite anode
	G4Box* solidRPCGraphiteAnode = new G4Box("GraphiteAnode" , sizeX/2 , sizeY/2 , RPC_Graphite_Thickness/2) ;
	G4LogicalVolume* logicRPCGraphiteAnode = new G4LogicalVolume(solidRPCGraphiteAnode , graphiteMaterial , "GraphiteAnode") ;
	new G4PVPlacement(0 , G4ThreeVector(0,0,RPC_GraphiteAnodePosZ) , logicRPCGraphiteAnode , "GraphiteAnode" , logicCassette , false , 0 , true) ;

	//Graphite cathode
	G4Box* solidRPCGraphiteCathode = new G4Box("GraphiteCathode" , sizeX/2 , sizeY/2 , RPC_Graphite_Thickness/2) ;
	G4LogicalVolume* logicRPCGraphiteCathode = new G4LogicalVolume(solidRPCGraphiteCathode ,  graphiteMaterial , "GraphiteCathode") ;
	new G4PVPlacement(0 , G4ThreeVector(0,0,RPC_GraphiteCathodePosZ) , logicRPCGraphiteCathode , "GraphiteCathode" ,  logicCassette , false , 0 , true) ;

	//thin glass
	G4Box* solidRPCThinGlass = new G4Box("ThinGlass" , sizeX/2 , sizeY/2 , RPC_ThinGlass/2) ;
	G4LogicalVolume* logicRPCThinGlass = new G4LogicalVolume(solidRPCThinGlass , glassMaterial , "ThinGlass") ;
	new G4PVPlacement(0 , G4ThreeVector(0,0,RPC_ThinGlassPosZ) , logicRPCThinGlass , "ThinGlass" , logicCassette , false , 0 , true) ;

	//thick glass
	G4Box* solidRPCThickGlass = new G4Box("ThickGlass" , sizeX/2 , sizeY/2 , RPC_ThickGlass/2) ;
	G4LogicalVolume* logicRPCThickGlass = new G4LogicalVolume(solidRPCThickGlass , glassMaterial , "ThickGlass") ;
	new G4PVPlacement(0 , G4ThreeVector(0,0,RPC_ThickGlassPosZ) , logicRPCThickGlass , "ThickGlass" , logicCassette , false , 0 , true) ;

	//gas gap
	G4Box* solidGap = new G4Box("GasGap" , sizeX/2 , sizeY/2 , RPC_Gap_Thickness/2) ;
	G4LogicalVolume* logicGap = new G4LogicalVolume(solidGap , gasGapMaterial , "GasGap") ;
	physiGasGap = new G4PVPlacement(0 , G4ThreeVector(0.,0.,RPC_GapPosZ) , logicGap , "GasGap" , logicCassette , false , 0 , true) ;

	//	G4bool allLocal = true ;
	//	logicGap->SetFieldManager( fEmFieldSetup->GetLocalFieldManager() , allLocal ) ;

	//scintillator
	G4Box* solidScintillator = new G4Box("Scintillator" , sizeX/2 , sizeY/2 , Scintillator_Thickness/2) ;
	G4LogicalVolume* logicScintillator = new G4LogicalVolume(solidScintillator , scintillatorMaterial , "Scintillator") ;
	new G4PVPlacement(0 , G4ThreeVector(0.,0.,ScintillatorPosZ) , logicScintillator , "Scintillator" , logicCassette , false , 0 , true) ;


	//back absorber
	G4Box* solidBackCassetteAbsorber = new G4Box("BackCassetteAbsorber" , sizeX/2 , sizeY/2 , CassetteAbsorber_Thickness/2) ;
	G4LogicalVolume* logicBackCassetteAbsorber = new G4LogicalVolume(solidBackCassetteAbsorber , absorberMaterial , "BackCassetteAbsorber") ;
	new G4PVPlacement(0 , G4ThreeVector(0,0,CassetteAbsorberBackZ) , logicBackCassetteAbsorber , "BackCassetteAbsorber" , logicCassette , false , 0 , true) ;

	//front absorber
	G4Box* solidFrontCassetteAbsorber = new G4Box("FrontCassetteAbsorber" , sizeX/2 , sizeY/2 , CassetteAbsorber_Thickness/2) ;
	G4LogicalVolume* logicFrontCassetteAbsorber = new G4LogicalVolume(solidFrontCassetteAbsorber , absorberMaterial , "FrontCassetteAbsorber") ;
	new G4PVPlacement(0 , G4ThreeVector(0,0,CassetteAbsorberFrontZ) , logicFrontCassetteAbsorber , "FrontCassetteAbsorber" , logicCassette , false , 0 , true) ;

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


