#include "SDHCALTrackingAction.hpp"


#include <G4Track.hh>

#include <stdexcept>

SDHCALTrackingAction* SDHCALTrackingAction::instance = nullptr ;

SDHCALTrackingAction::SDHCALTrackingAction()
	: G4UserTrackingAction()
{
	if (instance)
		throw std::logic_error("SDHCALTrackingAction already exists") ;

	instance = this ;

	G4cout << "Create SDHCALTrackingAction" << G4endl ;
}

void SDHCALTrackingAction::reset()
{
	linkMap.clear() ;
	primaryTrackMap.clear() ;
}


void SDHCALTrackingAction::PreUserTrackingAction(const G4Track* track)
{
	linkMap.insert( {track->GetTrackID() , track->GetParentID()} ) ;

	if ( track->GetParentID() == 0 )
	{
		PrimaryParticleInfo info ; info.id = track->GetTrackID() ; info.momentum = track->GetMomentum() ; info.pdg = track->GetParticleDefinition()->GetPDGEncoding() ;
		primaryTrackMap.insert( {info.id , info} ) ;
	}
}

G4int SDHCALTrackingAction::getPrimaryParent(const G4Track* track) const
{
	G4int parent = track->GetTrackID() ;

	while ( true )
	{
		if ( linkMap.at(parent) == 0 )
			return parent ;

		parent = linkMap.at(parent) ;
	}
}

SDHCALTrackingAction::PrimaryParticleInfo SDHCALTrackingAction::getPrimaryParticleInfo(G4int id) const
{
	SDHCALTrackingAction::PrimaryParticleInfo info ;
	try
	{
		info = primaryTrackMap.at(id) ;
	}
	catch (std::out_of_range&)
	{
		G4cout << "SDHCALTrackingAction::getPrimaryParticleInfo(" << id << ") -> error" << G4endl ;
	}
	return info ;
}
