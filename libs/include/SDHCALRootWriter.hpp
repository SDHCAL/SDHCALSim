#pragma once

#include "TFile.h"
#include "TTree.h"

#include "G4ThreeVector.hh"

#include<vector>
#include<memory>

class SDHCALRootWriter
{
public:
  void setFileName(const G4String& name){m_FileName=name;}
  void openFile();
  void closeFile();
  void fillTree();
  inline void setEventNumber(int value){m_EventNumber=value;}
  inline void setNHit(int value){m_NHit=value;}
  inline void setPrimaryID(int value){m_PrimaryID=value;}
  inline void setPrimaryEnergy(double value){m_PrimaryEnergy=value;}
  void setPrimaryPos(G4ThreeVector value) 
  { 
    m_PrimaryPos[0]=value.x();
    m_PrimaryPos[1]=value.y();
    m_PrimaryPos[2]=value.z();
  }
  void setPrimaryMom(G4ThreeVector value)
  {
    m_PrimaryMom[0]=value.x();
    m_PrimaryMom[1]=value.y();
    m_PrimaryMom[2]=value.z();
  }
  inline void setDepositedEnergy(double value){m_DepositedEnergy=value;}
  inline void setDepositedEnergyNeutron(double value) {m_DepositedEnergyNeutron=value;}
  inline void setNNeutrons(int value){m_NNeutrons=value;}
  inline void setNPi0(int value){m_NPi0=value;}
  inline void setLeakedEnergy(double value){m_LeakedEnergy=value;}
  inline void setEmFraction(double value){m_EmFraction=value;}
  inline void setComputingTime(double value){m_ComputingTime=value;}
  inline void setStepCosAngle(std::vector<double> value){m_StepCosAngle=value;}
  inline void setStepLength(std::vector<double> value){m_StepLength=value;}
  inline void setStepTime(std::vector<double> value){m_StepTime=value;}

private:
  G4String m_FileName{""};
  std::unique_ptr<TFile> m_File{nullptr};
  TTree* m_Tree{nullptr};

  int m_EventNumber{0};
  int m_NHit{0};
  int m_PrimaryID{0};
  double m_PrimaryEnergy{0.};
  double m_PrimaryPos[3]{0.,0.,0.};
  double m_PrimaryMom[3]{0.,0.,0.};
  double m_DepositedEnergy{0.};
  double m_DepositedEnergyNeutron{0.};
  double m_LeakedEnergy{0.};
  double m_EmFraction{0.};
  double m_ComputingTime{0.};

  int m_NNeutrons{0};
  int m_NPi0{0};

  std::vector<double> m_StepCosAngle{};
  std::vector<double> m_StepLength{};
  std::vector<double> m_StepTime{};
};
