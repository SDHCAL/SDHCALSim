#include "SDHCALRootWriter.hpp"

#include <stdexcept>

SDHCALRootWriter* SDHCALRootWriter::instance = nullptr ;

SDHCALRootWriter::SDHCALRootWriter()
{
	if (instance)
		throw std::logic_error("SDHCALRootWriter already exists") ;

	instance = this ;

	file = nullptr ;
	tree = nullptr ;
}

void SDHCALRootWriter::createRootFile( std::string fileName )
{
	file = new TFile(fileName.c_str() , "RECREATE") ;
	tree = new TTree("tree" , "tree") ;

	tree->Branch("eventNumber" , &eventNumber) ;
	tree->Branch("nHit" , &nHit) ;
	tree->Branch("primaryID" , &primaryID) ;
	tree->Branch("primaryEnergy" , &primaryEnergy) ;
	tree->Branch("primaryPos" , &primaryPos , "primaryPos[3]/D") ;
	tree->Branch("primaryMom" , &primaryMom , "primaryMom[3]/D") ;
	tree->Branch("depositedEnergy" , &depositedEnergy) ;
	tree->Branch("depositedEnergyNeutron" , &depositedEnergyNeutron) ;
	tree->Branch("nNeutrons" , &nNeutrons) ;
	tree->Branch("nPi0" , &nPi0) ;
	tree->Branch("leakedEnergy" , &leakedEnergy) ;
	tree->Branch("emFraction" , &emFraction) ;
	tree->Branch("computingTime" , &computingTime) ;
	tree->Branch("stepAngle" , "std::vector<double>" , &stepCosAngle) ;
	tree->Branch("stepLength" , "std::vector<double>" , &stepLength) ;
	tree->Branch("stepTime" , "std::vector<double>" , &stepTime) ;
}

void SDHCALRootWriter::closeRootFile()
{
	file->cd() ;
	tree->Write("tree") ;
	file->Purge() ;
	file->Close() ;
}

void SDHCALRootWriter::fillTree()
{
	tree->Fill() ;
}

