# AirBridge
Bridging the air gap. Uses AM radio signals to exfiltrate data from a secure air gapped computer.

Just tune your radio to AM 1580 kHz, and put the antenna just over your computer's CPU. Tested on a mid-2014 13" retina MacBook Pro.

Requires [PyAudio](https://people.csail.mit.edu/hubert/pyaudio) for Capture and Parse 

use python capture.py "seconds" to record audio saved as "output.wav"

use python parse.py "filename" to parse audio

Audio must start recording before signal starts to play


UIUC CS460 Final Project by jloew2 and foti2
