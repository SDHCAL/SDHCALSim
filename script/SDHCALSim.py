#!/usr/bin/env python

import os

class Particle :
	def __init__(self) :
		self.particleName = "mu-"

		self.time = 0

		self.positionOption = "fixed"       #fixed uniform gaus cosmic
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
		self.physicsList = "FTFP_BERT"

		self.nEvent = 100
		self.seed = 0
		self.particleList = []
		self.rpcType = "normal"
		self.outputFileName = "output"



def launch(a) :

	pid = os.getpid()
	xmlFileName = str(pid) + '.xml'

	xmlFileContent = '''
	<config>
		<physicsList>'''+ a.physicsList +'''</physicsList>
		<nEvent>'''+ str(a.nEvent) +'''</nEvent>
		<seed>'''+ str(a.seed) +'''</seed>
		<rpcType>'''+ a.rpcType +'''</rpcType>
		<outputFileName>'''+ a.outputFileName +'''</outputFileName>
		<guns>'''

	for particle in a.particleList :
		xmlFileContent = xmlFileContent + '''
			<particle>
					<pdgID>''' + particle.particleName + '''</pdgID>
					<time>''' + str(particle.time) + '''</time>
					<energy type="''' + particle.energyDistribution+ '''" sigma="''' + str(particle.sigmaEnergy) + '''" min="''' + str(particle.minEnergy) + '''" max="''' + str(particle.maxEnergy) + '''">''' + str(particle.energy) + '''</energy>
					<position type="''' + particle.positionOption + '''" delta="''' + str(particle.uniformDeltaPos) + '''" sigma="''' + str(particle.sigmaPos) + '''">''' + str(particle.positionX) + ''' ''' + str(particle.positionY) + ''' ''' + str(particle.positionZ) + '''</position> 
					<momentum type="''' + particle.momentumOption + '''" sigma="''' + str(particle.sigmaMomentum) + '''">''' + str(particle.momentumPhi) + ''' ''' + str(particle.momentumTheta) + '''</momentum>
			</particle>'''

	xmlFileContent = xmlFileContent + '''
		</guns>
	</config>'''

	xmlFile = open(xmlFileName , 'w')
	xmlFile.write(xmlFileContent)
	xmlFile.close()


	simuExe = os.environ["SIMEXE"]

	os.system(simuExe + ' ' + xmlFileName)

	os.system('rm ' + xmlFileName)
