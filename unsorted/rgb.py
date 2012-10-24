import serial
from sys import argv, exit
import struct
import random 
import time
import math

#ser = serial.Serial('COM13', 57600, timeout=1)
ser = serial.Serial('COM15', 250000, timeout=1)
a = bytearray(44*3+1);
a[0] = 0xFF;
c = 0
while True:
	for i in range(44*3):
		a[i+1] = 1;
	for i in range(44):
		a[1+i*3+c] = 127;
	print ser.write(a);
	c += 1;
	if (c == 3):
		c = 0
	time.sleep(0.01);
ser.close();