#include "Pattern.hpp"

#include "globals.hh"
#include "CLHEP/Units/SystemOfUnits.h"

G4double Pattern::getWidth()
{
  return m_Width;
}
  
G4String Pattern::getName()
{
  return m_Name;
}

void Pattern::addLayer(const G4String& name,const G4double& width,const G4String& material)
{
  m_Layers.push_back(Layer(name,width,material));
  m_Width+=width*CLHEP::mm;
}

bool Pattern::hasLayer(const G4String& name)
{
  for(std::size_t i=0;i!=m_Layers.size();++i)
  {
    if(m_Layers[i].getName()==name) return true;
  }
  return false;
}

void Pattern::print()
{
  G4cout<<"TYPE "<<m_Name<<" : "<<G4endl;
  for(std::size_t i=0;i!=m_Layers.size();++i)
  {
    m_Layers[i].print();
  }
  G4cout<<"Total Width "<<m_Width<<" : "<<G4endl;
}


void Pattern::Layer::print()
{
  G4cout<<"Name : "<<m_Name<<" Width : "<<m_Width<<" Material : "<<m_Material<<G4endl;
}
