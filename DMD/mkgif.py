#!/usr/bin/env python3
from PIL import Image, ImageDraw
import json

WIDTH = 128
HEIGHT = 32
MULTI = 4
gamma_ramp = [
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255]
inv_gamma_ramp = []
for i in reversed(gamma_ramp):
    inv_gamma_ramp.append(255-i)
#print(inv_gamma_ramp)
igr = inv_gamma_ramp

colors = [(0,0,0),   (16,5,0),  (32,11,0),
          (48,16,0), (64,21,0), (80,27,0),
          (96,32,0), (112,37,0),(128,43,0),
          (144,48,0),(160,53,0),(176,58,0),
          (192,64,0),(208,69,0),(224,74,0),
          (240,80,0),(255,85,0)]

def draw(basename, frames, anim):
    print("Making " + basename)
    images = []
    for i in range(0,len(anim)):
        print("New frame: " + str(i))
        im = Image.new("P", (WIDTH*MULTI,HEIGHT*MULTI), 0)
        #im = Image.new('RGB', (WIDTH*4, HEIGHT*4), colors[i])
        im.putpalette([0,0,0,
          igr[16],igr[5],0,
          igr[32],igr[11],0,
          igr[48],igr[16],0,
          igr[64],igr[21],0,
          igr[80],igr[27],0,
          igr[96],igr[32],0,
          igr[112],igr[37],0,
          igr[128],igr[43],0,
          igr[144],igr[48],0,
#          igr[160],igr[53],255,
          16,16,16,
          igr[176],igr[58],0,
          igr[192],igr[64],0,
          igr[208],igr[69],0,
          igr[224],igr[74],0,
          igr[240],igr[80],0,
          igr[255],igr[85],0])
        draw = ImageDraw.Draw(im)
        for x in range(0,128):
            for y in range(0,32):
#                draw.ellipse([x*16,y*16,x*16+16,y*16+16], fill=10)
#                print(x*128+y)
#                print(frames[anim[i]][y*128+x])
                draw.ellipse([x*MULTI+1,y*MULTI+1,x*MULTI+MULTI-1,y*MULTI+MULTI-1], fill=frames[anim[i]-1][y*128+x])
        images.append(im)
    gif = []
    for image in images:
        gif.append(image.convert("P",palette=Image.ADAPTIVE))
    images[0].save('img/' + basename + '.gif', save_all=True, append_images=images[1:], optimize=False, duration=100, loop=0, quality=100)
#    gif[0].save('img/' + basename + '.gif',save_all=True,optimize=False,append_images=gif[1:], duration=50, loop=0)

with open('json/dmd.json') as json_file:
    clips = json.load(json_file)
    for clip in clips:
        name = clip['name']
        frames = clip['frames']
        anim = clip['animation']
        with open('json/anim/' + name + '.json') as anim_file:
            a = json.load(anim_file)
            draw(name, a['frames'], anim)
