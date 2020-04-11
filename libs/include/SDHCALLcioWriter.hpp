#pragma once

#include <lcio.h>

#include <G4Event.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>

#include <IO/LCWriter.h>
#include <IOIMPL/LCFactory.h>
#include <IMPL/LCEventImpl.h>
#include <IMPL/SimCalorimeterHitImpl.h>
#include <IMPL/CalorimeterHitImpl.h>
#include <IMPL/LCGenericObjectImpl.h>
#include <IMPL/MCParticleImpl.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCTOOLS.h>
#include <IMPL/LCFlagImpl.h>
#include <UTIL/CellIDEncoder.h>

class G4Event;
class SDHCALHit;

class SDHCALLcioWriter
{
public:
  SDHCALLcioWriter();
  ~SDHCALLcioWriter();
  void setFileName(const G4String& name){m_FileName=name;}
  void openFile();
  void closeFile();
  void createLCEvent(const G4Event* event);
  void writeLCEvent();
  void createPrimaryParticles(const G4Event* event);
  void createSimCalorimeterHits(std::vector<SDHCALHit*> hits);
  void setValue(std::string key,int value);
  void setValue(std::string key,float value);
  void setValue(std::string key,double value);
  void setValue(std::string key,std::vector<int> values);
  void setValue(std::string key,std::vector<float> values);
  void setValue(std::string key,std::vector<double> values);
  void setValue(std::string key,G4ThreeVector values);
  void clear() ;

private:
  G4String m_FileName{""};
  lcio::LCWriter* m_Writer{nullptr};
  IMPL::LCEventImpl* m_LcEvent{nullptr};
  IMPL::LCCollectionVec* m_ParticleCol{nullptr};
  IMPL::LCCollectionVec* m_SimVec{nullptr};
  std::map<G4int,IMPL::MCParticleImpl*> m_PrimaryParticleMap{};
  G4String m_DetectorName{""};
};
