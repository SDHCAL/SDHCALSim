#ifndef SDHCALEventAction_h
#define SDHCALEventAction_h

#include <G4UserEventAction.hh>
#include <G4SDManager.hh>

#include "SDHCALRunAction.h"
#include "SDHCALSteppingAction.h"
#include "SDHCALTrackingAction.h"
#include "SDHCALStackingAction.h"
#include "SDHCALPrimaryGeneratorAction.h"
#include "SDHCALRPCSensitiveDetector.h"
#include "SDHCALRootWriter.h"

#include <ctime>


class SDHCALEventAction : public G4UserEventAction
{
	public :
		SDHCALEventAction(SDHCALRunAction* _runAction) ;
		virtual ~SDHCALEventAction() ;

		virtual void BeginOfEventAction(const G4Event* event) ;
		virtual void EndOfEventAction(const G4Event* event) ;

	private :

		clock_t beginClock ;
		double averageTime ;
		unsigned int nEventsProcessed ;

		SDHCALRunAction* runAction ;

} ;

#endif //SDHCALEventAction_h
