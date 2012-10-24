from sys import argv, exit
import struct
import random 
import time
import math
import socket
import threading
import colorsys

class LEDStrip( threading.Thread):
	
	def __init__(self, ip, port, length, reversed = False):
		self.ip = ip
		self.port = port
		self.length = length
		self.reversed = reversed
		self.led = bytearray(length*3+1)
		self.led[0] = 0xFF
		for i in range(length*3):
			self.led[i+1] = 0
		self.sock = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)
		threading.Thread.__init__(self)
		
	def run(self):
		print 'New LEDStrip created: ', self.ip, ':', self.port,' Length: ', self.length
		while True:
			self.sock.sendto(self.led, (self.ip, self.port))
			time.sleep(0.02)
			
	def setPixel(self, pixel, r, g, b):
		if self.reversed:
			pixel = self.length - pixel - 1
		self.led[pixel*3+1] = int(r/2)
		self.led[pixel*3+2] = int(g/2)
		self.led[pixel*3+3] = int(b/2)
		
	def setPixelHSV(self, pixel, h, s, v):
		r,g,b = colorsys.hsv_to_rgb(h, s, v)
		self.setPixel(pixel, r*255, g*255, b*255)
		
	def setPercent(self, percent, r, g, b):
		pixel = (self.length-1) * percent / 100
		self.setPixel(pixel, r, g, b)
		
	def setPercentHSV(self, percent, h, s, v):
		pixel = (self.length-1) * percent / 100
		self.setPixelHSV(pixel, h, s, v)
	
	def clear(self, r = 0, g = 0, b = 0):
		for pixel in range(self.length):
			self.setPixel(pixel, r, g, b)
			
	def decay(self):
		for pixel in range(self.length):
			self.led[pixel*3+1] = int(self.led[pixel*3+1] * 0.91)
			self.led[pixel*3+2] = int(self.led[pixel*3+2] * 0.91)
			self.led[pixel*3+3] = int(self.led[pixel*3+3] * 0.91)

	def getLength(self):
		return self.length
		
	def getValue(self):
		return self.r, self.g, self.b
			
class LEDStripCollection():
	length = 0
		
	pixel = list()
	pixelindex = list()
	strips = list()
	def add(self, strip):
		self.length = self.length + strip.getLength()
		self.strips.append(strip)
		for i in range(strip.getLength()):
			self.pixel.append(strip)
			self.pixelindex.append(i)
	
	def setPixel(self, pixel, r, g, b):
		self.pixel[pixel].setPixel(self.pixelindex[pixel], r, g, b)

	def setPixelHSV(self, pixel, h, s, v):
		self.pixel[pixel].setPixelHSV(self.pixelindex[pixel], h, s, v)		
	
	def setPercent(self, percent, r, g, b):
		pixel = (self.length-1) * percent / 100
		self.setPixel(pixel, r, g, b)

	def setPercentHSV(self, percent, h, s, v):
		pixel = (self.length-1) * percent / 100
		self.setPixelHSV(int(pixel), h, s, v)
		
	def clear(self, r = 0, g = 0, b = 0):
		for strip in strips:
			strip.clear(r,g,b)
			
		
	def decay(self):
		for strip in strips:
			strip.decay()
		
strips = list()
strip1 = LEDStrip('10.0.10.51', 1337, 44)
strip2 = LEDStrip('10.0.10.112', 1337, 82, True)
strip1.start()
strip2.start()
strips.append(strip1)
strips.append(strip2)
combined = LEDStripCollection()
combined.add(strip1)
combined.add(strip2)
#for i in range(100):
#	combined.setPercent(i,255,255,255)
time.sleep(1)
while True:
	for e in range(10000):
		for i in range(100):
			combined.setPercentHSV((i+e)%100, i/100.0,1,1)
		time.sleep(0.01)
	
	for e in range(10):
		for i in range(100):
			combined.decay()
			combined.setPercent(i, 255,0,0)
			time.sleep(0.01)
		for i in range(100):
			combined.decay()
			combined.setPercent(100-i, 255,0,0)
			time.sleep(0.01)

#	for e in range(1000):
#		for i in range(100):
#			combined.setPercentHSV((i+e)%100, i/100.0,1,1)
#		time.sleep(0.01)			
		
	for e in range(10):
		for i in range(100):
			combined.decay()
			combined.setPercent(i, 255,0,0)
			time.sleep(0.01)

while True:
#	for e in range(10000):
#		for strip in strips:
#			for i in range(100):
#				strip.setPercentHSV((i+e)%100, i/100.0,1,1)
#		time.sleep(0.011)
	
	for e in range(10):
		for i in range(100):
			for strip in strips:
				strip.decay()
				strip.setPercent(i, 255,0,0)
			time.sleep(0.01)
		for i in range(100):
			for strip in strips:
				strip.decay()
				strip.setPercent(100-i, 255,0,0)
			time.sleep(0.01)	