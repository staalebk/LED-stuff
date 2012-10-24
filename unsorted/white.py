import serial
from sys import argv, exit
import struct
import random 
import time
import math
import array

ser = serial.Serial('COM15', 250000, timeout=1)
#ser = serial.Serial('COM13', 57600, timeout=1)
a = bytearray(44*3+1);
a[0] = 0xFF;
for i in range(44*3):
	a[i+1] = 127;
while True:
	print ser.write(a);
	time.sleep(0.08);
ser.close();