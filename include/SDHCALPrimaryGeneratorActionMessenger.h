#ifndef SDHCALPrimaryGeneratorActionMessenger_h
#define SDHCALPrimaryGeneratorActionMessenger_h

#include <G4UImessenger.hh>

class SDHCALPrimaryGeneratorAction ;
class G4UIdirectory ;
class G4UIcmdWithADouble ;
class G4UIcmdWithAString ;
class G4UIcmdWith3Vector ;

class SDHCALPrimaryGeneratorActionMessenger : public G4UImessenger
{
	public :
		SDHCALPrimaryGeneratorActionMessenger(SDHCALPrimaryGeneratorAction* gen) ;
		virtual ~SDHCALPrimaryGeneratorActionMessenger() ;

		void SetNewValue(G4UIcommand* command, G4String value) ;


	protected :

		SDHCALPrimaryGeneratorAction* generator ;

		G4UIdirectory* dir ;

		G4UIcmdWithAString* particleDefinitionCmd ;

		G4UIcmdWithAString* posOptionCmd ;
		G4UIcmdWithADouble* posXCmd ;
		G4UIcmdWithADouble* posYCmd ;
		G4UIcmdWithADouble* posZCmd ;
		G4UIcmdWithADouble* uniformMaxPosCmd ;
		G4UIcmdWithADouble* sigmaPosCmd ;

		G4UIcmdWithAString* momOptionCmd ;
		G4UIcmdWithADouble* momPhiCmd ;
		G4UIcmdWithADouble* momThetaCmd ;
		G4UIcmdWithADouble* momSigmaCmd ; //gaussian sigma value for gaussian gun momentum option


		G4UIcmdWithAString* energyOptionCmd ;
		G4UIcmdWithADouble* energyCmd ;
		G4UIcmdWithADouble* sigmaEnergyCmd ;
		G4UIcmdWithADouble* minEnergyCmd ;
		G4UIcmdWithADouble* maxEnergyCmd ;
} ;

#endif //SDHCALPrimaryGeneratorActionMessenger_h
