#!/usr/bin/env python3
import array
from samplebase import SampleBase
from rgbmatrix import graphics
import dmdcolors

class Display(SampleBase):
    def __init__(self, width = 128, height = 32, *args, **kwargs):
        super(Display, self).__init__(*args, **kwargs)
        self.type = "Console"
        self.height = height
        self.width = width
        self.ascii_ramp = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,\"^`'. "
        self.ascii_ramp_rev = self.ascii_ramp[::-1]
        self.process()
        self.canvas = self.matrix.CreateFrameCanvas()

    def clearCanvas(self):
        for x in range(0, self.canvas.width):
            for y in range(0, self.canvas.height):
                self.canvas.SetPixel(x, y, 0,0,0)

    def get_repr(self, val):
        pos = int((val/255) * len(self.ascii_ramp_rev))
        return self.ascii_ramp_rev[pos]

    def display(self, canvas):
        self.clearCanvas()
        for x in range(0, self.height):
#            row = ""
            for y in range(0, self.width):
                val = canvas[x*self.width+y]
                self.canvas.SetPixel(y, x, dmdcolors.icp[val*3], dmdcolors.icp[val*3+1]*0.5, dmdcolors.icp[val*3+2])

#                row += self.get_repr(canvas[x*self.width + y])
#            print(row)
        self.canvas = self.matrix.SwapOnVSync(self.canvas)
        
