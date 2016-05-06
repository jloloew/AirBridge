import sys, math
from test import goertzel
import wave
import pyaudio
import Queue
import numpy as np
if len(sys.argv) < 2:
    print "Usage: %s <filename> " % sys.argv[0]
    sys.exit(1)

filename = sys.argv[1]

w = wave.open(filename)
fs = w.getframerate()
width = w.getsampwidth()
chunkDuration = .2 #.2 second chunks
chunk = int(chunkDuration*fs)
window = np.blackman(chunk)

p = pyaudio.PyAudio()
stream = p.open(format = p.get_format_from_width(w.getsampwidth()), channels = w.getnchannels(),rate = fs, output=True)

#read .2 second chunk
data = w.readframes(chunk)
chunk_data = [] 
#find the frequencies of each chunk

print "Running calculations on wav file"
num = 0 
while data != '':
	print "Calculating Chunk " + str(num)
	stream.write(data)
	indata = np.array(wave.struct.unpack("%dh"%(len(data)/width),\
                                         data))
	freqs , results = goertzel(indata,fs, (1036,1058), (1567,1569), (2082,2104))

	chunk_data.append((freqs,results))
	
	data = w.readframes(chunk)
	num+=.2
stream.close()
p.terminate()

#finished getting data from chunks, now to parse the data

hi = []
lo = []
mid = []

#average first second of audio to get frequency baselines
for i in range (5):
	a = chunk_data[i][0]
	b = chunk_data[i][1]
	for j in range(len(a)):
		if a[j] > 1700:
			hi.append(b[j])
		elif a[j] < 1300:
			lo.append(b[j])
		else:
			mid.append(b[j])

hi_average = sum(hi)/float(len(hi))
lo_average = sum(lo)/float(len(lo))
mid_average = sum(mid)/float(len(mid))


"""
Determine the frequency in each .2 second chunk that has the highest amplitude increase from its average, then determine the frequency 
of that second of data by the median frequency of its 5 chunks
"""



#looks for start signal in last 3 seconds of audio
def signal_found(arr):
	lst = arr[-15:]
	first = 0
	second = 0
	third = 0
	for i in range(0,5):
		if lst[i]=="mid":
			first += 1
	for i in range(5,10):
		if lst[i]=="mid":
			second += 1
	for i in range(10,15):
		if lst[i]=="mid":
			third += 1

	if first >= 5 and second >= 5 and third >= 5:
		return True
	else:
		return False		

#gets freq of 1 second of audio
def get_freq(arr):
	lo_count = 0
	hi_count = 0
	mid_count = 0
	for i in arr:
		if i=="lo":
			lo_count+=1
		if i=="hi":
			hi_count+=1
		if i=="mid":
			mid_count+=1

	if mid_count > hi_count and mid_count > lo_count:
		return 2
		
	if lo_count>hi_count:
		return 0
	else:
		return 1


start = False
freq_list = []
offset = 0
bits = []
for i in range(5,len(chunk_data)):
	a = chunk_data[i][0]
	b = chunk_data[i][1]
	hi_amp = []
	lo_amp = []
	mid_amp = []
	#get averages for each freq
	for j in range(len(a)):
		if a[j] > 1700:
			hi_amp.append(b[j])
		elif a[j] < 1300:
			lo_amp.append(b[j])
		else:
			mid_amp.append(b[j])

	hi_av = sum(hi_amp)/float(len(hi_amp))
	lo_av = sum(lo_amp)/float(len(lo_amp))
	mid_av = sum(mid_amp)/float(len(mid_amp))
	
	#get freq of this chunk
	diff = [lo_av-lo_average,mid_av-mid_average,hi_av-hi_average]
	index = diff.index(max(diff))
	if(index==0):
		freq_list.append("lo")
	if(index==1):
		freq_list.append("mid")
	if(index==2):
		freq_list.append("hi")

	print(freq_list[len(freq_list)-1])	
	if len(freq_list) > 5:
		if start:
			if len(freq_list)%5 == offset:
				bit = get_freq(freq_list[-5:])
				if bit != 2:
					bits.append(bit)
				else:
					print "Stop Signal Detected"
					break
		elif len(freq_list) >= 15:
			if signal_found(freq_list):
				print "signal found"
				start = True
				offset = len(freq_list)%5
			
			
		
print bits 	
