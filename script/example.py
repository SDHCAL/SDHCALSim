#!/usr/bin/env python

import os
import sys
import SDHCALSim as sim

import math


if __name__ == '__main__' :
  executable='SDHCALSim'
  os.environ["SIMEXE"] = os.path.join(os.path.dirname(__file__),os.pardir,'bin',executable)

  particle = 'e-'
  energy = 10
  params = sim.Params()
  params.physicsList = 'QGSP_BERT'
  params.nEvent = 10
  params.seed = 0
  params.outputFileName = 'outputName'
  params.MultiThread=False
  params.oldConfig = False
  params.killNeutrons = False

  charged = sim.Particle()
  charged.particleName = particle
  charged.energy = energy

  charged.momentumOption = 'gaus'
  charged.sigmaMomentum = 0.15

  charged.positionOption = 'gaus'
  charged.sigmaPos = 33

  params.particleList.append(charged)

  sim.launch( params )
