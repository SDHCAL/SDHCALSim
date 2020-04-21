#include "PatternTypes.hpp"

#include "G4String.hh"
#include "globals.hh"

#include <fstream>

PatternTypes::PatternTypes(const G4String& type):m_Type(type)
{
  verifyParameters();
}

void PatternTypes::setJsonFile(const nlohmann::json& json)
{
  m_Json=json;
  verifyParameters();
}

bool PatternTypes::hasType(const G4String& name)
{
  if(m_Pattern.find(name)!=m_Pattern.end()) return true;
  else return false;
}

void PatternTypes::verifyParameters()
{
  if(m_Json.count("detectorConfig"))
  {
    auto detectorConfig = m_Json.at("detectorConfig");
    if(detectorConfig.count("Types"))
    {
      G4String jsonFileName = detectorConfig.at("Types").get<G4String>();
      if(jsonFileName == G4String(""))
      {
        G4cerr << "ERROR : no json file provided" << G4endl ;
        std::exit(-1) ;
      }
      std::ifstream file(jsonFileName.c_str()) ;
      try
      {
        m_JsonTypes = nlohmann::json::parse(file);
      }
      catch (nlohmann::json::parse_error& e)
      {
        G4cerr <<  "message: " << e.what() << '\n'
        << "exception id: " << e.id << '\n'
        << "byte position of error: " << e.byte << G4endl ;
        std::exit(-1) ;
      }
    }
    else
    {
      G4cerr << "Types not provided !" << G4endl ;
      std::exit(-1) ;
    }
  }
}

void PatternTypes::buildTypes()
{
  if(m_JsonTypes.count(m_Type+"Types"))
  {
    for(nlohmann::json::iterator it =m_JsonTypes.at(m_Type+"Types").begin();it!=m_JsonTypes.at(m_Type+"Types").end();++it)
    {
      if(m_Pattern.find(it->at("Name"))!=m_Pattern.end())
      {
        G4cerr << m_Type<<"Type with name "<<it->at("Name")<<" already loaded"<< G4endl ;
        std::exit(-1) ;
      }
      else m_Pattern[it->at("Name")]=Pattern(it->at("Name"));
      for(nlohmann::json::iterator itt=it->at("Layers").begin();itt!=it->at("Layers").end();++itt)
      {
        m_Pattern[it->at("Name")].addLayer(itt->at("Name").get<G4String>(),itt->at("Width").get<G4double>(),itt->at("Material").get<G4String>());
      }
    }
  }
  else
  {
    G4cerr << m_Type<<"Types not provided !" << G4endl;
    std::exit(-1);
  }
}
