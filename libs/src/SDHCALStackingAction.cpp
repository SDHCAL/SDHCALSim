#include "SDHCALStackingAction.hpp"

#include <G4Track.hh>

SDHCALStackingAction* SDHCALStackingAction::instance = nullptr ;

SDHCALStackingAction::SDHCALStackingAction()
	: G4UserStackingAction()
{
	if (instance)
		throw std::logic_error("SDHCALStackingAction already exists") ;

	instance = this ;

	G4cout << "Create SDHCALStackingAction" << G4endl ;
}

G4ClassificationOfNewTrack SDHCALStackingAction::ClassifyNewTrack(const G4Track* track)
{
	nParticlesPerId[ track->GetDefinition()->GetPDGEncoding() ]++ ;

	if ( !killNeutrons )
		return fUrgent ;

	if ( track->GetDefinition()->GetPDGEncoding() == 2112 )
		return fKill ;

	return fUrgent ;
}

