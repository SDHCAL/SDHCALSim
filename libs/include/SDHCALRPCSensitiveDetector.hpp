#pragma once

#include "G4VSensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"
#include "G4Step.hh"

#include <set>

#include "SDHCALHit.hpp"

class SDHCALRPC ;

class SDHCALRPCSensitiveDetector : public G4VSensitiveDetector
{
public :
		SDHCALRPCSensitiveDetector(G4String name , SDHCALRPC* _rpc) ;
		virtual ~SDHCALRPCSensitiveDetector()=default;

		virtual void Initialize(G4HCofThisEvent*) ;
		virtual G4bool ProcessHits(G4Step* step , G4TouchableHistory*) ;

		virtual void EndOfEvent(G4HCofThisEvent*) ;

		virtual G4bool Interested(const G4Step* step) const ;

		void finalizeLastHit() ;

		SDHCALRPCSensitiveDetector(const SDHCALRPCSensitiveDetector&) = delete ;
		void operator=(const SDHCALRPCSensitiveDetector&) = delete ;

private:

		SDHCALHitCollection* hitsCollection {} ;
		G4int ID {} ;

		SDHCALRPC* rpc {} ;

		SDHCALHit* currentHit {} ;
};
