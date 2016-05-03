import pyaudio
import wave
import sys


if(len(sys.argv)!=2):
	print "Please enter the number of second for which to record."
	exit()

RATE = 44100
FORMAT = pyaudio.paInt16
CHUNK = 1024
CHANNELS = 2
SECONDS = int(sys.argv[1])
OUTPUT = "output.wav"

print SECONDS

p = pyaudio.PyAudio()


stream = p.open(format=FORMAT,
                channels=CHANNELS,
                rate=RATE,
                input=True,
                frames_per_buffer=CHUNK)

print("* recording")

frames = []

for i in xrange(0, int(RATE / CHUNK * SECONDS)):
    data = stream.read(CHUNK)
    frames.append(data)

print("* done recording")

stream.stop_stream()
stream.close()
p.terminate()

wf = wave.open(OUTPUT, 'wb')
wf.setnchannels(CHANNELS)
wf.setsampwidth(p.get_sample_size(FORMAT))
wf.setframerate(RATE)
wf.writeframes(b''.join(frames))
wf.close()


