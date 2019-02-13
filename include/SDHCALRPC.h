#ifndef SDHCALRPC_h
#define SDHCALRPC_h

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

class SDHCALRPC
{
	public :
		SDHCALRPC(G4int _id , G4int _nPadX , G4int _nPadY, G4double _cellSize , bool old = false) ;
		virtual ~SDHCALRPC() ;

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



	protected :
		static std::set<SDHCALRPC*> allTheRPC ;

		SDHCALRPC() { ; } //Just for derived class

		virtual void getMaterials() ;
		virtual void build() ;

		G4bool oldConfig = false ;

		G4String name ;

		G4int id ;
		G4int nPadX ;
		G4int nPadY ;
		G4double cellSize ;
		G4double sizeX ;
		G4double sizeY ;
		G4double sizeZ ;

		G4bool transformComputed ;
		G4AffineTransform rpcToGlobalTransform ;
		G4AffineTransform globalToRpcTransform ;

		G4Material* defaultMaterial ;
		G4Material* absorberMaterial ;
		G4Material* maskMaterial ;
		G4Material* PCBMaterial ;
		G4Material* mylarMaterial ;
		G4Material* graphiteMaterial ;
		G4Material* glassMaterial ;
		G4Material* gasGapMaterial ;

		G4LogicalVolume* logicRPC ;
		G4VPhysicalVolume* physicRPC ;
		SDHCALRPCSensitiveDetector* sensitiveDetector ;

		G4VPhysicalVolume* physiGasGap ;

} ;

#endif //SDHCALRPC_h
