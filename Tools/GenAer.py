import numpy as np
import math
import copy
import matplotlib.pyplot as plt
from PyAedatTools.ImportAedat import ImportAedat
from PyAedatTools.ExportAedat2Polarity import ExportAedat2Polarity

class Mask:
	
	def __init__(self, size=[0,0], centre=[0,0]):
		self.size = size
		self.vals = np.zeros(size)
		self.centre = centre
		
		# y, x = np.ogrid[0:512,0:512] # x and y indices of pixels
# y.shape, x.shape
# ((512, 1), (1, 512))
# centerx, centery = (256, 256) # center of the image
# mask = ((y - centery)**2 + (x - centerx)**2) > 230**2 # circle
		
class Simulation:
	
	def __init__(self, arrayDim=[0,0], startPos=[0,0], endPos=[0,0], velocity=0, noise=0, jitter=0):
		self.mask = Mask([0,0],[0,0])
		self.startPos = startPos
		self.endPos = endPos
		self.pixelArray = np.zeros(arrayDim)
		self.prevPixelArray = np.zeros(arrayDim)
		self.velocity = velocity #pixels/ms
		self.noise = noise
		self.jitter = jitter
		self.T = 0 #duration of sime
		self.t = 0 #current sim time
		self.dt = 0 #timestep
		
		self.aedat = {}
		self.aedat['data'] = {}
		self.aedat['data']['polarity'] = {}
		self.aedat['data']['polarity']['timeStamp'] = []
		self.aedat['data']['polarity']['x'] = []
		self.aedat['data']['polarity']['y'] = []
		self.aedat['data']['polarity']['polarity'] = []
		
		self.aedat['exportParams'] = {}
		self.aedat['exportParams']['filePath'] = "C:\\Users\\JW\\DVS-gazetracking\\groundtruth\\recordings\\movingBar2.aedat"
		
	def run(self, cycles=40):
		
		if self.velocity == 0:
			print("Please set a value for velocity")
			return
			
		elif self.startPos >= self.endPos:
			print("Please enter correct start and end positions")
			return
		
		else:
			self.dt = 1000/self.velocity #NEED TO FIX SO CAN MOVE IN DIRECTIONS OTHER THAN HORIZONTAL
			self.T = (self.endPos[1] - self.startPos[1])*self.dt #NEED TO FIX
			
			self.t = 0
			f = open("circle-groundtruth-ds.txt", "w+")
			for cycle in range(cycles):
				print(self.t)
				self.mask.centre = copy.copy(self.startPos)
				simTime = 0
				self.diffImg = np.zeros(self.pixelArray.shape)
				self.pixelArray = np.zeros(self.pixelArray.shape)
				
				#fig, ax = plt.subplots()
				while simTime < self.T:
					#data = [int(self.t), int(self.mask.centre[1]), int(self.mask.centre[0])]
					data = [int(self.t), int(self.mask.centre[1]), 0]
					if ((self.mask.centre[1] >= 0) and (self.mask.centre[1] < 240)): #should change 240
						if (self.mask.centre[1] % 20 == 0):
							f.write(','.join(str(i) for i in data))
							f.write('\n')
					self.prevPixelArray = copy.copy(self.pixelArray)
					self.mask.centre[1] += 1
					self.projectMask()
					self.diffImg = self.pixelArray - self.prevPixelArray
					self.t += self.dt
					simTime += self.dt
					#ax.cla()
					#ax.imshow(self.diffImg)
					#plt.pause(0.05)
					#self.generateEvents()
			f.close()		
			#self.outputAedat()
					
	def projectMask(self):
		
			a = self.mask.centre[0]
			b = self.mask.centre[1]
			n = self.pixelArray.shape[0]
			m = self.pixelArray.shape[1]
			r = 8

			y,x = np.ogrid[-a:n-a, -b:m-b]

			mask = x*x + y*y <= r*r
			#mask = ((x+3)*y == x+3)
			mask[:,:] = 0
			mask[:,b-r:b+r] = 1
			
			self.pixelArray[:,:] = 0
			self.pixelArray[mask] = 1
				
	def generateEvents(self):
	
		addr = np.where(self.diffImg != 0)
		#print(addr)
		#if len(addr[0][0]) > 0:
		x = addr[1]
		y = addr[0]
		pol = self.diffImg[addr] > 0
		#print(pol)
		#print(self.t)
		#print(self.dt)
		ts = np.random.triangular(self.t-self.dt/2, self.t, self.t+self.dt/2, len(x))
		ts = [math.floor(x) for x in ts]
		#print(ts)
		if len(x)>0:
			ts,x,y,pol = (list(t) for t in zip(*sorted(zip(ts, x, y, pol))))

		#print(ts)
		self.aedat['data']['polarity']['timeStamp'].extend(ts)
		self.aedat['data']['polarity']['x'].extend(x)
		self.aedat['data']['polarity']['y'].extend(y)
		self.aedat['data']['polarity']['polarity'].extend(pol)
		
		#print(len(self.aedat['data']['polarity']['polarity']))

	def outputAedat(self):
		ExportAedat2Polarity(self.aedat)

		
			
		

sim = Simulation(arrayDim=[180,240], startPos=[90,-20], endPos=[90,260], velocity=1)
sim.run()