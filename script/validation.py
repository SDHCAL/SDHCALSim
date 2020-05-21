#!/usr/bin/env python

import os
import sys
import SDHCALSim as sim

import math


if __name__ == '__main__' :
  executable='SDHCALSim'
  os.environ["SIMEXE"] = os.path.join(os.path.dirname(__file__),os.pardir,'bin',executable)
  os.environ["SIMCONFIG"] = os.path.join(os.path.dirname(__file__),os.pardir,'script')
  
  energies = [20,50,80] 
  particles= ['pi0','pi-','pi+','e-','mu-']
  
  for particle in particles:
    for energie in energies:
      params = sim.Params()
      params.oldConfig = False
      params.killNeutrons = False
      params.RPCType = "Standard"
      params.AbsorberType = "Standard"
      params.physicsList = 'QGSP_BERT'
      params.nEvent = 5000
      params.seed = 0
      params.outputFileName = params.RPCType+'_'+params.AbsorberType+'_'+particle+'_'+str(energie)
      charged = sim.Particle()
      charged.particleName = particle
      charged.energy = energie;
      charged.momentumOption = 'gaus'
      charged.sigmaMomentum = 0.15
      charged.positionOption = 'gaus'
      charged.sigmaPos = 33
      params.particleList.append(charged)
      sim.launch( params )


