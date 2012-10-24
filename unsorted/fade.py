import serial
from sys import argv, exit
import struct
import random 
import time
import math

ser = serial.Serial('COM15', 250000, timeout=1)
a = bytearray(44*3+1);
a[0] = 0xFF;
c = 1;
while True:
	for i in range(44*3):
		a[i+1] = c;
	print ser.write(a);
	time.sleep(0.01);
	c += 1
	if(c == 128):
		c = 1;
ser.close();