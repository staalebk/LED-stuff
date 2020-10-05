#!/usr/bin/env python3
from PIL import Image, ImageDraw
import json

WIDTH = 128
HEIGHT = 32

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
        im = Image.new("P", (WIDTH*16,HEIGHT*16), 0)
        #im = Image.new('RGB', (WIDTH*4, HEIGHT*4), colors[i])
        im.putpalette([0,0,0,
          16,5,0,
          32,11,0,
          48,16,0,
          64,21,0,
          80,27,0,
          96,32,0,
          112,37,0,
          128,43,0,
          144,48,0,
          160,53,0,
          176,58,0,
          192,64,0,
          208,69,0,
          224,74,0,
          240,80,0,
          255,85,0])
        draw = ImageDraw.Draw(im)
        for x in range(0,128):
            for y in range(0,32):
#                draw.ellipse([x*16,y*16,x*16+16,y*16+16], fill=10)
#                print(x*128+y)
#                print(frames[anim[i]][y*128+x])
                draw.ellipse([x*16,y*16,x*16+16,y*16+16], fill=frames[anim[i]-1][y*128+x])
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
