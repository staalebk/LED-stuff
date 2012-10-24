import socket
import pyaudio
import sys
import numpy
import analyze

p=pyaudio.PyAudio()

rec = p.open(format = pyaudio.paInt16,
             channels = 1,
             rate = 44100,
             input = True,
             frames_per_buffer = 1024)

while True:
    data = rec.read(1024)
    samps = numpy.fromstring(data, dtype=numpy.int16)
    print analyse.loudness(samps), analyse.musical_detect_pitch(samps)