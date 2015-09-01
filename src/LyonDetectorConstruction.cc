#include <fstream>
#include <iostream>

#include "LyonDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "globals.hh"
#include "G4SDManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include "G4VisAttributes.hh"

#include "LyonSensitiveDetector.hh"
#include "LyonDetectorFieldSetup.hh"

using namespace std;

#define PI 3.1415926
//#define MAGNETIC_FIELD
LyonDetectorConstruction::LyonDetectorConstruction()
{ 
  fEmFieldSetup = new LyonDetectorFieldSetup();
}

LyonDetectorConstruction::~LyonDetectorConstruction()
{
  delete fEmFieldSetup;
}

G4VPhysicalVolume* LyonDetectorConstruction::Construct()
{
#ifdef MAGNETIC_FIELD
  G4MagneticField* magField = new G4UniformMagField(G4ThreeVector(0.0,3.0*tesla,0.0));
  G4FieldManager* globalFieldManager=G4TransportationManager::GetTransportationManager()->GetFieldManager();
  globalFieldManager->SetDetectorField(magField);
  globalFieldManager->CreateChordFinder(magField);
#endif
  
  //------------------------------------------------------ materials
  //Get standard stuff from NIST database :
  G4NistManager *man=G4NistManager::Instance();
  G4Material *Air=man->FindOrBuildMaterial("G4_AIR");
  G4Material *Vacuum=man->FindOrBuildMaterial("G4_Galactic");
  G4Material *Fer=man->FindOrBuildMaterial("G4_Fe");
  G4Material *Cr=man->FindOrBuildMaterial("G4_Cr");
  G4Material *Ni=man->FindOrBuildMaterial("G4_Ni");
  G4Material *Pb=man->FindOrBuildMaterial("G4_Pb");
  G4Material *Al=man->FindOrBuildMaterial("G4_Al");

  //G4Material *Carbone=man->FindOrBuildMaterial("G4_C");

  G4double steeldensity=7.87*CLHEP::g/CLHEP::cm3;
  G4double fractionMassFe=0.70611;
  G4double fractionMassCr=0.18784;
  G4double fractionMassNi=0.10605;
  //G4cout << "POUR ACIER : PROPORTION DE FER = " << fractionMassFer << G4endl;
  //density=fractionMassFer*Fer->GetDensity()+(1-fractionMassFer)*Carbone->GetDensity();
  G4Material *Steel=new G4Material("Steel",steeldensity,3);
  Steel->AddMaterial(Fer,fractionMassFe);
  Steel->AddMaterial(Cr,fractionMassCr);
  Steel->AddMaterial(Ni,fractionMassNi);
  //G4Material *Steel=Fer;

 
  G4Material *glass   =man->FindOrBuildMaterial("G4_Pyrex_Glass");
  G4Material *mylar   =man->FindOrBuildMaterial("G4_MYLAR");
  G4Material *graphite=man->FindOrBuildMaterial("G4_GRAPHITE");
  G4Material *tungsten=man->FindOrBuildMaterial("G4_W");
  G4Material *silicon =man->FindOrBuildMaterial("G4_W");
  G4Material *polystyren =man->FindOrBuildMaterial("G4_POLYSTYRENE");

  //Definition du g10
  double a=1.01*CLHEP::g/CLHEP::mole;
  G4Element *elH=new G4Element("Hydrogen","H",1,a);

  a=12.01*CLHEP::g/CLHEP::mole;
  G4Element *elC=new G4Element("Carbon","C",6,a);

  a=16.00*CLHEP::g/CLHEP::mole;
  G4Element *elO=new G4Element("Oxygen","O",8,a);

  a=32.06*CLHEP::g/CLHEP::mole;
  G4Element *elS=new G4Element("Sulfur","S",16,a);

  a=19.00*CLHEP::g/CLHEP::mole;
  G4Element *elF=new G4Element("Fluor","F",9,a);


  G4double  epoxydensity=1.3*CLHEP::g/CLHEP::cm3;
  G4Material *epoxy=new G4Material("epoxy",epoxydensity,3);
  epoxy->AddElement(elC,15);
  epoxy->AddElement(elO,7);
  epoxy->AddElement(elH,44);

  G4Material *Si02=man->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  G4Material *Cl  =man->FindOrBuildMaterial("G4_Cl");


  G4double  g10density=1.7*CLHEP::g/CLHEP::cm3;
  G4Material *g10=new G4Material("g10",g10density,3);
  g10->AddMaterial(epoxy,14.7*CLHEP::perCent);
  g10->AddMaterial(Cl,8*CLHEP::perCent);
  g10->AddMaterial(Si02,77.3*CLHEP::perCent);


  //Definition du gaz
  //Le SF6:
  G4double  SF6density=6.27e-3*CLHEP::g/CLHEP::cm3;
  G4Material *SF6=new G4Material("SF6",SF6density,2);
  SF6->AddElement(elS,1);
  SF6->AddElement(elF,6);

  //Isobutane:
  G4double  Isobutanedensity=2.51e-3*CLHEP::g/CLHEP::cm3;
  G4Material *Isobutane=new G4Material("Isobutane",Isobutanedensity,2);
  Isobutane->AddElement(elC,4);
  Isobutane->AddElement(elH,10);

  //CO2
  G4double  CO2density=1.799e-3*CLHEP::g/CLHEP::cm3;
  G4Material *CO2=new G4Material("CO2",CO2density,2);
  CO2->AddElement(elC,1);
  CO2->AddElement(elO,2);

  //TFE:
  G4double  TFEdensity=4.25e-3*CLHEP::g/CLHEP::cm3;
  G4Material *TFE=new G4Material("TFE",TFEdensity,3);
  TFE->AddElement(elC,2);
  TFE->AddElement(elH,2);
  TFE->AddElement(elF,4);

  //RPCGaz
  G4double  RPCGazdensity=4.13e-3*CLHEP::g/CLHEP::cm3;
  G4Material *RPCGaz=new G4Material("RPCGaz",RPCGazdensity,3);
  
  //Modified by rhan, the conversion between V and M is M=22.4V*m,the volume Proportional of gaz: TFE:94.5 ,SF6 0.5,
  //  RPCGaz->AddMaterial(TFE,96.37*CLHEP::perCent);
  // RPCGaz->AddMaterial(Isobutane,2.899*CLHEP::perCent);
  //RPCGaz->AddMaterial(SF6,0.729*CLHEP::perCent);

  //for co2gas mixer. V=93%TEF,%5CO2,%2SF6
  //  RPCGaz->AddMaterial(TFE,95.64*CLHEP::perCent);
  //RPCGaz->AddMaterial(CO2,1.412*CLHEP::perCent);
  //RPCGaz->AddMaterial(SF6,2.944*CLHEP::perCent);

  
  RPCGaz->AddMaterial(TFE,93*CLHEP::perCent);
  RPCGaz->AddMaterial(Isobutane,5*CLHEP::perCent);
  RPCGaz->AddMaterial(SF6,2*CLHEP::perCent);
  


  G4Material *defaultMaterial=Vacuum;
  G4Material *AbsorberMaterial=Al;
  G4Material *AdditionalAbsorberLayerMaterial=Al;
  G4Material *GapMaterial=RPCGaz;//polystyren;

  G4cout <<"" << G4endl;   G4cout <<"" << G4endl; 
  G4cout <<"-----------------------------------" << G4endl; 
  G4cout <<"Printing table of materials" << G4endl; 
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
  G4cout <<"-----------------------------------" << G4endl; 


  //------------------------------ beam line along z axis

  //Geometry parameters
  //by rhan
  //G4int  NbOfLayers=48;
  G4int NbOfLayers=1;  // GG : a greater modification will be needed to put more than 1 layer
  G4double AbsorberThickness            =2.5*CLHEP::mm; //M=
  //G4double AbsorberThickness            =   0*CLHEP::cm; //M=
  //G4double AdditionalAirThickness       =0.5*CLHEP::cm;
  //G4double AdditionalAluThickness       =0.3*CLHEP::cm;
  
  G4double RPC_Thickness                =6.131*CLHEP::mm;//by rhan from 6.00 to 6.031
  G4double RPC_PCB_Thickness            =1.200*CLHEP::mm;//by rhan from 0.8 to 1.2
  G4double RPC_mylar_ThicknessAnode     =0.050*CLHEP::mm;
  G4double RPC_mylar_ThicknessCathode   =0.180*CLHEP::mm;//by rhan from 0.2 to 0.18
  G4double RPC_Graphite_ThicknessAnode  =0.050*CLHEP::mm;
  G4double RPC_Graphite_ThicknessCathode=0.050*CLHEP::mm;//by rhan from 0.1 to 0.05
  G4double RPC_ThinGlass                =0.700*CLHEP::mm;
  G4double RPC_Gap_Thickness            =1.200*CLHEP::mm;
  G4double RPC_ThickGlass               =1.100*CLHEP::mm;
  //G4double RPC_EdgeWidth                =3.000*CLHEP::mm;
  G4double RPC_ChipPackageThickness     =1.600*CLHEP::mm;//by rhan from 1.40 to 1.60
  G4double RPC_Free_Thickness=RPC_Thickness-RPC_ChipPackageThickness-RPC_PCB_Thickness-RPC_mylar_ThicknessAnode-RPC_mylar_ThicknessCathode-RPC_Graphite_ThicknessAnode-RPC_Graphite_ThicknessCathode-RPC_ThinGlass-RPC_ThickGlass-RPC_Gap_Thickness;


  //G4double LayerThickness   = AbsorberThickness +2*AdditionalAirThickness+ RPC_Thickness+AdditionalAluThickness;
  G4double LayerThickness   = AbsorberThickness+ RPC_Thickness;
  G4double CalorThickness   =NbOfLayers*LayerThickness;
  //  G4double CalorSizeX=342.5*mm;
  //  G4double CalorSizeY=90.5*mm;
  G4double CalorSizeX=1008.0*CLHEP::mm; //(96*1pads+96*0.05gaps)by rhan from 1010.5 to 1008
  G4double CalorSizeY=1008.0*CLHEP::mm;
  //G4double CalorSizeX=1000.0*mm; //(96*1pads+96*0.05gaps)by rhan from 1010.5 to 1008
  //G4double CalorSizeY=1000.0*mm;


  //Absorber in front of RPC
  G4double AdditionalAbsorberLayerThickness=2.5*CLHEP::mm;  
  G4double AdditionalAbsorberPos  =-CalorThickness/2-AdditionalAbsorberLayerThickness/2;
  //end of Absorber in front of RPC

  ifstream myfile ("setup_config.txt");
  if (myfile.is_open())
    {
      myfile >> AbsorberThickness ;
      myfile >> AdditionalAbsorberLayerThickness ;
    }
  myfile.close();

  //World
  G4double WorldSizeY=2.2*CalorSizeY;
  G4double WorldSizeX=2.2*CalorSizeX;
  G4double WorldSizeZ=6*(CalorThickness+AdditionalAbsorberLayerThickness);

  

  G4double AbsorberZ         =LayerThickness/2-AbsorberThickness/2;
  G4double RPCChipPackageZ   =AbsorberZ -AbsorberThickness/2 -RPC_ChipPackageThickness/2;
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //G4double RPCChipPackageZ   =-LayerThickness/2+RPC_ChipPackageThickness/2;
  G4double RPC_PCBPosZ       =RPCChipPackageZ   -RPC_ChipPackageThickness/2-RPC_PCB_Thickness/2;
  G4double RPC_mylarPosZ     =RPC_PCBPosZ       -RPC_PCB_Thickness/2       -RPC_mylar_ThicknessAnode/2;
  G4double RPC_GraphitePosZ  =RPC_mylarPosZ     -RPC_mylar_ThicknessAnode/2-RPC_Graphite_ThicknessAnode/2;
  G4double RPC_ThinGlassPosZ =RPC_GraphitePosZ  -RPC_Graphite_ThicknessAnode/2  -RPC_ThinGlass/2;
  G4double RPC_GapPosZ       =RPC_ThinGlassPosZ -RPC_ThinGlass/2           -RPC_Gap_Thickness/2;
  G4double RPC_ThickGlassPosZ=RPC_GapPosZ       -RPC_Gap_Thickness/2       -RPC_ThickGlass/2;
  G4double RPC_GraphitePosZ2 =RPC_ThickGlassPosZ-RPC_ThickGlass/2          -RPC_Graphite_ThicknessCathode/2;
  G4double RPC_mylarPosZ2    =RPC_GraphitePosZ2 -RPC_Graphite_ThicknessCathode/2  -RPC_mylar_ThicknessCathode/2;
  G4double RPC_FreePosZ      =RPC_mylarPosZ2-RPC_mylar_ThicknessCathode/2-RPC_Free_Thickness/2;
  
  ////////////////////////////////////////////////////////////A VERIFIER///////////////////////////////////////////////////////////////////////
  /*
    G4double AbsorberZ         =-LayerThickness/2+AbsorberThickness/2;
    G4double RPCChipPackageZ   =AbsorberZ +AbsorberThickness/2 +RPC_ChipPackageThickness/2;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //G4double RPCChipPackageZ   =-LayerThickness/2+RPC_ChipPackageThickness/2;
    G4double RPC_PCBPosZ       =RPCChipPackageZ   +RPC_ChipPackageThickness/2+RPC_PCB_Thickness/2;
    G4double RPC_mylarPosZ     =RPC_PCBPosZ       +RPC_PCB_Thickness/2       +RPC_mylar_ThicknessAnode/2;
    G4double RPC_GraphitePosZ  =RPC_mylarPosZ     +RPC_mylar_ThicknessAnode/2+RPC_Graphite_ThicknessAnode/2;
    G4double RPC_ThinGlassPosZ =RPC_GraphitePosZ  +RPC_Graphite_ThicknessAnode/2  +RPC_ThinGlass/2;
    G4double RPC_GapPosZ       =RPC_ThinGlassPosZ +RPC_ThinGlass/2           +RPC_Gap_Thickness/2;
    G4double RPC_ThickGlassPosZ=RPC_GapPosZ       +RPC_Gap_Thickness/2       +RPC_ThickGlass/2;
    G4double RPC_GraphitePosZ2 =RPC_ThickGlassPosZ+RPC_ThickGlass/2          +RPC_Graphite_ThicknessCathode/2;
    G4double RPC_mylarPosZ2    =RPC_GraphitePosZ2 +RPC_Graphite_ThicknessCathode/2  +RPC_mylar_ThicknessCathode/2;
    G4double RPC_FreePosZ      =RPC_mylarPosZ2+RPC_mylar_ThicknessCathode/2+RPC_Free_Thickness/2;
  */
  //G4double AdditionalAluZ    =RPC_FreePosZ+RPC_Free_Thickness/2+AdditionalAluThickness/2;

  //G4double AdditionalAirZ    =LayerThickness/2-AdditionalAirThickness/2;
  //G4double AbsorberZ         =-AbsorberThickness/2;//***********************************************************????????????????????????????
  //G4double AbsorberZ         =AdditionalAirZ-AdditionalAirThickness/2-AbsorberThickness/2;
  //G4double AdditionalAirZ0   =AbsorberZ-AdditionalAirThickness/2-AbsorberThickness/2;

  G4cout << "EL/ -----------------------------------" << G4endl;
  G4cout << "EL/ Cross check on geometry" << G4endl;
  G4cout << "LayerThickness        =" <<LayerThickness/CLHEP::cm  << " cm" << G4endl;
  G4cout << "CalorThickness        =" <<CalorThickness/CLHEP::cm  << " cm" << G4endl;

  //   G4cout << "AbsorberZ        =" <<AbsorberZ/mm  << G4endl;
  //   G4cout << "RPCChipPackageZ  =" <<RPCChipPackageZ/mm  << G4endl;
  //   G4cout << "RPC_PCBPosZ      =" <<RPC_PCBPosZ/mm  << G4endl;
  //   G4cout << "RPC_GraphitePosZ =" <<RPC_GraphitePosZ/mm  << G4endl;
  //   G4cout << "RPC_ThinGlassPosZ=" <<RPC_ThinGlassPosZ/mm << G4endl;
  G4cout << "EL/ -----------------------------------" << G4endl;


  G4cout <<"" << G4endl;  G4cout <<"" << G4endl; 
  G4cout <<"EL/ -----------------------------------" << G4endl; 
  G4cout <<"EL/ Using Test beam configuration at PS" << G4endl; 
  G4cout <<"EL/ with parameters:" << G4endl; 
  G4cout <<"EL/ Default    :" << defaultMaterial ->GetName() << G4endl; 
  G4cout <<"EL/ Absorber   :" << AbsorberMaterial->GetName() << G4endl; 
  G4cout <<"EL/ GapMaterial:" << GapMaterial     ->GetName() << G4endl; 
  G4cout <<"EL/ Size of absorber in front of RPC:" << AdditionalAbsorberLayerThickness/CLHEP::cm << " cm" <<G4endl; 
  G4cout <<"EL/ -----------------------------------" << G4endl; 
  G4cout <<"" << G4endl;  G4cout <<"" << G4endl; 

 

  //-------------------------------------------------------------------------
  // Detector geometry
  //-------------------------------------------------------------------------

  //     
  // World
  //
  G4Box *solidWorld = new G4Box("World",				   //its name
				WorldSizeX/2,WorldSizeY/2,WorldSizeZ/2);  //its size
                         
  G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld,		//its solid
						    defaultMaterial,	//its material
						    "World");		//its name
                                   
  G4VPhysicalVolume* physiWorld = new G4PVPlacement(0,			//no rotation
						    G4ThreeVector(),	//at (0,0,0)
						    logicWorld,		//its logical volume   
						    "World",		//its name
						    0,			//its mother  volume
						    false,		//no boolean operation
						    0,true);			//copy number



  //
  //To put additional absorber in front of RPC's
  //
  
  if (AdditionalAbsorberLayerThickness>0){
    G4Box *solidAdditionalAbsorber = new G4Box("Additional Absorber solid",		//its name
					       CalorSizeX/2,CalorSizeY/2,AdditionalAbsorberLayerThickness/2);//size
    
    
    G4LogicalVolume *logicAdditionalAbsorber = new G4LogicalVolume(solidAdditionalAbsorber,	//its solid
								   AdditionalAbsorberLayerMaterial,//Air,//AbsorberMaterial,	//its material
								   "Additional Absorber logic");	//its name
    
    G4VPhysicalVolume *physiAdditionalAbsorber = new G4PVPlacement(0,			//no rotation
								   G4ThreeVector(0,0,AdditionalAbsorberPos),	//at (0,0,0)
								   logicAdditionalAbsorber,	        //its logical volume
								   "Additional Absorber physic",	//its name
								   logicWorld,	        //its mother  volume
								   false,		//no boolean operation
								   0,true);		        //copy number
    
    
    G4VisAttributes* AdditionalAbsorberAttributes = new G4VisAttributes(G4Colour::Red());
    //Attributes->SetForceSolid(true);
    logicAdditionalAbsorber->SetVisAttributes(AdditionalAbsorberAttributes);
  }



  // Around Calorimeter
  //  
  
  //   G4double safety1=12*CLHEP::cm;
  //   G4double safety2=2*CLHEP::cm;

  //   G4Box *solidAroundCalor1 = new G4Box("AroundCalorimeter1",		//its name
  // 				       CalorSizeX/2+safety1,CalorSizeY/2+safety1,CalorThickness/2);//size

  //   G4Box *solidAroundCalor2 = new G4Box("AroundCalorimeter2",		//its name
  // 				       CalorSizeX/2+safety2,CalorSizeY/2+safety2,CalorThickness/2);//size

  //   G4VSolid *solidAroundCalo=new G4SubtractionSolid("AroundCalo",solidAroundCalor1,solidAroundCalor2,0,G4ThreeVector(0,0,0)); 
  
  //   G4LogicalVolume *logicAroundCalo = new G4LogicalVolume(solidAroundCalo,    //its solid
  // 							 GapMaterial , //its material
  // 							 "AroundCalo"); //name
 
  //   G4VPhysicalVolume *physiAroundCalo = new G4PVPlacement(0,		   //no rotation
  // 							 G4ThreeVector(0.,0.,0.),  //its position
  // 							 logicAroundCalo,     //its logical volume		    
  // 							 "AroundCalo", //its name
  // 							 logicWorld,        //its mother
  // 							 false,             //no boulean operat
  // 							 0,true);                //copy number
  



  /*
    G4double tubThickness=10*CLHEP::cm;
    G4double InnerRadius=WorldSizeX/2-tubThickness-10*CLHEP::cm;//parameter "r"
    G4double OuterRadius=InnerRadius+tubThickness;//parameter "r' "


    G4VSolid *solidTube=new G4Tubs("tube",InnerRadius,OuterRadius,WorldSizeZ/2,0,2*PI);

    G4LogicalVolume *logicTube = new G4LogicalVolume(solidTube,    //its solid
    GapMaterial, //its material
    "Tube"); //name

    G4VPhysicalVolume *physiTube = new G4PVPlacement(0,		   //no rotation
    G4ThreeVector(0.,0.,0.),  //its position
    logicTube,     //its logical volume		    
    "Tube", //its name
    logicWorld,        //its mother
    false,             //no boulean operat
    0);                //copy number
  */



  //                               
  // Calorimeter
  //  
  
  G4Box *solidCalor = new G4Box("Calorimeter",		//its name
				CalorSizeX/2,CalorSizeY/2,CalorThickness/2);//size
 
   			     
  G4LogicalVolume *logicCalor = new G4LogicalVolume(solidCalor,	//its solid
						    defaultMaterial,	//its material
						    "Calorimeter");	//its name
    				       
  new G4PVPlacement(0,			//no rotation
		    G4ThreeVector(),	//at (0,0,0)
		    logicCalor,	        //its logical volume
		    "Calorimeter",	//its name
		    logicWorld,	        //its mother  volume
		    false,		//no boolean operation
		    0,true);		        //copy number
  
  //
  // Calorimeter layer
  //
  
  G4Box *solidLayer = new G4Box("Layer",			//its name
				CalorSizeX/2,CalorSizeY/2,LayerThickness/2); //size
                       
  G4LogicalVolume *logicLayer = new G4LogicalVolume(solidLayer,	     //its solid
						    defaultMaterial, //its material
						    "Layer");	     //its name
      
  new G4PVReplica("Layer",	    //its name
		  logicLayer,	    //its logical volume
		  logicCalor,	    //its mother
		  kZAxis,	    //axis of replication
		  NbOfLayers,	    //number of replica
		  LayerThickness); //witdth of replica
  


  //Additional layer of air
  ////////////////////////:::::::::::::::::://////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /*G4Box *solidAdditionalAir0 = new G4Box("AdditionalAir0",		//its name
    CalorSizeX/2,CalorSizeY/2,AdditionalAirThickness/2); 
                          
    G4LogicalVolume *logicAdditionalAir0 = new G4LogicalVolume(solidAdditionalAir0,    //its solid
    Air, //its material
    "AdditionalAir0"); //name
      			                  
    G4VPhysicalVolume *physiAdditionalAir0 = new G4PVPlacement(0,		   //no rotation
    G4ThreeVector(0.,0.,AdditionalAirZ0),  //its position
    logicAdditionalAir0,     //its logical volume		    
    "AdditionalAir0", //its name
    logicLayer,        //its mother
    false,             //no boulean operat
    0,true);                //copy number


  */
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  
  //                               
  // Absorber
  //
  G4Box *solidAbsorber = NULL;
  G4LogicalVolume *logicAbsorber=NULL;
  //G4VPhysicalVolume *physiAbsorber=NULL;
  if (AbsorberThickness > 0 ) {
    solidAbsorber = new G4Box("Absorber",		//its name
			      CalorSizeX/2,CalorSizeY/2,AbsorberThickness/2); 
    
    logicAbsorber = new G4LogicalVolume(solidAbsorber,    //its solid
					AbsorberMaterial, //its material
					"AbsorberPlate"); //name
      			                  
    new G4PVPlacement(0,		   //no rotation
		      G4ThreeVector(0.,0.,AbsorberZ),  //its position
		      logicAbsorber,     //its logical volume		    
		      "AbsorberPlate", //its name
		      logicLayer,        //its mother
		      false,             //no boulean operat
		      0,true);                //copy number
  }



  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /*
  //Additional layer of air

  G4Box *solidAdditionalAir = new G4Box("AdditionalAir",		//its name
  CalorSizeX/2,CalorSizeY/2,AdditionalAirThickness/2); 
                          
  G4LogicalVolume *logicAdditionalAir = new G4LogicalVolume(solidAdditionalAir,    //its solid
  Air, //its material
  "AdditionalAir"); //name
      			                  
  G4VPhysicalVolume *physiAdditionalAir = new G4PVPlacement(0,		   //no rotation
  G4ThreeVector(0.,0.,AdditionalAirZ),  //its position
  logicAdditionalAir,     //its logical volume		    
  "AdditionalAir", //its name
  logicLayer,        //its mother
  false,             //no boulean operat
  0,true);                //copy number


  //
  */
  
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  
  
  
  ////***********************************************************************************************************
  
  
  //Chip+epoxy=electronics
  //


  G4Box *solidRPCElectronics = new G4Box("solidRPCElectronics",		//its name
					 CalorSizeX/2,CalorSizeY/2,RPC_ChipPackageThickness/2);//size
  
  
  G4LogicalVolume *logicRPCElectronics = new G4LogicalVolume(solidRPCElectronics,    //its solid
							     defaultMaterial , //its material
							     "logicRPC"); //name
  
  new G4PVPlacement(0,		   //no rotation
		    G4ThreeVector(0,0,RPCChipPackageZ),  //its position
		    logicRPCElectronics,     //its logical volume		    
		    "physiRPCElectronics", //its name
		    logicLayer,        //its mother
		    false,             //no boulean operat
		    0,true);                //copy number
  

  //                                
  //PCB
  //
  G4Box *solidRPCPCB = new G4Box("solidRPCPCB",		//its name
				 CalorSizeX/2,CalorSizeY/2,RPC_PCB_Thickness/2);//size

  
  G4LogicalVolume *logicRPCPCB = new G4LogicalVolume(solidRPCPCB,    //its solid
						     defaultMaterial , //its material
						     "logicRPCPCB"); //name
 
  new G4PVPlacement(0,		   //no rotation
		    G4ThreeVector(0,0,RPC_PCBPosZ),  //its position
		    logicRPCPCB,     //its logical volume		    
		    "physiRPCPCB", //its name
		    logicLayer,        //its mother
		    false,             //no boulean operat
		    0,true);                //copy number


  //                               
  //mylar
  //
  G4Box *solidRPCmylarAnode = new G4Box("solidRPCmylarAnode",		//its name
					CalorSizeX/2,CalorSizeY/2,RPC_mylar_ThicknessAnode/2);//size

  
  G4LogicalVolume *logicRPCmylarAnode = new G4LogicalVolume(solidRPCmylarAnode,    //its solid
							    mylar , //its material
							    "logicRPCmylarAnode"); //name
 
  new G4PVPlacement(0,		   //no rotation
		    G4ThreeVector(0,0,RPC_mylarPosZ),  //its position
		    logicRPCmylarAnode,     //its logical volume		    
		    "physiRPCmylar", //its name
		    logicLayer,        //its mother
		    false,             //no boulean operat
		    0,true);                //copy number


  G4Box *solidRPCmylarCathode = new G4Box("solidRPCmylarCathode",		//its name
					  CalorSizeX/2,CalorSizeY/2,RPC_mylar_ThicknessCathode/2);//size

  
  G4LogicalVolume *logicRPCmylarCathode = new G4LogicalVolume(solidRPCmylarCathode,    //its solid
							      mylar, //its material
							      "logicRPCmylarCathode"); //name
  
  new G4PVPlacement(0,		   //no rotation
		    G4ThreeVector(0,0,RPC_mylarPosZ2),  //its position
		    logicRPCmylarCathode,     //its logical volume		    
		    "physiRPCmylarCathode", //its name
		    logicLayer,        //its mother
		    false,             //no boulean operat
		    0,true);                //copy number


  
  //                               
  //Graphite
  //
  G4Box *solidRPCGraphiteAnode = new G4Box("solidRPCGraphiteAnode",		//its name
					   CalorSizeX/2,CalorSizeY/2,RPC_Graphite_ThicknessAnode/2);//size

  
  G4LogicalVolume *logicRPCGraphiteAnode = new G4LogicalVolume(solidRPCGraphiteAnode,    //its solid
							       graphite, //its material
							       "logicRPCGraphiteAnode"); //name
  
  new G4PVPlacement(0,		   //no rotation
		    G4ThreeVector(0,0,RPC_GraphitePosZ),  //its position
		    logicRPCGraphiteAnode,     //its logical volume		    
		    "physiRPCGraphiteAnode", //its name
		    logicLayer,        //its mother
		    false,             //no boulean operat
		    0,true);                //copy number
  
  G4Box *solidRPCGraphiteCathode = new G4Box("solidRPCGraphiteCathode",		//its name
					     CalorSizeX/2,CalorSizeY/2,RPC_Graphite_ThicknessCathode/2);//size
 
  G4LogicalVolume *logicRPCGraphiteCathode = new G4LogicalVolume(solidRPCGraphiteCathode,    //its solid
								 graphite, //its material
								 "logicRPCGraphiteCathode"); //name
 
  new G4PVPlacement(0,		   //no rotation
		    G4ThreeVector(0,0,RPC_GraphitePosZ2),  //its position
		    logicRPCGraphiteCathode,     //its logical volume		    
		    "physiRPCGraphiteCathode", //its name
		    logicLayer,        //its mother
		    false,             //no boulean operat
		    0,true);                //copy number





  //
  //Glass
  //
  G4Box *solidRPCThinGlass = new G4Box("solidRPCThinGlass",		//its name
				       CalorSizeX/2,CalorSizeY/2,RPC_ThinGlass/2);//size

  
  G4LogicalVolume *logicRPCThinGlass = new G4LogicalVolume(solidRPCThinGlass,    //its solid
							   glass, //its material
							   "logicRPCThinGlass"); //name
 
  new G4PVPlacement(0,		   //no rotation
		    G4ThreeVector(0,0,RPC_ThinGlassPosZ),  //its position
		    logicRPCThinGlass,     //its logical volume		    
		    "physiRPCThinGlass", //its name
		    logicLayer,        //its mother
		    false,             //no boulean operat
		    0,true);                //copy number



  G4Box *solidRPCThickGlass = new G4Box("solidRPCThickGlass",		//its name
					CalorSizeX/2,CalorSizeY/2,RPC_ThickGlass/2);//size

  
  G4LogicalVolume *logicRPCThickGlass = new G4LogicalVolume(solidRPCThickGlass,    //its solid
							    glass, //its material
							    "logicRPCThickGlass"); //name
 
  new G4PVPlacement(0,		   //no rotation
		    G4ThreeVector(0,0,RPC_ThickGlassPosZ),  //its position
		    logicRPCThickGlass,     //its logical volume		    
		    "physiRPCThickGlass", //its name
		    logicLayer,        //its mother
		    false,             //no boulean operat
		    0,true);                //copy number




  
  //                                 
  // Gap
  //

  G4Box *solidGap = new G4Box("Gap",
			      CalorSizeX/2,CalorSizeY/2,RPC_Gap_Thickness/2);
    			   
  G4LogicalVolume *logicGap = new G4LogicalVolume(solidGap,
						  GapMaterial,
						  "GapSpace");
  G4bool allLocal = true ;
       
  logicGap->SetFieldManager( fEmFieldSetup->GetLocalFieldManager(), 
			     allLocal ) ;
      				     
  new G4PVPlacement(0,                      //no rotation
		    G4ThreeVector(0.,0.,RPC_GapPosZ),   //its position
		    logicGap,               //its logical volume	       
		    "GapSpace",             //its name
		    logicLayer,             //its mother
		    false,                  //no boulean operat
		    0,true);                     //copy number




  //
  //Left space
  //
  G4Box *solidRPCFree = new G4Box("solidRPCFree",		//its name
				  CalorSizeX/2,CalorSizeY/2,RPC_Free_Thickness/2);//size

  
  G4LogicalVolume *logicRPCFree = new G4LogicalVolume(solidRPCFree,    //its solid
						      defaultMaterial , //its material
						      "logicRPCFree"); //name
 
  new G4PVPlacement(0,		   //no rotation
		    G4ThreeVector(0,0,RPC_FreePosZ),  //its position
		    logicRPCFree,     //its logical volume		    
		    "physiRPCFree", //its name
		    logicLayer,        //its mother
		    false,             //no boulean operat
		    0,true);                //copy number

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 
  //Additional layer of air

  /*G4Box *solidAdditionalAlu = new G4Box("AdditionalAlu",		//its name
    CalorSizeX/2,CalorSizeY/2,AdditionalAluThickness/2); 
                          
    G4LogicalVolume *logicAdditionalAlu = new G4LogicalVolume(solidAdditionalAlu,    //its solid
    Al, //its material
    "AdditionalAl"); //name
      			                  
    G4VPhysicalVolume *physiAdditionalAlu = new G4PVPlacement(0,		   //no rotation
    G4ThreeVector(0.,0.,AdditionalAluZ),  //its position
    logicAdditionalAlu,     //its logical volume		    
    "AdditionalAlu", //its name
    logicLayer,        //its mother
    false,             //no boulean operat
    0,true);                //copy number




  */

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //                                        
  // Visualization attributes
  //
  logicWorld->SetVisAttributes (G4VisAttributes::Invisible);

  G4VisAttributes* absorberAttributes = new G4VisAttributes(G4Colour::Yellow());
  absorberAttributes->SetForceSolid(true);
  if(logicAbsorber)logicAbsorber->SetVisAttributes(absorberAttributes);

  /*G4VisAttributes* AdditionalAirAttributes= new G4VisAttributes(G4Colour::Cyan());
    AdditionalAirAttributes->SetForceSolid(true);
    logicAdditionalAir->SetVisAttributes(AdditionalAirAttributes);
    logicAdditionalAir0->SetVisAttributes(AdditionalAirAttributes);

    G4VisAttributes* AdditionalAluAttributes= new G4VisAttributes(G4Colour::Grey());
    logicAdditionalAlu->SetVisAttributes(AdditionalAluAttributes);
 
  */
  
  
  G4VisAttributes* gapAttributes = new G4VisAttributes(G4Colour::Magenta());
  //gapAttributes->SetForceSolid(true);
  logicGap->SetVisAttributes(gapAttributes);

  //G4VisAttributes* logicTubeAttributes=new G4VisAttributes(G4Colour::Red());
  //logicTubeAttributes->SetForceSolid(true);

  logicRPCElectronics->SetVisAttributes (G4Colour::Green());
  //logicRPCElectronics->SetForceSolid(true);
  logicRPCPCB->SetVisAttributes (G4Colour::Green());
  logicRPCmylarAnode->SetVisAttributes (G4Colour::Red());
  logicRPCmylarCathode->SetVisAttributes (G4Colour::Red());
  logicRPCGraphiteAnode->SetVisAttributes (G4Colour::Green());
  logicRPCGraphiteCathode->SetVisAttributes (G4Colour::Green());
  logicRPCThinGlass->SetVisAttributes (G4Colour::Grey());
  logicRPCThickGlass->SetVisAttributes (G4Colour::Grey());
  logicRPCFree->SetVisAttributes (G4VisAttributes::Invisible);
  //logicRPCPCB->SetForceSolid(true);
  //logicRPCmylarAnode->SetForceSolid(true);
  //logicRPCmylarCathode->SetForceSolid(true);
  //logicRPCGraphiteAnode->SetForceSolid(true);
  //logicRPCGraphiteCathode->SetForceSolid(true);
  //logicRPCThinGlass->SetForceSolid(true);
  //logicRPCThickGlass->SetForceSolid(true);
  

  logicCalor->SetVisAttributes(G4VisAttributes::Invisible);

  //Scorers
  LyonSensitiveDetector* det = new LyonSensitiveDetector("BeamTest",1); //1 car le layer (volume parent) est replique.
  //il faut rendre le detector sensitive pour que le scorer fasse quelque chose
  G4SDManager::GetSDMpointer()->AddNewDetector(det);
  //logicGap->SetSensitiveDetector(det); //Gap soi même  
  logicGap->SetSensitiveDetector(det);
  //logicTube->SetSensitiveDetector(det);
  //logicAroundCalo->SetSensitiveDetector(det);
  //
  //always return the physical World
  //
  return physiWorld;
}
