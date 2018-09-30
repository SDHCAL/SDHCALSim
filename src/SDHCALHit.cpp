#include "SDHCALHit.h"

#include <G4Step.hh>
#include <G4TouchableHistory.hh>
#include "SDHCALTrackingAction.h"

#include "SDHCALRPC.h"

SDHCALHit::SDHCALHit()
{
}

SDHCALHit::SDHCALHit(const G4Step* step , SDHCALRPC* _rpc)
{
	rpc = _rpc ;
	if ( !rpc->isTransformComputed() )
		rpc->setCoordTransform( step->GetPreStepPoint()->GetTouchable()->GetHistory()->GetTopTransform() ) ;

	beginPos = step->GetPreStepPoint()->GetPosition() ;
	endPos = step->GetPostStepPoint()->GetPosition() ;
	deltaPos = endPos - beginPos ;
	trackID = step->GetTrack()->GetTrackID() ;
	pdgID = step->GetTrack()->GetDynamicParticle()->GetPDGcode() ;
	energyDeposited = step->GetTotalEnergyDeposit() ;

	betaGamma = step->GetPreStepPoint()->GetBeta()*step->GetPreStepPoint()->GetGamma() ;

	RPCID = rpc->getID() ;
	isEnteringStep = (step->GetPreStepPoint()->GetStepStatus() == fGeomBoundary) ;
	isLeavingStep = (step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) ;
	time = step->GetPreStepPoint()->GetGlobalTime() ;
	trueLength = deltaPos.mag() ;
	charge = step->GetTrack()->GetDynamicParticle()->GetCharge() ;
	trackStatus = step->GetTrack()->GetTrackStatus() ;

	primaryID = SDHCALTrackingAction::Instance()->getPrimaryParent( step->GetTrack() ) ;

	//I , J and coordInPad are computed when hit is finalized
}

void SDHCALHit::computePosition()
{
	G4ThreeVector globalPosition = 0.5*( endPos + beginPos ) ;
	G4ThreeVector posInRPC = rpc->globalToRpcCoordTransform(globalPosition) ;

	I = rpc->localCoordToIJ(posInRPC).at(0) ;
	J = rpc->localCoordToIJ(posInRPC).at(1) ;
	coordInPad = posInRPC - rpc->IJToLocalCoord(I,J) ;
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
	std::cout << "Hit at " << "I:" << I << " J:" << J << " K:" << RPCID << " at pos " << endPos-beginPos << std::endl ;
}
