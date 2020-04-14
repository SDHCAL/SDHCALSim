#pragma once

#include "TFile.h"
#include "TTree.h"

#include "G4ThreeVector.hh"
#include "G4Event.hh"

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
  static std::unique_ptr<TFile> m_File;
  static TTree* m_Tree;

  static  int m_EventNumber;
  static  int m_NHit;
  static  int m_PrimaryID;
  static  double m_PrimaryEnergy;
  static  double m_PrimaryPos[3];
  static  double m_PrimaryMom[3];
  static  double m_DepositedEnergy;
  static  double m_DepositedEnergyNeutron;
  static  double m_LeakedEnergy;
  static  double m_EmFraction;
  static  double m_ComputingTime;

  static  int m_NNeutrons;
  static  int m_NPi0;

  static  std::vector<double> m_StepCosAngle;
  static  std::vector<double> m_StepLength;
  static  std::vector<double> m_StepTime;
};
