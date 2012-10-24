import serial
from sys import argv, exit
import struct
import random 
import time
import math
import socket

UDP_IP="10.0.10.51"
UDP_PORT=1337

sock = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)

a = bytearray(44*3+1);
a[0] = 0xFF;
for i in range(44*3):
	a[i+1] = 0;
i = 0;
rev = False;
while True:
	
	if not rev:
		a[1+i*3] += 8;
		a[1+(i+1)*3] += 16;
		a[1+(i+2)*3] += 32;
		a[1+(i+3)*3] = 127;
		print sock.sendto(a, (UDP_IP, UDP_PORT));
		a[1+i*3] -= 8;
		i += 1
	else:
		a[1+i*3] = 127;
		a[1+(i+1)*3] = 64;
		a[1+(i+2)*3] = 16;
		a[1+(i+3)*3] = 8;
		print sock.sendto(a, (UDP_IP, UDP_PORT));
		a[1+(i+3)*3] = 0;
		i -= 1
	if(i == 40):
		for i in range(44*3):
			a[i+1] = 0;
		rev = True
		i = 40
	if(i == 0):
		for i in range(44*3):
			a[i+1] = 0;
		rev = False
		i = 0
	time.sleep(0.01);