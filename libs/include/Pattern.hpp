#pragma once

#include "json.hpp"
#include "G4Types.hh"
#include "G4String.hh"

class Pattern
{
public:
  Pattern()=default;
  Pattern(const G4String& name):m_Name(name){}
  G4double getWidth();
  G4String getName();
  G4int getNbrLayers(){return m_Layers.size();}
  void addLayer(const G4String& name,const G4double& width,const G4String& material);
  bool hasLayer(const G4String& name);
  G4String getLayerMaterial(const G4int& i) {return m_Layers[i].getMaterial();}
  G4String getLayerName(const G4int& i) {return m_Layers[i].getName();}
  G4double getLayerWidth(const G4int& i) {return m_Layers[i].getWidth();}
  void print();
private:
  class Layer
  {
  public:
    Layer()=delete;
    Layer(const G4String& name,const G4double& width,const G4String& material):m_Name(name),m_Width(width),m_Material(material){}
    void setName(const G4String& name){m_Name=name;}
    void setWidth(const G4double& width){m_Width=width;}
    void setMaterial(const G4String& material){m_Material=material;}
    G4String getName(){return m_Name;}
    G4double getWidth(){return m_Width;}
    G4String getMaterial(){return m_Material;}
    void print();
  private:
    G4String m_Name{""};
    G4double m_Width{0.};
    G4String m_Material{""};
  };
  std::vector<Layer> m_Layers;
  G4double m_Width{0.};
  G4String m_Name{""};
};
