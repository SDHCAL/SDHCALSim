#include "LyonDetectorConstruction.hh"
#include "G4PhysicalConstants.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "globals.hh"
#include "G4SDManager.hh"

#include "G4VisAttributes.hh"

#include "LyonSensitiveDetector.hh"
#include "VideauDetectorConstruction.hh"


VideauDetectorConstruction::VideauDetectorConstruction()
{;}

VideauDetectorConstruction::~VideauDetectorConstruction()
{
}

G4VPhysicalVolume* VideauDetectorConstruction::Construct()
{
  
  using namespace CLHEP;

  //------------------------------------------------------ materials

  //Get standard stuff from NIST database :
  G4NistManager *man=G4NistManager::Instance();
  G4Material *Air=man->FindOrBuildMaterial("G4_AIR");
  G4Material *Vacuum=man->FindOrBuildMaterial("G4_Galactic");
  G4Material *Fer=man->FindOrBuildMaterial("G4_Fe");
  //G4Material *Carbone=man->FindOrBuildMaterial("G4_C");


  G4Material *Steel=Fer;

  G4cout << *(G4Material::GetMaterialTable()) << G4endl;


  //------------------------------ beam line along z axis

  //Geometry parameters
  G4double InnerRadius=100*cm;
  G4double OuterRadius=200*cm;
  G4double AngleRatio=0.76536686;
  G4double InnerHexaSize=AngleRatio*InnerRadius;
  G4double OuterHexaSize=AngleRatio*OuterRadius;
  G4double BaseLength=0.5*(sqrt(2*OuterHexaSize*OuterHexaSize*(2+sqrt(2))-InnerHexaSize*InnerHexaSize*(3+2*sqrt(2)))-InnerHexaSize);
  G4double BiggestRPC=InnerHexaSize+BaseLength;

  G4int NbOfLayers=40;
  G4double AbsorberThickness=2*cm;
  G4double GapThickness=2*cm;
  G4double LayerThickness = AbsorberThickness + GapThickness;
  G4double CalorLength=100*cm;

  G4double WorldSizeZ=1.2*CalorLength;
  G4double WorldSizeY=1.2*OuterRadius*2;
  G4double WorldSizeX=1.2*OuterRadius*2;

  G4Material *defaultMaterial=Vacuum;
  G4Material *AbsorberMaterial=Steel;
  G4Material *GapMaterial=Air;
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
						    0);			//copy number




  //                               
  // Absorber
  //
  const G4double zPlane[2]={0,0}; 
  const G4double rInner[2]={1,1}; 
  const G4double rOuter[2]={0,1}; 

  G4Polyhedra *solidModule = new G4Polyhedra("Module",
					       0,
					       twopi,
					       3,		
					       2,
					       zPlane,
                                               rInner,
                                               rOuter
				        ); 
                          
  G4LogicalVolume *logicModule = new G4LogicalVolume(solidModule,    //its solid
						       AbsorberMaterial, //its material
						       "AbsorberPlate"); //name
      			                  
  G4VPhysicalVolume *physiModule = new G4PVPlacement(0,		   //no rotation
						       G4ThreeVector(0.,0.,0.),  //its position
						       logicModule,     //its logical volume		    
						       "AbsorberPlate", //its name
						       logicModule,        //its mother
						       false,             //no boulean operat
						       0);                //copy number
                                        
  
  
  //                                        
  // Visualization attributes
  //
  logicWorld->SetVisAttributes (G4VisAttributes::Invisible);

  G4VisAttributes* absorberAttributes = new G4VisAttributes(G4Colour::Yellow());
  //absorberAttributes->SetForceSolid(true);
  logicModule->SetVisAttributes(absorberAttributes);
 
  /* 
  G4VisAttributes* gapAttributes = new G4VisAttributes(G4Colour::Red());
  gapAttributes->SetForceSolid(true);
  logicGap->SetVisAttributes(gapAttributes);

  //Scorers
  VideauSensitiveDetector* det = new VideauSensitiveDetector("BeamTest",1); //1 car le layer (volume parent) est replique.
  //il faut rendre le detector sensitive pour que le scorer fasse quelque chose
  G4SDManager::GetSDMpointer()->AddNewDetector(det);
  //logicGap->SetSensitiveDetector(det); //Gap soi même  
  logicAbsorber->SetSensitiveDetector(det);
  */

  //
  //always return the physical World
  //
  return physiWorld;
}
