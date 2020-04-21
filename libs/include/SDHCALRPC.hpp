#pragma once

#include <globals.hh>
#include <G4Material.hh>

#include <G4VPhysicalVolume.hh>
#include <G4AffineTransform.hh>

#include <G4TouchableHistory.hh>
#include <G4GRSVolume.hh>

#include <vector>
#include <set>
class G4LogicalVolume ;


struct Layer
{
  G4String m_Name{""};
  G4double m_Width{0.}; //in mm
  G4String m_Material{""};
};

class SDHCALRPC
{
public:
  void setSizes(const G4double& X,const G4double& Y, const G4double& Z)
  {
    m_SizeX=X;
    m_SizeY=Y;
    m_SizeZ=Z;
  }
  void setCellXYSize(const G4double& X,const G4double& Y)
  {
    m_CellSizeX=X;
    m_CellSizeY=Y;
  }
  // Helper functions to construct pre-defined RPCs
  void buildStandardRPC();
  void buildOldStandardRPC();
  void buildWithScintillatorRPC();

  G4LogicalVolume* getGasGap(){return m_GasGap;}
  
  SDHCALRPC(G4int _id);
  virtual ~SDHCALRPC()=default;
  
  inline G4double getSizeZ() const {return m_SizeZ;}

  G4VPhysicalVolume* createPhysicalVolume(G4RotationMatrix* rot , G4ThreeVector trans , G4LogicalVolume* motherLogic) ;
    
private:
  G4int m_ID{0};
  G4String m_Name{""};
  std::vector<Layer> m_Layers;//This vector needs one and only one layer called 'GasGap' which corresponds to the gas gap
  virtual void build();
  G4double m_CellSizeX{0.};
  G4double m_CellSizeY{0.};
  G4double m_SizeX{0.};
  G4double m_SizeY{0.};
  G4double m_SizeZ{0.};
  G4LogicalVolume* m_GasGap{nullptr};
  G4LogicalVolume* m_LogicRPC{nullptr};


};
