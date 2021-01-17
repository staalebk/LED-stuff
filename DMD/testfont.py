#!/usr/bin/env python3
import fonts

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

for font in fonts.fonts:
    print(font)
    for i in range(0, 11):
        print_num(font, i)
