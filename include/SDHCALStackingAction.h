#ifndef SDHCALStackingAction_h
#define SDHCALStackingAction_h

#include <G4UserStackingAction.hh>
#include <G4UImessenger.hh>
#include <globals.hh>

#include <map>

class G4Track ;

class SDHCALStackingAction : public G4UserStackingAction
{
		//singleton stuff
		static SDHCALStackingAction* instance ;
	protected :
		SDHCALStackingAction() ;
		static void initInstance() { new SDHCALStackingAction ; }
		virtual ~SDHCALStackingAction() { instance = nullptr ; }
	public :
		static SDHCALStackingAction* Instance()
		{
			if ( !instance )
				initInstance() ;
			return instance ;
		}

	public :

		virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* track) ;

		void setKillNeutrons( G4bool kill ) { killNeutrons = kill ; }
		void reset() { nParticlesPerId.clear() ; }

		const std::map<G4int , G4int>& getNumberOfParticlesPerID() const { return nParticlesPerId ; }

	protected :
		std::map<G4int , G4int> nParticlesPerId = {} ;

		G4bool killNeutrons = false ;
} ;

#endif //LyonStackingAction_h
