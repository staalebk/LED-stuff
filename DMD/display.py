#!/usr/bin/env python3
import array

class Display:
    def __init__(self, width = 128, height = 32):
        self.type = "Console"
        self.height = height
        self.width = width
        self.ascii_ramp = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,\"^`'. "
        self.ascii_ramp_rev = self.ascii_ramp[::-1]

    def get_repr(self, val):
        pos = int((val/255) * len(self.ascii_ramp_rev))
        return self.ascii_ramp_rev[pos]

    def display(self, canvas):
        for x in range(0, self.height):
            row = ""
            for y in range(0, self.width):
                row += self.get_repr(canvas[x*self.width + y])
            print(row)
        
