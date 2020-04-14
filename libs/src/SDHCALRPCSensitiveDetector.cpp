#include "SDHCALRPCSensitiveDetector.hpp"

#include <G4VProcess.hh>
#include <G4Track.hh>
#include <G4Step.hh>
#include <G4SDManager.hh>
#include <G4ios.hh>

#include <sstream>

SDHCALRPCSensitiveDetector::SDHCALRPCSensitiveDetector(G4String name,const G4int& id):G4VSensitiveDetector(name),m_ID(id)
{
  std::stringstream colName;
  colName<<"HitsRPC"<<m_ID;
  collectionName.insert(colName.str());
}

void SDHCALRPCSensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
  currentHit = nullptr;
  hitsCollection = new SDHCALHitCollection(SensitiveDetectorName , collectionName.at(0));
  HCE->AddHitsCollection(m_ID , hitsCollection);
}

G4bool SDHCALRPCSensitiveDetector::ProcessHits(G4Step* step , G4TouchableHistory*)
{
  if(!Interested(step))return true;

  if (currentHit)
  {
    if(currentHit->getTrackID() == step->GetTrack()->GetTrackID() &&currentHit->getEndPos() == step->GetPreStepPoint()->GetPosition())
    {
      currentHit->updateWith(step);
    }
    else
    {
      currentHit->finalize();
      hitsCollection->insert(currentHit);
      currentHit = new SDHCALHit(step,m_ID);
      currentHit->setSizes(m_SizeX,m_SizeY,m_SizeZ);
      currentHit->setCellXYSize(m_CellSizeX,m_CellSizeY);
    }
  }
  else
  {
    currentHit = new SDHCALHit(step,m_ID);
    currentHit->setSizes(m_SizeX,m_SizeY,m_SizeZ);
    currentHit->setCellXYSize(m_CellSizeX,m_CellSizeY);
  }

  if(currentHit->isLeaving())
  {
    currentHit->finalize();
    hitsCollection->insert(currentHit);
    currentHit = nullptr;
  }
  return true;
}

G4bool SDHCALRPCSensitiveDetector::Interested(const G4Step* step) const
{
  return static_cast<G4bool>(step->GetTrack()->GetDynamicParticle()->GetCharge());
}

void SDHCALRPCSensitiveDetector::finalizeLastHit()
{
  if(!currentHit)return;
  currentHit->finalize();
  hitsCollection->insert(currentHit);
  currentHit = nullptr;
}

void SDHCALRPCSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
  finalizeLastHit();
}
