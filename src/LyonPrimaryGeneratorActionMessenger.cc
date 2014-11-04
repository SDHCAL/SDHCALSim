//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: ExN03EventActionMessenger.cc,v 1.12 2006/10/26 14:28:00 allison Exp $
// GEANT4 tag $Name: geant4-09-02 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "LyonPrimaryGeneratorActionMessenger.hh"
#include "LyonPrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//double _Seuil,_Distance,_ChardisWidth,padsize;
//double _Seuil1,_Seuil2,_Seuil3,_energy;
//int halfNPad;
LyonPrimaryGeneratorActionMessenger::LyonPrimaryGeneratorActionMessenger(LyonPrimaryGeneratorAction* PrimaryGenerator)
  :primaryGenerator(PrimaryGenerator)
{
  runDir = new G4UIdirectory("/GunParameter/");
  //  runDir->SetGuidance("event control");

  gunOptionPositionCmd =  new G4UIcmdWithAString("/GunParameter/gunOptionPosition",this);
  gunOptionPositionCmd->SetGuidance("Set primary gun option position");
  gunOptionPositionCmd->SetParameterName("gunOptionPosition",true);
  gunOptionPositionCmd->SetCandidates("default random");					

  gunOptionMomentumCmd =  new G4UIcmdWithAString("/GunParameter/gunOptionMomentum",this);
  gunOptionMomentumCmd->SetGuidance("Set primary gun option momentum");
  gunOptionMomentumCmd->SetParameterName("gunOptionMomentum",true);
  gunOptionMomentumCmd->SetCandidates("normal solidAngle gaus uniform");					

  randPosMaxCmd = new G4UIcmdWithADouble("/GunParameter/randomMaxPosition",this);
  randPosMaxCmd->SetGuidance("Set max position parameter for random gun");
  randPosMaxCmd->SetParameterName("randMaxPos",true);
  randPosMaxCmd->SetDefaultValue(0.4);					

  solidAngleX0Cmd = new G4UIcmdWithADouble("/GunParameter/solidAngleX0",this);
  solidAngleX0Cmd->SetGuidance("Set X0 parameter for solid angle gun momentum");
  solidAngleX0Cmd->SetParameterName("solidAngleX0",true);
  solidAngleX0Cmd->SetDefaultValue(2.0);					

  solidAngleRadCmd = new G4UIcmdWithADouble("/GunParameter/solidAngleRad",this);
  solidAngleRadCmd->SetGuidance("Set diameter for solid angle gun momentum");
  solidAngleRadCmd->SetParameterName("solidAngleRad",true);
  solidAngleRadCmd->SetDefaultValue(0.5);	
  //  solidAngleRadCmd->SetRange("cdw>0");

  gaussianMeanCmd = new G4UIcmdWithADouble("/GunParameter/gaussianMean",this);
  gaussianMeanCmd->SetGuidance("Set gaussian mean value for gaussian gun momentum");
  gaussianMeanCmd->SetParameterName("gaussianMean",true);
  gaussianMeanCmd->SetDefaultValue(0.0);
  
  gaussianSigmaCmd = new G4UIcmdWithADouble("/GunParameter/gaussianSigma",this);
  gaussianSigmaCmd->SetGuidance("Set gaussian sigma value for gaussian gun momentum");
  gaussianSigmaCmd->SetParameterName("gaussianSigma",true);
  gaussianSigmaCmd->SetDefaultValue(0.1);

  uniformParameterCmd = new G4UIcmdWithADouble("/GunParameter/uniformParameter",this);
  uniformParameterCmd->SetGuidance("Set uniform paramter value for uniform gun momentum");
  uniformParameterCmd->SetParameterName("uniformParameter",true);
  uniformParameterCmd->SetDefaultValue(0.1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LyonPrimaryGeneratorActionMessenger::~LyonPrimaryGeneratorActionMessenger()
{
  delete gunOptionPositionCmd;
  delete gunOptionMomentumCmd;
  
  delete randPosMaxCmd;
  delete solidAngleX0Cmd;
  delete solidAngleRadCmd;
  delete gaussianMeanCmd;
  delete gaussianSigmaCmd;
  
  delete runDir;   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void LyonPrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ 
  if( command == gunOptionPositionCmd )
    primaryGenerator->setGunOptionPosition(newValue);
  if( command == gunOptionMomentumCmd )
    primaryGenerator->setGunOptionMomentum(newValue);
  
  if(command == randPosMaxCmd)
    primaryGenerator->setRandomPosMax(randPosMaxCmd->GetNewDoubleValue(newValue));
  
  if(command == solidAngleX0Cmd)
    primaryGenerator->setSolidAngleX0(solidAngleX0Cmd->GetNewDoubleValue(newValue));
  if(command == solidAngleRadCmd)
    primaryGenerator->setSolidAngleRad(solidAngleRadCmd->GetNewDoubleValue(newValue));
  
  if(command == gaussianMeanCmd)
    primaryGenerator->setGaussianMean(gaussianMeanCmd->GetNewDoubleValue(newValue));
  if(command == gaussianSigmaCmd)
    primaryGenerator->setGaussianSigma(randPosMaxCmd->GetNewDoubleValue(newValue));
  

  if(command == uniformParameterCmd)
    primaryGenerator->setUniformParameter(randPosMaxCmd->GetNewDoubleValue(newValue));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
