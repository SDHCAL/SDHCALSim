#include "SDHCALRPCSensitiveDetector.hpp"

#include <G4VProcess.hh>
#include <G4Track.hh>
#include <G4Step.hh>
#include <G4SDManager.hh>
#include <G4ios.hh>

#include <sstream>

#include "SDHCALRPC.hpp"

std::set<SDHCALRPCSensitiveDetector*> SDHCALRPCSensitiveDetector::sensitiveDetectorVec ;

SDHCALRPCSensitiveDetector::SDHCALRPCSensitiveDetector(G4String name , SDHCALRPC* _rpc)
	: G4VSensitiveDetector(name)
{
	rpc = _rpc ;
	ID = rpc->getID() ;
	std::stringstream colName ; colName << "HitsRPC" << ID ;
	collectionName.insert( colName.str() ) ;

	sensitiveDetectorVec.insert(this) ;
}
SDHCALRPCSensitiveDetector::~SDHCALRPCSensitiveDetector()
{
	std::set<SDHCALRPCSensitiveDetector*>::iterator it = sensitiveDetectorVec.find(this) ;
	sensitiveDetectorVec.erase(it) ;
}

void SDHCALRPCSensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
	currentHit = nullptr ;

	hitsCollection = new SDHCALHitCollection(SensitiveDetectorName , collectionName.at(0) ) ;
	HCE->AddHitsCollection(ID , hitsCollection) ;
}

G4bool SDHCALRPCSensitiveDetector::ProcessHits(G4Step* step , G4TouchableHistory*)
{
	if ( !Interested(step) )
		return true ;

	if (currentHit)
	{
		if ( currentHit->getTrackID() == step->GetTrack()->GetTrackID() &&
			 currentHit->getEndPos() == step->GetPreStepPoint()->GetPosition() )
		{
			currentHit->updateWith(step) ;
		}
		else
		{
			currentHit->finalize() ;
			hitsCollection->insert(currentHit) ;
			currentHit = new SDHCALHit(step , rpc) ;
		}
	}
	else
	{
		currentHit = new SDHCALHit(step , rpc) ;
	}

	if ( currentHit->isLeaving() )
	{
		currentHit->finalize() ;
		hitsCollection->insert(currentHit) ;
		currentHit = nullptr ;
	}

	return true ;
}



G4bool SDHCALRPCSensitiveDetector::Interested(const G4Step* step) const
{
	return static_cast<G4bool>( step->GetTrack()->GetDynamicParticle()->GetCharge() ) ;
}

void SDHCALRPCSensitiveDetector::finalizeLastHit()
{
	if (!currentHit)
		return ;

	currentHit->finalize() ;
	hitsCollection->insert(currentHit) ;
	currentHit = nullptr ;
}

void SDHCALRPCSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
	finalizeLastHit() ;
}

