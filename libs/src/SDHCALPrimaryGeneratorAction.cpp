#include "SDHCALPrimaryGeneratorAction.hpp"

SDHCALPrimaryGeneratorAction::SDHCALPrimaryGeneratorAction(const nlohmann::json& json):m_Json(json)
{
  if(!m_Json.count("particuleGuns"))
  {
    G4cout<<"ERROR : no gun provided"<<G4endl;
    std::exit(-1);
  }
  auto gunList = m_Json.at("particuleGuns");
  for(const auto& gun:gunList) m_GunVec.push_back(std::make_shared<SDHCALGun>(gun));
}

void SDHCALPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  for ( const auto& gun : m_GunVec )gun->generatePrimary(event) ;
}

void SDHCALPrimaryGeneratorAction::print() const
{
  G4cout << "Primary particles : " << G4endl ;
  for ( const auto& gun : m_GunVec )
  {
    G4cout << G4endl ;
    G4cout << "    pdgID : " << gun->GetParticleDefinition()->GetParticleName() << G4endl ;
    G4cout << "     Time : " << gun->GetParticleTime() << " ns" << G4endl ;
    G4cout << "      Pos : " << gun->GetParticlePosition() << G4endl ;
    G4cout << "      Mom : " << gun->GetParticleMomentumDirection() << G4endl ;
    G4cout << "   Energy : " << gun->GetParticleEnergy()/CLHEP::GeV << " GeV" << G4endl ;
  }
}


