#!/usr/bin/env python3
from samplebase import SampleBase
from rgbmatrix import graphics
import json, random, time, sys, math, threading, os
from enum import Enum, auto

class State(Enum):
    IDLE = auto()
    TEXT = auto()
    ANIM = auto()
    ANIM_IN = auto()
    ANIM_OUT = auto()
    ROLL_IN = auto()
    ROLL_OUT = auto()


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
igr = inv_gamma_ramp
#igr = gamma_ramp
icp = [0,0,0,
          igr[16],igr[5],0,
          igr[32],igr[11],0,
          igr[48],igr[16],0,
          igr[64],igr[21],0,
          igr[80],igr[27],0,
          igr[96],igr[32],0,
          igr[112],igr[37],0,
          igr[128],igr[43],0,
          igr[144],igr[48],0,
          0,0,0,
          igr[176],igr[58],0,
          igr[192],igr[64],0,
          igr[208],igr[69],0,
          igr[224],igr[74],0,
          igr[240],igr[80],0,
          igr[255],igr[85],0]
lin = [0,0,0,
      16,5,0,
      32,11,0,
      48,16,0,
      64,21,0,
      80,27,0,
      96,32,0,
      112,37,0,
      128,43,0,
      144,48,0,
      0,0,0,
      176,58,0,
      192,64,0,
      208,69,0,
      224,74,0,
      240,80,0,
      255,85,0]

def gamma(x):
    g = 2 # Gamma
    return int(pow(x,g) * 254 + 0.5) + 1

def gammag(x):
    g = 2 # Gamma
    return int(pow(x,g) * 254 + 0.5) + 1

step = 1.0/14
stepg = 1.0/20
ramp = [0,0,0, # 0
      gamma(step*1),gammag(stepg*1),0, # 1
      gamma(step*2),gammag(stepg*2),0, # 2
      gamma(step*3),gammag(stepg*3),0, # 3
      gamma(step*4),gammag(stepg*4),0, # 4
      gamma(step*5),gammag(stepg*5),0, # 5
      gamma(step*6),gammag(stepg*6),0, # 6
      gamma(step*7),gammag(stepg*7),0, # 7
      gamma(step*8),gammag(stepg*8),0, # 8
      gamma(step*9),gammag(stepg*9),0, # 9
      0,0,0, # 10 NOT USED
      gamma(step*10),gammag(stepg*10),0, # 11
      gamma(step*11),gammag(stepg*11),0, # 12
      gamma(step*12),gammag(stepg*12),0, # 13
      gamma(step*13),gammag(stepg*13),0, # 14
      gamma(step*14),gammag(stepg*14),0] # 15

step = 1.0/4
stepg = 1.0/5
reduced = [0,0,0, # 0
      gamma(step*2),gammag(stepg*2),0, # 1
      128,128,128, # 2
      gamma(step*3),gammag(stepg*3),0, # 3
      128,128,128, # 2
      128,128,128, # 2
      128,128,128, # 2
      128,128,128, # 2
      128,128,128, # 2
      128,128,128, # 2
      0,0,0, # 10 NOT USED
      128,128,128, # 2
      128,128,128, # 2
      128,128,128, # 2
      128,128,128, # 2
      gamma(step*4),gammag(stepg*4),0] # 15

#icp = reduced
#font = graphics.Font()
#font.LoadFont("../../dmd/rpi-rgb-led-matrix/fonts/7x13.bdf")
#font.LoadFont("../../dmd/rpi-rgb-led-matrix/fonts/10x20.bdf")
#font.LoadFont("font/ui.bdf")
#font.LoadFont("../../dmd/rpi-rgb-led-matrix/fonts/tom-thumb.bdf")
#font.LoadFont("../../dmd/rpi-rgb-led-matrix/fonts/helvR12.bdf")
#font.LoadFont("../../dmd/rpi-rgb-led-matrix/fonts/.bdf")
#font.LoadFont("../../dmd/rpi-rgb-led-matrix/fonts/")
#font.LoadFont("../../dmd/rpi-rgb-led-matrix/fonts/clR6x12.bdf")
#font.LoadFont("../../dmd/rpi-rgb-led-matrix/fonts/9x18B.bdf")
#font.LoadFont("font/leggie-18b.bdf")
#font.LoadFont("font/nethack12B.bdf")
#font.LoadFont("font/galacticagridspace.bdf")
#font.LoadFont("font/orangejuice.bdf")
#font.LoadFont("font/airstrike.bdf")
#font.LoadFont("font/snacker.bdf")
#font.LoadFont("font/.bdf")
#font.LoadFont("font/monsters.bdf")
#font.LoadFont("font/fedesc.bdf")
#font.LoadFont("font/cloister.bdf")
#font.LoadFont("font/sfsports.bdf")
#font.LoadFont("font/metalord.bdf")
#font.LoadFont("font/army.bdf")
#font.LoadFont("font/raider.bdf")
#font.LoadFont("font/bold.bdf")
#font.LoadFont("font/life.bdf")
#font.LoadFont("font/transformers.bdf")
#font.LoadFont("font/alba.bdf")
for x in range(0, 16):
    print(str(x) + ":\t" + str(icp[x*3]) + "\t" + str(icp[x*3+1]))

def testPattern(canvas, t = 0):
    for x in range(0, canvas.width):
        for y in range(0, canvas.height):
#            pixel = int(abs(math.sin(y/8.0+t))*16)
            pixel = int((x/7))%16
            canvas.SetPixel(x, y, icp[pixel*3+0], icp[pixel*3+1]*0.5, icp[pixel*3+2])
    return canvas

def clearCanvas(canvas):
    for x in range(0, canvas.width):
        for y in range(0, canvas.height):
            canvas.SetPixel(x, y, 0,0,0)
    return canvas

