#ifndef SDHCALDetectorConstruction_h
#define SDHCALDetectorConstruction_h

#include <G4VUserDetectorConstruction.hh>
#include <SDHCALSteppingAction.h>

enum RPCType
{
	kNormalRPC,
	kWithScintillatorRPC
} ;

class SDHCALDetectorConstruction : public G4VUserDetectorConstruction
{
	public :
		SDHCALDetectorConstruction(RPCType _rpcType = kNormalRPC) ;
		~SDHCALDetectorConstruction() ;

		G4VPhysicalVolume* Construct() ;

		static G4double sizeX ;
		static G4double sizeZ ;

	protected :

		RPCType rpcType ;


} ;


#endif //SDHCALDetectorConstruction_h
