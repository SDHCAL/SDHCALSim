#!/usr/bin/env python

import os
import sys
import SDHCALSim

import math



if __name__ == '__main__' :

	os.environ["SIMEXE"] = '/home/garillot/SDHCALSim2/bin/SDHCALSim'

	params = SDHCALSim.Params()

	params.particle = "pi-"
	params.physicsList = "FTF_BIC"

	#params.energy = sys.argv[1]
	params.energyDistribution = "uniform"
	params.minEnergy = 19.9
	params.maxEnergy = 20

	params.nEvent = 10
	params.seed = 0

	params.momentumOption = "gaus"
	params.sigmaMomentum = 0.20

	params.positionOption = "uniform"
	params.positionX = 0
	params.positionY = 0
	#params.uniformDeltaPos = 500
	params.sigmaPos = 100 

	SDHCALSim.launch( params )

	#os.system('rm Oldtest.slcio')

	outputFile = 'single_' + params.particle + '_' + str(params.energy) + 'GeV' + '_I' + str(params.seed) 

	os.system('mv test.slcio ' + outputFile + '.slcio')

	#os.system('mv test.slcio /home/garillot/files/local/SimCalorimeterHit/Geant4.10.01/' + params.physicsList + '/' + outputFile + '.slcio')
	#os.system('mv test.root /home/garillot/files/local/SimCalorimeterHit/Geant4.10.01/' + params.physicsList + '/ControlFiles/' + outputFile + '.root')

	#os.system('mv test.slcio /home/garillot/files/muonsForBing/' + outputFile + '.slcio')
	#os.system('mv test.root /home/garillot/files/muonsForBing/ControlFiles/' + outputFile + '.root')
