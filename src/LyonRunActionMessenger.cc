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

#include "LyonRunActionMessenger.hh"
#include "LyonRunAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
double _Seuil,_Distance,_ChardisWidth,padsize;
//double _Seuil1,_Seuil2,_Seuil3,_energy;
int halfNPad;
LyonRunActionMessenger::LyonRunActionMessenger(LyonRunAction* Run)
  :runsetup(Run)
{
  runDir = new G4UIdirectory("/LyonLcio/");
  runDir->SetGuidance("event control");

  ThresholdCmd = new G4UIcmdWithADouble("/LyonLcio/threshold",this);
  ThresholdCmd->SetGuidance("Set threshold for LCIO file");
  ThresholdCmd->SetParameterName("seuil",true);
  ThresholdCmd->SetDefaultValue(1.6);					
  ThresholdCmd->SetRange("seuil>0");

  /*
  ThresholdCmd1 = new G4UIcmdWithADouble("/LyonLcio/threshold1",this);
  ThresholdCmd1->SetGuidance("Set threshold1 for LCIO file");
  ThresholdCmd1->SetParameterName("seuil1",true);
  ThresholdCmd1->SetDefaultValue(1.6);					
  ThresholdCmd1->SetRange("seuil1>0");

  ThresholdCmd2 = new G4UIcmdWithADouble("/LyonLcio/threshold2",this);
  ThresholdCmd2->SetGuidance("Set threshold2 for LCIO file");
  ThresholdCmd2->SetParameterName("seuil2",true);
  ThresholdCmd2->SetDefaultValue(1.6);					
  ThresholdCmd2->SetRange("seuil2>0");

  ThresholdCmd3 = new G4UIcmdWithADouble("/LyonLcio/threshold3",this);
  ThresholdCmd3->SetGuidance("Set threshold3 for LCIO file");
  ThresholdCmd3->SetParameterName("seuil3",true);
  ThresholdCmd3->SetDefaultValue(1.6);					
  ThresholdCmd3->SetRange("seuil3>0");
  */
  DistanceCmd = new G4UIcmdWithADouble("/LyonLcio/distance",this);
  DistanceCmd->SetGuidance("Set distance for PAD Multi");
  DistanceCmd->SetParameterName("PadMulti",true);
  DistanceCmd->SetDefaultValue(0.25);					
  DistanceCmd->SetRange("PadMulti>0&&PadMulti<1.0");
  /*
  EnergyCmd =  new G4UIcmdWithADouble("/LyonLcio/energyofparticle",this);
  EnergyCmd->SetGuidance("Set energy for particle");
  EnergyCmd->SetParameterName("energy",true);
  EnergyCmd->SetDefaultValue(10.);
  EnergyCmd->SetRange("energy>0.");
  */
  CharDisWidthCmd = new G4UIcmdWithADouble("/LyonLcio/chardiswidth",this);
  CharDisWidthCmd->SetGuidance("Set width for charge distribution");
  CharDisWidthCmd->SetParameterName("cdw",true);
  CharDisWidthCmd->SetDefaultValue(4.0);	
  CharDisWidthCmd->SetRange("cdw>0");

  halfNPadCmd = new G4UIcmdWithAnInteger("/LyonLcio/halfNPad",this);
  halfNPadCmd->SetGuidance("Set the pad number in x or y");
  halfNPadCmd->SetParameterName("halfNPad",true);
  halfNPadCmd->SetDefaultValue(8);
					
  halfNPadCmd->SetRange("halfNPad<9");

  padsizeCmd = new G4UIcmdWithADouble("/LyonLcio/padsize",this);
  padsizeCmd->SetGuidance("the pad's size+gap's size");
  padsizeCmd->SetParameterName("padsize",true);
  padsizeCmd->SetDefaultValue(1.05);					
  // padsizeCmd->SetRange("halfNPad<8");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LyonRunActionMessenger::~LyonRunActionMessenger()
{
  delete ThresholdCmd;
  /*
  delete ThresholdCmd1;
  delete ThresholdCmd2;
  delete ThresholdCmd3;
  delete EnergyCmd;
  */
  delete DistanceCmd;
  delete CharDisWidthCmd;
  delete runDir;   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LyonRunActionMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
  if(command == ThresholdCmd)
    {runsetup->SetThreshold(ThresholdCmd->GetNewDoubleValue(newValue));
     
      _Seuil=ThresholdCmd->GetNewDoubleValue(newValue);
      
    }
  /*
  if(command == ThresholdCmd1)
    {runsetup->SetThreshold(ThresholdCmd1->GetNewDoubleValue(newValue));
     
      _Seuil1=ThresholdCmd1->GetNewDoubleValue(newValue);
      
    }

  if(command == ThresholdCmd2)
    {runsetup->SetThreshold(ThresholdCmd2->GetNewDoubleValue(newValue));
     
      _Seuil2=ThresholdCmd2->GetNewDoubleValue(newValue);
      
    }

  if(command == ThresholdCmd3)
    {runsetup->SetThreshold(ThresholdCmd3->GetNewDoubleValue(newValue));
     
      _Seuil3=ThresholdCmd3->GetNewDoubleValue(newValue);
      
    }


  if(command == EnergyCmd)_energy=EnergyCmd->GetNewDoubleValue(newValue);
  */

  if(command == DistanceCmd)
    {
      //runsetup->SetThreshold(ThresholdCmd->GetNewDoubleValue(newValue));
     
      _Distance=DistanceCmd->GetNewDoubleValue(newValue);
      
    }

  if(command == halfNPadCmd)halfNPad=halfNPadCmd->GetNewIntValue(newValue);
  if(command == padsizeCmd)padsize=padsizeCmd->GetNewDoubleValue(newValue);
    
  if(command == CharDisWidthCmd)_ChardisWidth=CharDisWidthCmd->GetNewDoubleValue(newValue);
}

//anais//



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
