#!/usr/bin/env python

import os
import sys
import SDHCALSim2 as s

import math


if __name__ == '__main__' :

	os.environ["SIMEXE"] = '/home/garillot/SDHCALSim2/bin/SDHCALSim'

	params = s.Params()

	params.physicsList = 'FTF_BIC'
	params.nEvent = 200
	params.seed = 0
	params.outputFileName = 'dual'


	charged = s.Particle()
	charged.particleName = 'pi+'
	charged.energy = 30

	charged.positionOption = 'gaus'
	charged.positionX = 30
	charged.positionY = 30
	charged.sigmaPos = 2 

	charged.momentumOption = 'gaus'
	charged.sigmaMomentum = 0.05 


	neutral = s.Particle()
	neutral.particleName = 'kaon0'
	neutral.energy = 10

	neutral.time = 50

	neutral.positionOption = 'gaus'
	neutral.positionX = -30
	neutral.positionY = -30
	neutral.sigmaPos = 2

	neutral.momentumOption = 'gaus'
	neutral.sigmaMomentum = 0.05 

	params.particleList.append(charged)
	params.particleList.append(neutral)


	s.launch( params )
