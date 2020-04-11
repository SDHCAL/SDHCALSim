#pragma once

#include <G4ThreeVector.hh>
#include <G4UserTrackingAction.hh>
#include <globals.hh>

#include <map>

class SDHCALTrackingAction : public G4UserTrackingAction
{
		//singleton stuff
		static SDHCALTrackingAction* instance ;
	protected :
		SDHCALTrackingAction() ;
		static void initInstance() { new SDHCALTrackingAction ; }
		virtual ~SDHCALTrackingAction() { instance = nullptr ; }
	public :
		static SDHCALTrackingAction* Instance()
		{
			if ( !instance )
				initInstance() ;
			return instance ;
		}


	public :

		struct PrimaryParticleInfo
		{
				G4int id {} ;
				G4int pdg {} ;
				G4ThreeVector momentum {} ;
		} ;

		virtual void PreUserTrackingAction(const G4Track* track) ;

		G4int getPrimaryParent(const G4Track* track) const ;
		PrimaryParticleInfo getPrimaryParticleInfo(G4int id) const ;

		void reset() ;


	private :

		std::map<G4int,PrimaryParticleInfo> primaryTrackMap {} ;
		std::map<G4int,G4int> linkMap {} ;

};
