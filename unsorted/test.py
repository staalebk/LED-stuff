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
for i in range(44*3):
	a[i+1] = 1;
i = 0;
while True:
	
	while(ser.inWaiting()):
		print ser.read(1);
	a[1+i*3-2] = 1;
	a[1+i*3+1] = 127;
	print ser.write(a);
	i += 1
	if(i == 43):
		a[i*3+1] = 1;
		a[i*3+1-2] = 1;
		i = 0;
	time.sleep(0.001);
ser.close();