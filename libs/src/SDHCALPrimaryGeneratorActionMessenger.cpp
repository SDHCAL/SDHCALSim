#include "SDHCALPrimaryGeneratorActionMessenger.h"
#include "SDHCALPrimaryGeneratorAction.h"

#include <G4UIdirectory.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWith3Vector.hh>
#include <globals.hh>


SDHCALPrimaryGeneratorActionMessenger::SDHCALPrimaryGeneratorActionMessenger(SDHCALPrimaryGeneratorAction* gen)
{
	generator = gen ;

	dir = new G4UIdirectory("/GunParameter/") ;


	particleDefinitionCmd = new G4UIcmdWithAString("/GunParameter/Particle" , this) ;
	particleDefinitionCmd->SetGuidance("Particle type") ;
	particleDefinitionCmd->SetParameterName("Particle" , true) ;
	particleDefinitionCmd->SetDefaultValue("mu-") ;



	posOptionCmd = new G4UIcmdWithAString("/GunParameter/PositionOption" , this) ;
	posOptionCmd->SetGuidance("Set primary gun option position") ;
	posOptionCmd->SetParameterName("PositionOption" , true) ;
	posOptionCmd->SetCandidates("fixed uniform gaus cosmic") ;
	posOptionCmd->SetDefaultValue("fixed") ;

	posXCmd = new G4UIcmdWithADouble("/GunParameter/PositionX" , this) ;
	posXCmd->SetGuidance("Set X Position") ;
	posXCmd->SetParameterName("PositionX" , true) ;
	posXCmd->SetDefaultValue(0) ;

	posYCmd = new G4UIcmdWithADouble("/GunParameter/PositionY" , this) ;
	posYCmd->SetGuidance("Set Y Position") ;
	posYCmd->SetParameterName("PositionY" , true) ;
	posYCmd->SetDefaultValue(0) ;

	posZCmd = new G4UIcmdWithADouble("/GunParameter/PositionZ" , this) ;
	posZCmd->SetGuidance("Set Z Position") ;
	posZCmd->SetParameterName("PositionZ" , true) ;
	posZCmd->SetDefaultValue(-20) ;

	uniformMaxPosCmd = new G4UIcmdWithADouble("/GunParameter/UniformDeltaPosition" , this) ;
	uniformMaxPosCmd->SetGuidance("Delta position for uniform distribution (in mm)") ;
	uniformMaxPosCmd->SetParameterName("UniformDeltaPosition" , true) ;
	uniformMaxPosCmd->SetDefaultValue(10) ;

	sigmaPosCmd = new G4UIcmdWithADouble("/GunParameter/GausSigmaPosition" , this) ;
	sigmaPosCmd->SetGuidance("Sigma position for gaussian distribution (in mm)") ;
	sigmaPosCmd->SetParameterName("GausSigmaPosition" , true) ;
	sigmaPosCmd->SetDefaultValue(10) ;



	momOptionCmd = new G4UIcmdWithAString("/GunParameter/MomentumOption" , this) ;
	momOptionCmd->SetGuidance("Set primary gun momentum position") ;
	momOptionCmd->SetParameterName("MomentumOption" , true) ;
	momOptionCmd->SetCandidates("fixed gaus") ;
	momOptionCmd->SetDefaultValue("fixed") ;

	momPhiCmd = new G4UIcmdWithADouble("/GunParameter/MomentumPhi" , this) ;
	momPhiCmd->SetGuidance("Phi angle of momentum vector") ;
	momPhiCmd->SetParameterName("MomentumPhi" , true) ;
	momPhiCmd->SetDefaultValue(0) ;

	momThetaCmd = new G4UIcmdWithADouble("/GunParameter/MomentumTheta" , this) ;
	momThetaCmd->SetGuidance("Theta angle of momentum vector") ;
	momThetaCmd->SetParameterName("MomentumTheta" , true) ;
	momThetaCmd->SetDefaultValue(0) ;

	momSigmaCmd = new G4UIcmdWithADouble("/GunParameter/MomentumSigma" , this) ;
	momSigmaCmd->SetGuidance("Sigma momentum for gaussian distribution") ;
	momSigmaCmd->SetParameterName("MomentumSigma" , true) ;
	momSigmaCmd->SetDefaultValue(0.1) ;



	energyOptionCmd = new G4UIcmdWithAString("/GunParameter/EnergyDistribution" , this) ;
	energyOptionCmd->SetGuidance("Set energy distribution") ;
	energyOptionCmd->SetParameterName("EnergyOption" , true) ;
	energyOptionCmd->SetCandidates("fixed uniform gaus forNN") ;
	energyOptionCmd->SetDefaultValue("fixed") ;

	energyCmd = new G4UIcmdWithADouble("/GunParameter/Energy",this) ;
	energyCmd->SetGuidance("Set particle energy (in GeV)") ;
	energyCmd->SetParameterName("Energy",true) ;
	energyCmd->SetDefaultValue(50) ;

	sigmaEnergyCmd = new G4UIcmdWithADouble("/GunParameter/SigmaEnergy" , this) ;
	sigmaEnergyCmd->SetGuidance("Energy sigma for gaussian distribution") ;
	sigmaEnergyCmd->SetParameterName("SigmaEnergy" , true) ;
	sigmaEnergyCmd->SetDefaultValue(0.5) ;

	minEnergyCmd = new G4UIcmdWithADouble("/GunParameter/MinEnergy" , this) ;
	minEnergyCmd->SetGuidance("Set min energy for uniform of forNN distribution") ;
	minEnergyCmd->SetParameterName("MinEnergy" , true) ;
	minEnergyCmd->SetDefaultValue(1) ;

	maxEnergyCmd = new G4UIcmdWithADouble("/GunParameter/MaxEnergy" , this) ;
	maxEnergyCmd->SetGuidance("Set max energy for uniform of forNN distribution") ;
	maxEnergyCmd->SetParameterName("MaxEnergy" , true) ;
	maxEnergyCmd->SetDefaultValue(120) ;
}

