#ifndef SDHCALTrackingAction_h
#define SDHCALTrackingAction_h

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
				G4int id = 0 ;
				G4int pdg = 0 ;
				G4ThreeVector momentum ;
		} ;

		virtual void PreUserTrackingAction(const G4Track* track) ;

		G4int getPrimaryParent(const G4Track* track) const ;
		PrimaryParticleInfo getPrimaryParticleInfo(G4int id) const ;


	private :

		std::map<G4int,PrimaryParticleInfo> primaryTrackMap = {} ;
		std::map<G4int,G4int> linkMap = {} ;

} ;



#endif //SDHCALTrackingAction_h
