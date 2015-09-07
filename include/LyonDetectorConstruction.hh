#ifndef LyonDetectorConstruction_H
#define LyonDetectorConstruction_H 1

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4Box;
class LyonDetectorFieldSetup;

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

//to communicate with DHCalEventReader
//NB : GG : Geant4 might provide some mechanism to get this information transfered but I haven't checked
struct GeometryDataForLCIO
{
  G4double calorthickness;
  G4double layerThickness;
  G4int nbLayers;
  G4double gapThickness;
  G4double gapRelativeMiddlePositionInLayer; 
};


class LyonDetectorConstruction : public G4VUserDetectorConstruction
{
public:

  LyonDetectorConstruction();
  ~LyonDetectorConstruction();
  
  G4VPhysicalVolume* Construct();
   
  static GeometryDataForLCIO m_GeometryDataForLCIO;

private:
  LyonDetectorFieldSetup* fEmFieldSetup;    
  
 
};

#endif