SDHCALPrimaryGeneratorActionMessenger::~SDHCALPrimaryGeneratorActionMessenger()
{
	delete particleDefinitionCmd ;

	delete posOptionCmd ;
	delete posXCmd ;
	delete posYCmd ;
	delete posZCmd ;
	delete uniformMaxPosCmd ;
	delete sigmaPosCmd ;

	delete momOptionCmd ;
	delete momPhiCmd ;
	delete momThetaCmd ;
	delete momSigmaCmd ;

	delete energyOptionCmd ;
	delete energyCmd ;
	delete sigmaEnergyCmd ;
	delete minEnergyCmd ;
	delete maxEnergyCmd ;

	delete dir ;
}

void SDHCALPrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command , G4String value)
{
	if ( command == particleDefinitionCmd )
		generator->setParticleDefinition(value) ;

	else if ( command == posOptionCmd )
		generator->setGunOptionPosition(value) ;
	else if ( command == posXCmd )
		generator->setMeanPositionX( posXCmd->GetNewDoubleValue(value) ) ;
	else if ( command == posYCmd )
		generator->setMeanPositionY( posYCmd->GetNewDoubleValue(value) ) ;
	else if ( command == posZCmd )
		generator->setMeanPositionZ( posZCmd->GetNewDoubleValue(value) ) ;
	else if ( command == uniformMaxPosCmd )
		generator->setUniformMaxPosition( uniformMaxPosCmd->GetNewDoubleValue(value) ) ;
	else if ( command == sigmaPosCmd )
		generator->setSigmaPosition( sigmaPosCmd->GetNewDoubleValue(value) ) ;

	else if ( command == momOptionCmd )
		generator->setGunOptionMomentum( value ) ;
	else if ( command == momPhiCmd )
		generator->SetMomentumPhi( momPhiCmd->GetNewDoubleValue(value) ) ;
	else if ( command == momThetaCmd )
		generator->SetMomentumTheta( momThetaCmd->GetNewDoubleValue(value) ) ;
	else if ( command == momSigmaCmd )
		generator->setGaussianMomentumSigma( momSigmaCmd->GetNewDoubleValue(value) ) ;

	else if ( command == energyOptionCmd )
		generator->setGunOptionEnergy( value ) ;
	else if ( command == energyCmd )
		generator->setParticleEnergy( energyCmd->GetNewDoubleValue(value) ) ;
	else if ( command == sigmaEnergyCmd )
		generator->setSigmaEnergy( sigmaEnergyCmd->GetNewDoubleValue(value) ) ;
	else if ( command == minEnergyCmd )
		generator->setMinEnergy( minEnergyCmd->GetNewDoubleValue(value) ) ;
	else if ( command == maxEnergyCmd )
		generator->setMaxEnergy( maxEnergyCmd->GetNewDoubleValue(value) ) ;
}
