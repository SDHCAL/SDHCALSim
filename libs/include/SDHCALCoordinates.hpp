#pragma once

#include "G4AffineTransform.hh"
#include "G4ThreeVector.hh"

#include <vector>

class SDHCALCoordinates 
{
public:
  SDHCALCoordinates()=default;
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
  const G4ThreeVector IJToLocalCoord(const G4int& I ,const G4int& J) const;
  const std::vector<int> localCoordToIJ(const G4ThreeVector& localCoord) const;
  const G4ThreeVector getGlobalCoord(const G4int& I ,const G4int& J) const;
  inline G4bool isTransformComputed() const 
  { 
    return m_TransformComputed;
  }
  void setCoordTransform(const G4AffineTransform& trans);
  inline const G4AffineTransform& getGlobalToRpcTransform() const 
  { 
    return m_GlobalToRPCTransform;
  }
  const G4ThreeVector globalToRPCCoordTransform(const G4ThreeVector& globalCoord) const ;
  const G4ThreeVector RPCToGlobalCoordTransform(const G4ThreeVector& localCoord) const ;
  G4double getCellSizeX() {return m_CellSizeX;}
  G4double getCellSizeY() {return m_CellSizeY;}
private:
  G4double m_SizeX{0*CLHEP::mm};
  G4double m_SizeY{0*CLHEP::mm};
  G4double m_SizeZ{0*CLHEP::mm};
  G4double m_CellSizeX{0*CLHEP::mm};
  G4double m_CellSizeY{0*CLHEP::mm};
  G4bool m_TransformComputed{false};
  G4AffineTransform m_RPCToGlobalTransform{};
  G4AffineTransform m_GlobalToRPCTransform{};
};
