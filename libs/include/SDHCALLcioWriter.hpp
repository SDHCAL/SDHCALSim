#pragma once

#include "lcio.h"

#include "G4ThreeVector.hh"
#include "G4Event.hh"

#include "IO/LCWriter.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/MCParticleImpl.h"

#include <memory>

class SDHCALHit;

class SDHCALLcioWriter
{
public:
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
  static lcio::LCWriter* m_Writer;
  static G4ThreadLocal IMPL::LCEventImpl* m_LcEvent;
  static G4ThreadLocal std::map<G4int,IMPL::MCParticleImpl*> m_PrimaryParticleMap;
  static G4ThreadLocal IMPL::LCCollectionVec* m_ParticleCol;
  static G4ThreadLocal IMPL::LCCollectionVec* m_SimVec;
  static G4String m_DetectorName;
};
