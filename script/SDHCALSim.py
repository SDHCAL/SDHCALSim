#!/usr/bin/env python

import os

class Particle :
	def __init__(self) :
		self.particleName = "mu-"

		self.time = 0
		
		self.cosmic = False

		self.positionOption = "fixed"       #fixed uniform gaus
		self.positionX = 0                  #0 is calorimeter center
		self.positionY = 0
		self.positionZ = -20                #0 is calorimeter begin
		self.uniformDeltaPos = 0            #if uniform
		self.sigmaPos = 0                   #if gaus

		self.momentumOption = "fixed"       #fixed gaus
		self.momentumPhi = 0                #
		self.momentumTheta = 0              #
		self.sigmaMomentum = 0              #if gaus

		self.energyDistribution = "fixed"   #fixed gaus uniform forNN
		self.energy = 50                    #if fixed or gaus
		self.sigmaEnergy = 0                #if gaus
		self.minEnergy = 1                  #if uniform or forNN
		self.maxEnergy = 120                #if uniform or forNN


class Params :
  def __init__(self) :
    self.MultiThread = True
    self.physicsList = "FTFP_BERT"
    self.nEvent = 100
    self.seed = 0
    self.particleList = []
    self.rpcType = "normal"
    self.oldConfig = False
    self.outputFileName = "output"
    self.killNeutrons = False



def particleConfig(particle) :
	cosmicBool = "true"
	if not particle.cosmic :
		cosmicBool = "false"
			
		return '''
			{
				"particleName" : "''' + particle.particleName + '''",
				"cosmic"   : ''' + cosmicBool + ''',		
				"energy"   : { "option" : "''' + particle.energyDistribution+ '''" , "value" : ''' + str(particle.energy) + ''' , "sigma" : ''' + str(particle.sigmaEnergy) + ''' , "min" : ''' + str(particle.minEnergy) + ''' , "max" : ''' + str(particle.maxEnergy) + ''' },	
				"vertex"   : { "time" : ''' + str(particle.time) + ''' , "option" : "''' + particle.positionOption + '''" , "position" : {"x":''' + str(particle.positionX) + ''', "y":''' + str(particle.positionY) + ''', "z":''' + str(particle.positionZ) + '''} , "delta" : ''' + str(particle.uniformDeltaPos) + ''' , "sigma" : ''' + str(particle.sigmaPos) + ''' },
				"momentum" : { "option" : "''' + particle.momentumOption + '''" , "direction" : {"phi": ''' + str(particle.momentumPhi) + ''', "theta" : ''' + str(particle.momentumTheta) + '''} , "sigma" : ''' + str(particle.sigmaMomentum) + ''' }
			}'''

def launch(a) :

  pid = os.getpid()
  jsonFileName = str(pid) + '.json'

  oldConfigBool = "false"
  if a.oldConfig :
    oldConfigBool = "true"

  killNeutronsBool = "false"
  if a.killNeutrons : 
    killNeutronsBool = "true"
  
  MultiThreadBool = "false"
  if a.MultiThread : 
    MultiThreadBool = "true"

  jsonFileContent = '''
  {
    "outputFileName" : "'''+ a.outputFileName +'''",
    "physicsList" : "'''+ a.physicsList +'''",
    "nEvents" : '''+ str(a.nEvent) +''',
    "seed" : '''+ str(a.seed) +''',
    "killNeutrons" : ''' + killNeutronsBool + ''',
    "MultiThread" : ''' + MultiThreadBool + ''',
    "detectorConfig" :
    {
      "rpcType" : "'''+ a.rpcType +'''",
      "oldConfig" : ''' + oldConfigBool + '''
    },

    "particuleGuns" :
    ['''


  for particle in a.particleList[:-1] :
    jsonFileContent = jsonFileContent + particleConfig(particle) + ','

  jsonFileContent = jsonFileContent + particleConfig(a.particleList[-1])


  jsonFileContent = jsonFileContent + '''
    ]
  }'''


  jsonFile = open(jsonFileName , 'w')
  jsonFile.write(jsonFileContent)
  jsonFile.close()


  simuExe = os.environ["SIMEXE"]

  os.system(simuExe + ' ' + jsonFileName)

  os.system('rm ' + jsonFileName)
