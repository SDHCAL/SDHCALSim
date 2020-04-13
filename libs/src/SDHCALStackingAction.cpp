#include "SDHCALStackingAction.hpp"

SDHCALStackingAction::SDHCALStackingAction(const nlohmann::json& json):G4UserStackingAction(),m_Json(json)
{
  m_KillNeutrons=m_Json.value("killNeutrons",false);
}

G4ClassificationOfNewTrack SDHCALStackingAction::ClassifyNewTrack(const G4Track* track)
{
  nParticlesPerId[ track->GetDefinition()->GetPDGEncoding() ]++;
  if(!m_KillNeutrons)return fUrgent;
  if(track->GetDefinition()->GetPDGEncoding()==2112) return fKill;
  return fUrgent;
}

