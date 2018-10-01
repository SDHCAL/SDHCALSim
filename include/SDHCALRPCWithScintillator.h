#ifndef SDHCALRPCWithScintillator_h
#define SDHCALRPCWithScintillator_h


#include "SDHCALRPC.h"
//Custom RPC with scintillator to test


class SDHCALRPCWithScintillator : public SDHCALRPC
{
	public :
		SDHCALRPCWithScintillator(G4int _id , G4int _nPadX , G4int _nPadY, G4double _cellSize) ;
		virtual ~SDHCALRPCWithScintillator() { ; }


	protected :
		virtual void getMaterials() ;
		virtual void build() ;

		G4Material* scintillatorMaterial ;
} ;



#endif //SDHCALRPCWithScintillator_h
