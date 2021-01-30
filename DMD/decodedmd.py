#!/usr/bin/env python3
from struct import *
import array
import time
from display import Display

class DmdFrame():
    def __init__(self, width, height, delay, data):
        self.x = width 
        self.y = height
        self.delay = delay
        self.canvas = array.array('B', [0] * 128 * 32 * 3)
        for x in range(0,width):
            for y in range(0,height):
                offset = (y*width + x)*2
                ab, gr = unpack("BB", data[offset:offset+2])
#                a = ab >> 8
#                b = ab & 0x0F
#                g = gr >> 8
                r = gr & 0x0F
#                print("RGBA: {} {} {} {}".format(r,g,b,a))
                self.canvas[(y*128+x)*3+0] = r*16
#                self.canvas[(y*128+x)*3+1] = g*16
#                self.canvas[(y*128+x)*3+2] = b*16

class DmdFile():
    def __init__(self, path):
        with open(path, "rb") as f:
            self.content = bytearray(f.read())
        name, size, data = self.parseChunk(self.content)
        self.frames = []
        if name != "DMD1":
            print("Error, unsupported format type")
            return
        while len(data) > 8:
            name, size, cdata = self.parseChunk(data)
            data = data[8+size:]
            print(name)
            if (name == "VERS"):
                self.version, = unpack(">I", cdata)
                print("Version found {}".format(self.version))
            if (name == "ANIM"):
                self.parseAnim(cdata)

    def parseInfo(self, data):
        width, height, unknown1, unknown2, unknown3, frames = unpack("IIIIII", data)
        print("Info: {} frames {}x{}, {} {} {}".format(frames, width, height, unknown1, unknown2, unknown3))
        self.width = width
        self.height = height
        self.framecnt = frames

    def parseFrame(self, data):
        delay = 2000
        while len(data) > 8:
            name, size, cdata = self.parseChunk(data)
            data = data[8+size:]
            if name == "TIME":
                delay, = unpack("I", cdata)
            if name == "DATA":
                frame = DmdFrame(self.width, self.height, delay, cdata)
                self.frames.append(frame)

    def parseAnim(self, data):
        while len(data) > 8:
            name, size, cdata = self.parseChunk(data)
            data = data[8+size:]
            if name == "INFO":
                self.parseInfo(cdata)
            if name == "FRAM":
                self.parseFrame(cdata)

    def parseChunk(self, chunk):
        name, size = unpack("4sI", chunk[:8])
        name = name.decode("utf-8")
        data = chunk[8:8+size]
        return (name, size, data)


        

if __name__ == "__main__":
    d = Display()
#    anim = DmdFile("/home/chiller/dmdgifs/dmd/sonicfaces.dmd")
    anim = DmdFile("/home/chiller/dmdgifs/dmd/mariokartwin03.dmd")
    for frame in anim.frames:
        d.displayRGB(frame.canvas)
        time.sleep(frame.delay/1000)
