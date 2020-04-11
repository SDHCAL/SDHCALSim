#include "SDHCALRootWriter.hpp"

SDHCALRootWriter::SDHCALRootWriter()
{
}

void SDHCALRootWriter::openFile()
{
  m_File=new TFile(m_FileName.c_str(),"RECREATE");
  m_Tree=new TTree("tree","tree");
  m_Tree->Branch("eventNumber",&m_EventNumber);
  m_Tree->Branch("nHit",&m_NHit);
  m_Tree->Branch("primaryID",&m_PrimaryID);
  m_Tree->Branch("primaryEnergy",&m_PrimaryEnergy);
  m_Tree->Branch("primaryPos",&m_PrimaryPos , "primaryPos[3]/D");
  m_Tree->Branch("primaryMom",&m_PrimaryMom , "primaryMom[3]/D");
  m_Tree->Branch("depositedEnergy",&m_DepositedEnergy);
  m_Tree->Branch("depositedEnergyNeutron",&m_DepositedEnergyNeutron);
  m_Tree->Branch("nNeutrons",&m_NNeutrons);
  m_Tree->Branch("nPi0",&m_NPi0);
  m_Tree->Branch("leakedEnergy",&m_LeakedEnergy);
  m_Tree->Branch("emFraction",&m_EmFraction);
  m_Tree->Branch("computingTime",&m_ComputingTime);
  m_Tree->Branch("stepAngle","std::vector<double>",&m_StepCosAngle);
  m_Tree->Branch("stepLength","std::vector<double>",&m_StepLength);
  m_Tree->Branch("stepTime","std::vector<double>",&m_StepTime);
}

void SDHCALRootWriter::closeFile()
{
  m_File->cd();
  m_Tree->Write("m_Tree");
  m_File->Purge();
  m_File->Close();
}

void SDHCALRootWriter::fillTree()
{
  m_Tree->Fill();
}
