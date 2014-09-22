#include "LyonSensitiveDetector.hh"
#include "LyonSensitiveDetector.hh"
#include "G4VProcess.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

LyonSensitiveDetector::LyonSensitiveDetector(G4String name,int replicaCountLevel)
  : G4VSensitiveDetector(name),
    collectionID(-1),
    _replicaCountLevel(replicaCountLevel)
{
  collectionName.insert("LyonTrackHits");
  //add by rhan
  processName="hIoni";
}

void LyonSensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
  if (collectionID<0) collectionID=GetCollectionID(0);
  hitsCollection = new LyonTrackHitsCollection(SensitiveDetectorName,collectionName[0]);
  HCE->AddHitsCollection(collectionID,hitsCollection);
}


G4bool LyonSensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{

  
  
  //
  //  if(Interested(aStep))//select muIon process or not
  if ( aStep->GetTrack()->GetDynamicParticle()->GetCharge() != 0 )//add by rhan if Interested astep
  {
    
    LyonTrackHit* aHit=new LyonTrackHit(aStep,_replicaCountLevel);
    hitsCollection->insert(aHit);
   
  
  }
  return true;
}

//add by rhan

G4bool LyonSensitiveDetector::Interested(const G4Step* aStep) const
{
  G4Track* aTrack = aStep->GetTrack();

  // check if the track already has UserTrackInformation
  if(aTrack->GetUserInformation()) return true;

  // check if the track is interested particle and the step is limitted by
  // the interested process
  //if(aTrack->GetDefinition()!=interestedParticle) return false;
  const G4VProcess* processDefinedTheStep
    = aStep->GetPostStepPoint()->GetProcessDefinedStep();
  if((processDefinedTheStep->GetProcessName()==processName)||(processDefinedTheStep->GetProcessName()=="eIoni")||(processDefinedTheStep->GetProcessName()=="muIoni")) return true;
  //if(processDefinedTheStep->GetProcessName()=="muIoni") return true;
  

  return false;
}

