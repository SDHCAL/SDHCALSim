#pragma once

#include "G4VHit.hh"
#include "G4ThreeVector.hh"
#include "G4THitsCollection.hh"
#include "G4Step.hh"

#include "SDHCALCoordinates.hpp"

class SDHCALHit;
extern G4ThreadLocal G4Allocator<SDHCALHit>* SDHCALHitHitAllocator;

class SDHCALHit : public G4VHit
{
public:
  SDHCALHit(const G4Step* step,const G4int id);
  void setSizes(const G4double& X,const G4double& Y, const G4double& Z)
  {
    m_Coordinates.setSizes(X,Y,Z);
  }
  void setCellXYSize(const G4double& X,const G4double& Y)
  {
    m_Coordinates.setCellXYSize(X,Y);
  }
		void updateWith(const G4Step* step) ;
		void finalize() ;
		virtual void computePosition() ;

		inline const G4ThreeVector getPos() const { return 0.5*(endPos + beginPos) ; }
		inline const G4ThreeVector& getBeginPos() const { return beginPos ; }
		inline const G4ThreeVector& getEndPos() const { return endPos ; }
		inline const G4ThreeVector& getDeltaPos() const { return deltaPos ; }

		inline G4int getTrackID() const { return trackID ; }
		inline G4int getPdgID() const { return pdgID ; }
		inline G4double getEnergyDeposited() const { return energyDeposited ; }
		inline G4double getBetaGamma() const { return betaGamma ; }
		inline G4int getK() const {return m_K;}
		inline G4int getI() const { return I ; }
		inline G4int getJ() const { return J ; }

  inline G4double getCellSizeX(){return m_Coordinates.getCellSizeX();}
  inline G4double getCellSizeY(){return m_Coordinates.getCellSizeY();}
  G4ThreeVector RPCToGlobalCoordTransform(const G4ThreeVector& localCoord)
  {
    return m_Coordinates.RPCToGlobalCoordTransform(localCoord);
  }
		inline G4bool isEntering() const { return isEnteringStep ; }
		inline G4bool isLeaving() const { return isLeavingStep ; }
		inline G4double getTime() const { return time ; }
		inline G4double getTrueLength() const { return trueLength ; }
		inline G4double getCharge() const { return charge ; }
		inline const G4ThreeVector& getCoordInPad() const { return coordInPad ; }
		inline G4int getTrackStatus() const { return trackStatus ; }
//		inline G4PrimaryParticle* getPrimaryParticle() const { return primaryParticle ; }
 		inline G4int getPrimaryID() const {return m_PrimaryID;}
    inline void setPrimaryID(const G4int& primaryID){m_PrimaryID=primaryID;}

		virtual void Print() ;
  inline void* operator new(size_t)
  {
    if(!SDHCALHitHitAllocator) SDHCALHitHitAllocator=new G4Allocator<SDHCALHit>;
    return (void*) SDHCALHitHitAllocator->MallocSingle();
  }
  
  inline void operator delete(void*hit)
  {
    SDHCALHitHitAllocator->FreeSingle((SDHCALHit*)hit);
  }


private:
  SDHCALCoordinates m_Coordinates;
  G4ThreeVector beginPos {} ;
  G4ThreeVector endPos {} ;
  G4ThreeVector deltaPos {} ;
  G4int trackID {} ;
  G4int pdgID {} ;
  G4double energyDeposited {} ;
  G4double betaGamma {} ;
  G4int m_K{0};
  G4int I {} ;
  G4int J {} ;
  G4bool isEnteringStep {} ;
  G4bool isLeavingStep {} ;
  G4double time {} ;
  G4double trueLength {} ;
  G4double charge {} ;
  G4ThreeVector coordInPad {} ;
  //G4PrimaryParticle* primaryParticle ;
  G4int m_PrimaryID{0};
  G4int trackStatus{};
};

typedef G4THitsCollection<SDHCALHit> SDHCALHitCollection;
