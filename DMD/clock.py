#!/usr/bin/env python3
import fonts
import random
from display import Display
import array
import time

class Clock:
    """A simple class that can return a clock"""
    def __init__(self, seconds = True, font_name = "", width = 128, height = 32):
        if font_name == "":
            font_name = random.choice(list(fonts.fonts))
        self.font = font_name
        self.width = width
        self.height = height
        self.seconds = seconds

    def getTime(self):
        t = time.time()
        divider = ":"
        if (t%1) > 0.5:
            divider = " "
        text = time.strftime("%H" + divider + "%M")
        if self.seconds:
            text = time.strftime("%H" + divider + "%M" + divider + "%S")
#        cursor_x = int((self.width - (chars * FONT_WIDTH))/2)
#        cursor_y = int((self.height - FONT_HEIGHT)/2)
#        for face in range(0, chars):
#            number_txt = text[face]
#        print(text)
#        text = "823:48"
        return fonts.draw_text_center(self.font, text)

def print_num(font, num):
    number = fonts.fonts[font][num]
    for i in range(0, 18):
        row = ""
        for j in range(0,14):
            p = i*14 + j
            if number[p] == "0":
                row += " "
            elif number[p] == "1":
                row += "."
            elif number[p] == "2":
                row += "-"
            elif number[p] == "3":
                row += "+"
            elif number[p] == "4":
                row += "#"
            elif number[p] == "5":
                row += "@"
        print(row)

if __name__ == "__main__":
    c = Clock(False, "")
    d = Display()
    while True:
        canvas = c.getTime()
        d.display(canvas)
        print(c.font)
        time.sleep(0.1)
    
#for font in fonts.fonts:
#    print(font)
#    for i in range(0, 11):
#        print_num(font, i)
