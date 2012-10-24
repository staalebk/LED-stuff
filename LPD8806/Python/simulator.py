import pygame, sys, random, socket, struct
from pygame.locals import *
from struct import *

pygame.init()

led_num = int(sys.argv[1])
udp_port = int(sys.argv[2])

udp_buffer_size = led_num*3

led_width = 3
led_height = 20
led_padding = 20

screen_width = led_num*led_width+led_padding*2
screen_height = led_height+led_padding*2


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("10.0.10.51", udp_port))


surf = pygame.display.set_mode((screen_width, screen_height));
pygame.display.set_caption('LED strip simulator')
udp_format = ""
for i in range(0, led_num*3):
        udp_format = udp_format + "B"

bgColor = pygame.Color(0,0,0)
print "yay"
while True:
        print "waiting"
        # Get data from UDP
        data, addr = sock.recvfrom(udp_buffer_size)
        print "got data"
        values = unpack(udp_format,data)

        surf.fill(bgColor)

        for i in range(led_num):
                c_r = values[i*3]*2
                c_g = values[i*3+1]*2
                c_b = values[i*3+2]*2

                pygame.draw.rect(surf, pygame.Color(c_r,c_g,c_b), (led_padding+i*led_width,led_padding,led_width,led_height))

        
        for event in pygame.event.get():
                if event.type == QUIT:
                        pygame.quit()
                        sys.exit()


        pygame.display.update()