#ifndef LyonDetectorConstruction_H
#define LyonDetectorConstruction_H 1

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4Box;
class LyonDetectorFieldSetup;

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class LyonDetectorConstruction : public G4VUserDetectorConstruction
{
public:

  LyonDetectorConstruction();
  ~LyonDetectorConstruction();
  
  G4VPhysicalVolume* Construct();
   
private:
  LyonDetectorFieldSetup* fEmFieldSetup;    
  
 
};

#endif
