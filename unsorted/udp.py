import serial
from sys import argv, exit
import struct
import random 
import time
import math
import socket

ser = serial.Serial('COM15', 250000, timeout=1)
UDP_IP="10.0.10.51"
UDP_PORT=1337

sock = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)
sock.bind( (UDP_IP, UDP_PORT) )

a = bytearray(44*3+1);
a[0] = 0xFF;
for i in range(44*3):
	a[i+1] = 0;
i = 0;
rev = False;
while True:
	
	while(ser.inWaiting()):
		print ser.read(1);
	data, addr = sock.recvfrom( 1024 )
#	print addr
#	print ser.write(data)
	ser.write(data)
#	time.sleep(0.01);
ser.close();