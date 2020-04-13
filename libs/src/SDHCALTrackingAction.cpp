#include "SDHCALTrackingAction.hpp"


#include <G4Track.hh>


SDHCALTrackingAction::SDHCALTrackingAction(): G4UserTrackingAction(){}

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

G4int SDHCALTrackingAction::getPrimaryParent(const G4int& trackID) const
{
	G4int parent = trackID;

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