def easeOutCubic(x):
    return 1 - pow(1 - x, 3)

class Text():
    def __init__(self, fontName):
        self.font = graphics.Font()
        self.font.LoadFont(fontName)

    def drawText(self, canvas, text, posX, posY):
        fontb = 1
        graphics.DrawText(canvas, self.font, posX+1, posY+1, graphics.Color(icp[fontb*3],icp[fontb*3+1]*0.5,icp[fontb*3+2]), text)
        fontb = 16
        graphics.DrawText(canvas, self.font, posX, posY, graphics.Color(icp[fontb*3],icp[fontb*3+1]*0.5,icp[fontb*3+2]), text)
        return canvas
        

class Animation():
    def __init__(self, name, frames, animation, atime, mystery):
        print("Loading " + name + "\t" + str(mystery[3]) + "\t" + str(mystery[9]))
        animation_file = 'json/anim/' + name + '.json'
        with open(animation_file) as anim_file:
            self.anim = json.load(anim_file)
        self.frame = 0
        self.isDone = False
        self.animation = animation
        self.atime = atime
        self.mystery = mystery
        self.next_frame = 0
        colors = [0] * 16
        for frame in self.anim['frames']:
            for pixel in frame:
                colors[pixel] += 1
        fullColors = [2, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14]
        full = False
        for f in fullColors:
            if colors[f] != 0:
                full = True
        if full:
#            print("Full palette")
            self.palette = icp
        else:
#            print("Reduced palette")
            self.palette = reduced
#        print(colors)

    def getNextFrame(self, canvas):
        if self.isDone:
            return canvas
        if self.next_frame == 0:
            time_n = self.atime[0]
            if time_n > 100:
                print(time_n)
                time_n = time_n * 5
            self.next_frame = time.perf_counter() + (time_n/1000)*1.5
        if time.perf_counter() > self.next_frame:
            self.frame += 1
            if self.frame == len(self.animation) or self.frame == len(self.atime):
                self.isDone = True
                return canvas
            time_n = self.atime[self.frame]
            if time_n > 100:
#                print(time_n)
                time_n = time_n * 2
            self.next_frame = time.perf_counter() + (time_n/1000)*1.5
        frame_n = self.animation[self.frame] - 1
        time_n = self.atime[self.frame]
        frame = self.anim['frames'][frame_n]
        for x in range(0, canvas.width):
            for y in range(0, canvas.height):
                pixel = frame[x+y*canvas.width]
                if pixel != 10:
                    canvas.SetPixel(x, y, self.palette[pixel*3+0], self.palette[pixel*3+1]*0.5, self.palette[pixel*3+2])
        return canvas

class KeyboardThread(threading.Thread):

    def __init__(self, input_cbk = None, name='keyboard-input-thread'):
        self.input_cbk = input_cbk
        super(KeyboardThread, self).__init__(name=name)
        self.start()

    def run(self):
        while True:
            self.input_cbk(input()) #waits to get input + Return


class DMD(SampleBase):
    def __init__(self, *args, **kwargs):
        super(DMD, self).__init__(*args, **kwargs)
        self.ani = 0
        self.state = State.IDLE
        self.nextState = time.perf_counter()
        with open('json/dmd.json') as dmd_list:
            self.animations = json.load(dmd_list)
#            self.animations = list(filter(lambda x: "24_" in x['name'], self.animations))

    def run(self):
#        canv = self.matrix
#        animation = random.choice(self.animations)
#        self.a = Animation(animation['name'], animation['frames'], animation['animation'], animation['time'], animation['mystery'])
        t = Text("font/cloister.bdf")
        canvas = self.matrix.CreateFrameCanvas()
        while True:
            clearCanvas(canvas)
            if self.state == State.IDLE:
                if time.perf_counter() > self.nextState:
                    self.state = random.choices(population=[State.TEXT, State.ANIM], weights=[0.2,0.8])[0]
                    if self.state == State.TEXT:
                        self.nextState = time.perf_counter() + 5
                    if self.state == State.ANIM:
                        animation = random.choice(self.animations)
                        self.a = Animation(animation['name'], animation['frames'], animation['animation'], animation['time'], animation['mystery'])
                    continue
            if self.state == State.TEXT:
                canvas = t.drawText(canvas, "TSLA $429.90", 0, 28)
                if time.perf_counter() > self.nextState:
                    self.state = State.IDLE
                    self.nextState = time.perf_counter() + 0.5
                    continue
            if self.state == State.ANIM:
                if self.a.isDone:
                    self.state = State.IDLE
                    self.nextState = time.perf_counter() + 0.5
                    continue
#                animation = self.animations[self.ani]
#                self.ani += 1
#                self.a = Animation(animation['name'], animation['frames'], animation['animation'], animation['time'], animation['mystery'])
#            canvas = clearCanvas(canvas) 
#            canvas = t.drawText(canvas, "TSLA $429.90", (1-easeOutCubic(time.perf_counter()%1))*128, 28)
#            canvas = t.drawText(canvas, "TSLA $429.90", 0, 28)
                canvas = self.a.getNextFrame(canvas)
#            canvas = testPattern(canvas)
#            if self.a.isDone:
#                continue
            canvas = self.matrix.SwapOnVSync(canvas)

    def keypress(self, inp):
        if inp == 'x':
            os._exit(0)
        elif inp == '':
            self.ani += 1
            self.a.isDone = True
        elif inp == 'f':
            print("Change font!")
        print('input! ', inp, ' ', self.ani)



# Main function
if __name__ == "__main__":
    dmd = DMD()
    kthread = KeyboardThread(dmd.keypress)
    if (not dmd.process()):
        dmd.print_help()
