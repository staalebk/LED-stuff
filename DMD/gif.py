#!/usr/bin/env python3
"""
Displays 32x32 animated gif on Adafruit RGB LED Matrix.
Running this code assumes that you have followed the instructions and installed
the dependencies as described in the README file:
https://github.com/poemusica/rpi-matrix-gif/blob/master/README.md
Requires rgbmatrix.so to be present in the same directory as this script.
Gif extraction code copied from BigglesZX's github gist (thank you!):
https://gist.github.com/BigglesZX/4016539
"""


from PIL import Image
import os
import time
import array
import random
import os
from display import Display


def analyseImage(path):
    """
    Pre-process pass over the image to determine the mode (full or additive).
    Necessary as assessing single frames isn't reliable. Need to know the mode 
    before processing all frames.
    """
    im = Image.open(path)
    results = {
        'size': im.size,
        'mode': 'full',
    }
    try:
        while True:
            if im.tile:
                tile = im.tile[0]
                update_region = tile[1]
                update_region_dimensions = update_region[2:]
                if update_region_dimensions != im.size:
                    results['mode'] = 'partial'
                    break
            im.seek(im.tell() + 1)
    except EOFError:
        pass
    return results


def processImage(path):
    """
    Iterate the GIF, extracting each frame.
    """
    mode = analyseImage(path)['mode']
    
    im = Image.open(path)

    frames = []

    i = 0
    p = im.getpalette()
    last_frame = im.convert('RGBA')
    
    try:
        while True:            
            '''
            If the GIF uses local colour tables, each frame will have its own palette.
            If not, we need to apply the global palette to the new frame.
            '''
            if not im.getpalette():
                im.putpalette(p)
            
            new_frame = Image.new('RGBA', im.size)
            
            '''
            Is this file a "partial"-mode GIF where frames update a region of a different size to the entire image?
            If so, we need to construct the new frame by pasting it on top of the preceding frames.
            '''
            if mode == 'partial':
                new_frame.paste(last_frame)
            
            new_frame.paste(im, (0,0), im.convert('RGBA'))
            new_frame.duration = im.info['duration']
            frames.append(new_frame)

            i += 1
            last_frame = new_frame
            im.seek(im.tell() + 1)
    except EOFError:
        pass

    return frames

def main():
    d = Display()
    while True:
#        for file in os.listdir("gifs/gif/"):
        files = os.listdir("gifs/gif/")
        while True:
            file = random.choice(files)
#        for file in os.listdir("."):
            filename = os.fsdecode(file)
            if filename.endswith(".gif"):# and "mario" in filename:
#                print(filename)
                frames = processImage('gifs/gif/'+filename)
#                frames = processImage(filename)
                for frame in frames:
                    px = frame.load()
                    canvas = array.array('B', [0] * 128 * 32 * 3)
                    for y in range(0, 32):
                        for x in range(0,128):
                            try:
                                r, g, b, a = px[x,y]
                                canvas[(y*128+x)*3+0] = r
                                canvas[(y*128+x)*3+1] = g
                                canvas[(y*128+x)*3+2] = b
                            except:
                                pass
                    d.displayRGB(canvas)
                    duration = (frame.duration/1000)-0.02
                    if duration < 0:
                        duration = 0
#                    print(int(duration*1000))
                    time.sleep(duration)


    

if __name__ == "__main__":
    main()

