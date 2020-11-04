#!/usr/bin/env python
from samplebase import SampleBase
import json, random, time, sys
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
          2,2,2,
          igr[176],igr[58],0,
          igr[192],igr[64],0,
          igr[208],igr[69],0,
          igr[224],igr[74],0,
          igr[240],igr[80],0,
          igr[255],igr[85],0]


class DMD(SampleBase):
    def __init__(self, *args, **kwargs):
        super(DMD, self).__init__(*args, **kwargs)

    def run(self):
        canvas = self.matrix.CreateFrameCanvas()
        while True:
            # Select animation
            with open('json/dmd.json') as dmd_list:
                animations = json.load(dmd_list)
                animations = filter(lambda x: "AVENG" in x['name'], animations)
                animation = random.choice(animations)
                print(animation['name'])
                animation_file = 'json/anim/' + animation['name'] + '.json'
                with open(animation_file) as anim_file:
                    anim = json.load(anim_file)
                    for frame_n, time_n in zip(animation['animation'], animation['time']):
                        frame = anim['frames'][frame_n - 1]
                        for x in range(0, self.matrix.width):
                            for y in range(0, canvas.height):
                                pixel = frame[x+y*canvas.width]
                                canvas.SetPixel(x, y, icp[pixel*3+0], icp[pixel*3+1], icp[pixel*3+2])
                        canvas = self.matrix.SwapOnVSync(canvas)
                        if time_n > 100:
                            time_n = time_n * 5
                        sleep_s = time_n/1000.0
                        print(sleep_s)
                        time.sleep(sleep_s)
#                for x in range(0, self.matrix.width):
#                    for y in range(0, canvas.height):
#                        canvas.SetPixel(x, y, 0, 0, 0)
#                canvas = self.matrix.SwapOnVSync(canvas)


# Main function
if __name__ == "__main__":
    simple_square = DMD()
    if (not simple_square.process()):
        simple_square.print_help()
