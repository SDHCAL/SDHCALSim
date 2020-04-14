#include "SDHCALHit.hpp"

#include "G4Step.hh"
#include "G4TouchableHistory.hh"

G4ThreadLocal G4Allocator<SDHCALHit>* SDHCALHitHitAllocator=0;

SDHCALHit::SDHCALHit(const G4Step* step,const G4int id):m_K(id)
{
  if(!m_Coordinates.isTransformComputed())
  {
    m_Coordinates.setCoordTransform(step->GetPreStepPoint()->GetTouchable()->GetHistory()->GetTopTransform());
  }
	beginPos = step->GetPreStepPoint()->GetPosition() ;
	endPos = step->GetPostStepPoint()->GetPosition() ;
	deltaPos = endPos - beginPos ;
	trackID = step->GetTrack()->GetTrackID() ;
	pdgID = step->GetTrack()->GetDynamicParticle()->GetPDGcode() ;
	energyDeposited = step->GetTotalEnergyDeposit() ;

	betaGamma = step->GetPreStepPoint()->GetBeta()*step->GetPreStepPoint()->GetGamma() ;
	isEnteringStep = (step->GetPreStepPoint()->GetStepStatus() == fGeomBoundary) ;
	isLeavingStep = (step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) ;
	time = step->GetPreStepPoint()->GetGlobalTime() ;
	trueLength = deltaPos.mag() ;
	charge = step->GetTrack()->GetDynamicParticle()->GetCharge() ;
	trackStatus = step->GetTrack()->GetTrackStatus() ;
	//I , J and coordInPad are computed when hit is finalized
}

void SDHCALHit::computePosition()
{
	G4ThreeVector globalPosition = 0.5*( endPos + beginPos ) ;
	G4ThreeVector posInRPC = m_Coordinates.globalToRPCCoordTransform(globalPosition) ;

	I = m_Coordinates.localCoordToIJ(posInRPC).at(0) ;
	J = m_Coordinates.localCoordToIJ(posInRPC).at(1) ;
	coordInPad = posInRPC - m_Coordinates.IJToLocalCoord(I,J) ;
}

void SDHCALHit::updateWith(const G4Step* step)
{
	endPos = step->GetPostStepPoint()->GetPosition() ;
	deltaPos = endPos - beginPos ;
	energyDeposited += step->GetTotalEnergyDeposit() ;
	isLeavingStep = (step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) ;
	trueLength += step->GetDeltaPosition().mag() ;

	if( trackStatus < step->GetTrack()->GetTrackStatus() )
		trackStatus = step->GetTrack()->GetTrackStatus() ;
}

void SDHCALHit::finalize()
{
	computePosition() ;
}

void SDHCALHit::Print()
{
  G4cout<<"Hit at "<<"I:"<<I<<" J:"<<J<<" K:"<<m_K<<" at pos "<<endPos-beginPos<<G4endl;
}
