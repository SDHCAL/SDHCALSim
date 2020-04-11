#pragma once

#include <G4UserEventAction.hh>
#include <G4SDManager.hh>

#include "SDHCALRunAction.hpp"
#include "SDHCALSteppingAction.hpp"
#include "SDHCALTrackingAction.hpp"
#include "SDHCALStackingAction.hpp"
#include "SDHCALPrimaryGeneratorAction.hpp"
#include "SDHCALRPCSensitiveDetector.hpp"
#include "SDHCALRootWriter.hpp"

#include <ctime>


class SDHCALEventAction : public G4UserEventAction
{
	public :
		SDHCALEventAction(SDHCALRunAction* _runAction) ;
		virtual ~SDHCALEventAction() = default ;

		virtual void BeginOfEventAction(const G4Event* event) ;
		virtual void EndOfEventAction(const G4Event* event) ;

		SDHCALEventAction(const SDHCALEventAction&) = delete ;
		void operator=(const SDHCALEventAction&) = delete ;

	private :

		clock_t beginClock {} ;
		double averageTime {} ;
		unsigned int nEventsProcessed {} ;

		SDHCALRunAction* runAction {} ;

};
