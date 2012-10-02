#!/usr/bin/env python
import socket, time

UDP_IP="192.168.7.41"
UDP_PORT=1337
print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT
data = bytearray(160*3)
sock = socket.socket( socket.AF_INET, # Internet
                      socket.SOCK_DGRAM ) # UDP
for i in range(0, 160*3):
  data[i] = 97
while True:
  for v in range(0, 128):
    for i in range(0, 160):
      data[i*3] = 0
      data[i*3 + 1] = v
      data[i*3 + 2] = 0
    print sock.sendto( data, (UDP_IP, UDP_PORT) )
    time.sleep(0.01)
  for v in range(0,128):
    for i in range(0, 160):
      data[i*3] = 0
      data[i*3 + 1] = 127-v
      data[i*3 + 2] = 0

    print sock.sendto( data, (UDP_IP, UDP_PORT) )
    time.sleep(0.01)
