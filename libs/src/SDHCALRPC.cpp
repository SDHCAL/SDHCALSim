#include "SDHCALRPC.hpp"

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

#include "SDHCALRPCSensitiveDetector.hpp"


SDHCALRPC* SDHCALRPC::buildStandardRPC(G4int _id , G4int _nPadX , G4int _nPadY , G4double _cellSize)
{
	SDHCALRPCGeom geom ;
	geom.nPadX = _nPadX ;
	geom.nPadY = _nPadY ;
	geom.cellSize = _cellSize ;

	geom.layers = {} ;
	geom.layers.push_back( {"FrontCassetteAbsorber" , 2.500 , "SDHCAL_Steel316L" } ) ;
	geom.layers.push_back( {"Mask"                  , 1.600 , "SDHCAL_epoxy"     } ) ;	
	geom.layers.push_back( {"PCB"                   , 1.200 , "SDHCAL_g10"       } ) ;	
	geom.layers.push_back( {"MylarAnode"            , 0.050 , "G4_MYLAR"         } ) ;	
	geom.layers.push_back( {"GraphiteAnode"         , 0.050 , "G4_GRAPHITE"      } ) ;	
	geom.layers.push_back( {"ThinGlass"             , 0.700 , "G4_Pyrex_Glass"   } ) ;	
	geom.layers.push_back( {"GasGap"                , 1.200 , "SDHCAL_RPCGaz"    } ) ; //the layer called GasGap has a special Role
	geom.layers.push_back( {"ThickGlass"            , 1.100 , "G4_Pyrex_Glass"   } ) ;
	geom.layers.push_back( {"GraphiteCathode"       , 0.050 , "G4_GRAPHITE"      } ) ;	
	geom.layers.push_back( {"MylarCathode"          , 0.180 , "G4_MYLAR"         } ) ;
	geom.layers.push_back( {"BackCassetteAbsorber"  , 2.500 , "SDHCAL_Steel316L" } ) ;

	return new SDHCALRPC(_id , geom) ;
}

SDHCALRPC* SDHCALRPC::buildOldStandardRPC(G4int _id , G4int _nPadX , G4int _nPadY , G4double _cellSize)
{
	SDHCALRPCGeom geom ;
	geom.nPadX = _nPadX ;
	geom.nPadY = _nPadY ;
	geom.cellSize = _cellSize ;

	geom.layers = {} ;
	geom.layers.push_back( {"FrontCassetteAbsorber" , 2.500 , "SDHCAL_Steel316L_Old" } ) ;
	geom.layers.push_back( {"Mask"                  , 1.600 , "G4_Galactic"          } ) ;	
	geom.layers.push_back( {"PCB"                   , 1.200 , "G4_Galactic"          } ) ;	
	geom.layers.push_back( {"MylarAnode"            , 0.050 , "G4_MYLAR"             } ) ;	
	geom.layers.push_back( {"GraphiteAnode"         , 0.050 , "G4_GRAPHITE"          } ) ;	
	geom.layers.push_back( {"ThinGlass"             , 0.700 , "G4_Pyrex_Glass"       } ) ;	
	geom.layers.push_back( {"GasGap"                , 1.200 , "SDHCAL_RPCGaz"        } ) ; //the layer called GasGap has a special Role
	geom.layers.push_back( {"ThickGlass"            , 1.100 , "G4_Pyrex_Glass"       } ) ;
	geom.layers.push_back( {"GraphiteCathode"       , 0.050 , "G4_GRAPHITE"          } ) ;	
	geom.layers.push_back( {"MylarCathode"          , 0.180 , "G4_MYLAR"             } ) ;
	geom.layers.push_back( {"BackCassetteAbsorber"  , 2.500 , "SDHCAL_Steel316L_Old" } ) ;

	return new SDHCALRPC(_id , geom) ;
}

SDHCALRPC* SDHCALRPC::buildWithScintillatorRPC(G4int _id , G4int _nPadX , G4int _nPadY , G4double _cellSize)
{
	SDHCALRPCGeom geom ;
	geom.nPadX = _nPadX ;
	geom.nPadY = _nPadY ;
	geom.cellSize = _cellSize ;

	geom.layers = {} ;
	geom.layers.push_back( {"FrontCassetteAbsorber" , 2.500 , "SDHCAL_Steel316L" } ) ;
	geom.layers.push_back( {"MylarAnode"            , 0.015 , "G4_MYLAR"         } ) ;	
	geom.layers.push_back( {"GraphiteAnode"         , 0.050 , "G4_GRAPHITE"      } ) ;	
	geom.layers.push_back( {"ThinGlass"             , 0.700 , "G4_Pyrex_Glass"   } ) ;	
	geom.layers.push_back( {"GasGap"                , 1.200 , "SDHCAL_RPCGaz"    } ) ; //the layer called GasGap has a special Role
	geom.layers.push_back( {"ThickGlass"            , 1.100 , "G4_Pyrex_Glass"   } ) ;
	geom.layers.push_back( {"GraphiteCathode"       , 0.050 , "G4_GRAPHITE"      } ) ;	
	geom.layers.push_back( {"MylarCathode"          , 0.015 , "G4_MYLAR"         } ) ;
	geom.layers.push_back( {"Scintillator"          , 3.000 , "G4_POLYSTYRENE"   } ) ;
	geom.layers.push_back( {"BackCassetteAbsorber"  , 2.500 , "SDHCAL_Steel316L" } ) ;

	return new SDHCALRPC(_id , geom) ;
}

