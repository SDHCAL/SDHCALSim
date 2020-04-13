#pragma once

#include <globals.hh>
#include <G4Material.hh>

#include <G4VPhysicalVolume.hh>
#include <G4AffineTransform.hh>

#include <G4TouchableHistory.hh>
#include <G4GRSVolume.hh>

#include <vector>
#include <set>
class G4LogicalVolume ;
class SDHCALRPCSensitiveDetector ;

struct SDHCALRPCGeom
{
	G4int nPadX {} ;
	G4int nPadY {} ;
	G4double cellSize {} ;
	
	struct Layer
	{
		G4String name ;
		G4double width ; //in mm
		G4String material ;
	} ;

	std::vector<Layer> layers {} ; //This vector needs one and only one layer called 'GasGap' which corresponds to the gas gap
} ;

class SDHCALRPC
{
	public : 
		// Helper functions to construct pre-defined RPCs
		static SDHCALRPC* buildStandardRPC(G4int _id , G4int _nPadX , G4int _nPadY , G4double _cellSize) ;
		static SDHCALRPC* buildOldStandardRPC(G4int _id , G4int _nPadX , G4int _nPadY , G4double _cellSize) ;
		static SDHCALRPC* buildWithScintillatorRPC(G4int _id , G4int _nPadX , G4int _nPadY , G4double _cellSize) ;

	public :
		SDHCALRPC(G4int _id , const SDHCALRPCGeom& _geom) ;
		virtual ~SDHCALRPC()=default;

		G4LogicalVolume* getLogicRPC() { return logicRPC ; }
		G4VPhysicalVolume* getPhysicRPC() { return physicRPC ; }

		inline G4int getnPadX() const { return nPadX ; }
		inline G4int getnPadY() const { return nPadY ; }
		inline G4double getCellSize() const { return cellSize ; }
		inline G4double getSizeX() const { return sizeX ; }
		inline G4double getSizeY() const { return sizeY ; }
		inline G4double getSizeZ() const { return sizeZ ; }
		inline G4int getID() const { return id ; }

		inline G4bool isTransformComputed() const { return transformComputed ; }
		void setCoordTransform(G4AffineTransform trans) ;
		inline const G4AffineTransform& getGlobalToRpcTransform() const { return globalToRpcTransform ; }


		const G4ThreeVector globalToRpcCoordTransform(G4ThreeVector globalCoord) const ;
		const G4ThreeVector rpcToGlobalCoordTransform(G4ThreeVector localCoord) const ;

		const G4ThreeVector IJToLocalCoord(G4int I , G4int J) const ;
		std::vector<int> localCoordToIJ(G4ThreeVector localCoord) const ;

		G4ThreeVector getGlobalCoord(G4int I , G4int J) const ;

		G4VPhysicalVolume* createPhysicalVolume(G4RotationMatrix* rot , G4ThreeVector trans , G4LogicalVolume* motherLogic) ;

		SDHCALRPC(const SDHCALRPC&) = delete ;
		void operator=(const SDHCALRPC&) = delete ;

private:

		virtual void build(const SDHCALRPCGeom& _geom) ;

		G4String name {} ;

		G4int id {} ;
		G4int nPadX {} ;
		G4int nPadY {} ;
		G4double cellSize {} ;
		G4double sizeX {} ;
		G4double sizeY {} ;
		G4double sizeZ {} ;

		G4bool transformComputed {} ;
		G4AffineTransform rpcToGlobalTransform {} ;
		G4AffineTransform globalToRpcTransform {} ;

		G4LogicalVolume* logicRPC {} ;
		G4VPhysicalVolume* physicRPC {} ;
		SDHCALRPCSensitiveDetector* sensitiveDetector {} ;

		G4VPhysicalVolume* physiGasGap {} ;

};
