import sys, math
from aubio import source, pitch, freqtomidi




if len(sys.argv) < 2:
    print "Usage: %s <filename> [samplerate]" % sys.argv[0]
    sys.exit(1)

filename = sys.argv[1]
downsample = 1
samplerate = 44100 / downsample
if len( sys.argv ) > 2: samplerate = int(sys.argv[2])

win_s = 4096 / downsample # fft size
hop_s = 512  / downsample # hop size

s = source(filename, samplerate, hop_s)
samplerate = s.samplerate

tolerance = 0.8


pitch_o = pitch("yin", win_s, hop_s, samplerate)
pitch_o.set_tolerance(tolerance)

pitches = []
confidences = []
total_frames = 0
while True:
	samples, read = s()
	pitch = pitch_o(samples)[0]
	pitch = int(round(pitch))
	confidence = pitch_o.get_confidence()
	if confidence < 0.6: pitch = 0.
	print "%f %f %f" % (total_frames / float(samplerate), pitch, confidence)
	pitches.append((total_frames/float(samplerate),pitch))
	confidences += [confidence]
	total_frames += read
	if read < hop_s: break

chunks = []
average_pitches = []

for i in range(len(pitches)):
	if pitches[i][1] == 0:
		del pitches[i]
	else:
		break

for i in reversed(range(len(pitches))):
	if pitches[i][1] == 0:
		del pitches[i]
	else:
		break


seconds = math.ceil(total_frames/float(samplerate))
start = pitches[0][0]



for i in range(int(pitches[len(pitches)-1][0]-start)+1):
	chunks.append([])


for pitch in pitches:
	if(pitch[0]==0):
		continue
	chunks[int(pitch[0]-start)].append(pitch[1])

for chunk in chunks:
	average_pitches.append(sum(chunk)/float(len(chunk)))

bits = []
for pitch in average_pitches:
	if 900 <= pitch < 1200:
		bits.append(0)
	if 1700 <= pitch <2100:
		bits.append(1) 

print bits