SDHCALRPC::SDHCALRPC(G4int _id , const SDHCALRPCGeom& _geom)
{
	id = _id ;

	G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance() ;
	std::stringstream sname ; sname << "Cassette" << id ;
	name = sname.str() ;
	if ( store->GetVolume(name , false) )
	{
		G4cerr << "WARNING : RPC with ID = " << id << " already existing" << G4endl ;
		std::terminate() ;
	}

	build(_geom) ;
}

void SDHCALRPC::build(const SDHCALRPCGeom& _geom)
{
	using CLHEP::mm ;
	G4NistManager* man = G4NistManager::Instance() ;

	auto defaultMaterial = man->FindOrBuildMaterial("G4_Galactic") ;

	nPadX = _geom.nPadX ;
	nPadY = _geom.nPadY ;
	cellSize = _geom.cellSize ;

	sizeX = nPadX*cellSize ;
	sizeY = nPadY*cellSize ;

 	//layers of the RPC ordered from front to back
	const auto& layers = _geom.layers ;

	//compute total RPC cassette width
	G4double cassetteThickness = 0.0 ;
	for ( const auto& layer : layers )
		cassetteThickness += layer.width*mm ;

	sizeZ = cassetteThickness ;

	//create logic RPC volume (indefined placement)
	G4Box* solidCassette = new G4Box(name , sizeX/2, sizeY/2, cassetteThickness/2) ;
	G4LogicalVolume* logicCassette = new G4LogicalVolume(solidCassette ,  defaultMaterial , name ) ;

	G4LogicalVolume* logicGap = nullptr ;

	//construct all the layers 
	G4double zPos = -cassetteThickness/2 ; //we start at front of the cassette
	for ( const auto& layer : layers )
	{
		auto material = man->FindOrBuildMaterial(layer.material) ;
		if ( !material )
		{
			G4cerr << "Error : material " << layer.material << " not known" << G4endl ;
			std::terminate() ;
		}

		//create logic layer volume (indefined placement)
		auto solid = new G4Box(layer.name , sizeX/2 , sizeY/2 , layer.width*mm/2) ;
		auto logic = new G4LogicalVolume(solid , material , layer.name) ;

		zPos += layer.width*mm/2 ; //we are now at center of the current layer (where it has to be placed)

		//place the layer at zPos
		G4VPhysicalVolume* volume = new G4PVPlacement(nullptr , G4ThreeVector(0,0,zPos) , logic , layer.name , logicCassette , false , 0 , true) ;	//logicCassette is the mother volume

		zPos += layer.width*mm/2 ; //we are now at the back of the current layer

		if ( layer.name == "GasGap" ) //we keep trace of the gas gap
		{
			if ( logicGap ) //we only want one gas gap
			{
				G4cerr << "Error : there is more than one gas gap in the RPC" << G4endl ;
				std::terminate() ;
			}

			logicGap = logic ;
			physiGasGap = volume ;			
		}
	}

	if ( !logicGap ) //if we don't have the gas gap we have a problem
	{
		G4cerr << "Error : no gas gap in the RPC" << G4endl ;
		std::terminate() ;
	}

	std::stringstream sensName ; sensName << "RPC" << id ;
	sensitiveDetector = new SDHCALRPCSensitiveDetector(sensName.str() , this) ;
	G4SDManager::GetSDMpointer()->AddNewDetector(sensitiveDetector) ;
	logicGap->SetSensitiveDetector(sensitiveDetector) ;

	logicRPC = logicCassette ;
}

G4VPhysicalVolume* SDHCALRPC::createPhysicalVolume(G4RotationMatrix* rot , G4ThreeVector trans , G4LogicalVolume* motherLogic)
{
	physicRPC = new G4PVPlacement(rot , trans , logicRPC , name , motherLogic , false , 0 , true) ;
	return physicRPC ;
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
