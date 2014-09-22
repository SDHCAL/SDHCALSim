#ifndef VideauDetectorConstruction_H
#define VideauDetectorConstruction_H 1

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4Box;

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class VideauDetectorConstruction : public G4VUserDetectorConstruction
{
public:

  VideauDetectorConstruction();
  ~VideauDetectorConstruction();
  
  G4VPhysicalVolume* Construct();
   
private:
  
  
 
};

#endif
