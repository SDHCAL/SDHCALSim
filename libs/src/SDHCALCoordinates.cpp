#include "SDHCALCoordinates.hpp"

const G4ThreeVector SDHCALCoordinates::IJToLocalCoord(const G4int& I ,const G4int& J) const
{
 return G4ThreeVector(-0.5*m_SizeX + (I+0.5)*m_CellSizeX , -0.5*m_SizeY + (J+0.5)*m_CellSizeY, 0) ;
}

const std::vector<int> SDHCALCoordinates::localCoordToIJ(const G4ThreeVector& localCoord) const
{
  G4ThreeVector localCoord2= localCoord+G4ThreeVector(0.5*m_SizeX , 0.5*m_SizeY ,0);
  return std::vector<int>{static_cast<G4int>(localCoord2.x()/m_CellSizeX),static_cast<G4int>(localCoord2.y()/m_CellSizeY),0};
}

const G4ThreeVector SDHCALCoordinates::getGlobalCoord(const G4int& I ,const G4int& J) const
{
  G4ThreeVector localCoord( -0.5*m_SizeX + (I+0.5)*m_CellSizeX , -0.5*m_SizeY + (J+0.5)*m_CellSizeY , 0) ;
  return m_RPCToGlobalTransform.TransformPoint(localCoord) ;
}

void SDHCALCoordinates::setCoordTransform(const G4AffineTransform& trans)
{
  m_GlobalToRPCTransform = trans;
  m_RPCToGlobalTransform = m_GlobalToRPCTransform.Inverse();
  m_TransformComputed = true ;
}

const G4ThreeVector SDHCALCoordinates::globalToRPCCoordTransform(const G4ThreeVector& globalCoord) const
{
  return m_GlobalToRPCTransform.TransformPoint(globalCoord) ;
}
const G4ThreeVector SDHCALCoordinates::RPCToGlobalCoordTransform(const G4ThreeVector& localCoord) const
{
  return m_RPCToGlobalTransform.TransformPoint(localCoord) ;
}
