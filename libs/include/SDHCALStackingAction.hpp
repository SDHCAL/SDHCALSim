#pragma once

#include <G4UserStackingAction.hh>
#include <G4UImessenger.hh>
#include <globals.hh>
#include "G4Track.hh"

#include <map>

#include "json.hpp"

class SDHCALStackingAction : public G4UserStackingAction
{
public:
  SDHCALStackingAction(const nlohmann::json&);
  virtual ~SDHCALStackingAction(){}

		virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* track) ;
		void reset() { nParticlesPerId.clear() ; }

		const std::map<G4int , G4int>& getNumberOfParticlesPerID() const { return nParticlesPerId ; }

private:
  std::map<G4int , G4int> nParticlesPerId;
  G4bool m_KillNeutrons{false};
  nlohmann::json m_Json{};
};
