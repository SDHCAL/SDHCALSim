#pragma once

#include <G4VSensitiveDetector.hh>
#include <G4HCofThisEvent.hh>
#include <G4Event.hh>

#include <set>

#include "SDHCALHit.hpp"

class G4Step ;
class G4HCofThisEvent ;
class SDHCALRPC ;

class SDHCALRPCSensitiveDetector : public G4VSensitiveDetector
{
	public :
		SDHCALRPCSensitiveDetector(G4String name , SDHCALRPC* _rpc) ;
		virtual ~SDHCALRPCSensitiveDetector() ;

		virtual void Initialize(G4HCofThisEvent*) ;
		virtual G4bool ProcessHits(G4Step* step , G4TouchableHistory*) ;

		virtual void EndOfEvent(G4HCofThisEvent*) ;

		virtual G4bool Interested(const G4Step* step) const ;

		void finalizeLastHit() ;

		static inline const std::set<SDHCALRPCSensitiveDetector*>& getSensitiveDetectorVec() { return sensitiveDetectorVec ; }

		SDHCALRPCSensitiveDetector(const SDHCALRPCSensitiveDetector&) = delete ;
		void operator=(const SDHCALRPCSensitiveDetector&) = delete ;

	protected :
		static std::set<SDHCALRPCSensitiveDetector*> sensitiveDetectorVec ;

		SDHCALHitCollection* hitsCollection {} ;
		G4int ID {} ;

		SDHCALRPC* rpc {} ;

		SDHCALHit* currentHit {} ;
};
