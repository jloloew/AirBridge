# AirBridge

See [https://github.com/jloloew/AirBridge](https://github.com/jloloew/AirBridge) for the most up-to-date version.

Bridging the air gap. Uses AM radio signals to exfiltrate data from an otherwise secure air gapped computer.

Just tune your radio to AM 1580 kHz, and put the antenna just over your computer's CPU. Tested on a mid-2014 13" retina MacBook Pro.

Requires [PyAudio](https://people.csail.mit.edu/hubert/pyaudio) for capture and parse.

Use `python capture.py "seconds"` to record audio saved as `output.wav`.

After compiling, use `./transmitter -s "string"` to transmit a string. For other methods of transmission, run `./transmitter` to see usage information.

Use `python parse.py "filename"` to parse the captured audio file.

Obviously, the audio must start recording before the signal starts to play.

Old demo video:
[![Demo video](https://img.youtube.com/vi/27C9zokH7Z8/maxresdefault.jpg)](https://youtu.be/27C9zokH7Z8 "Demo video")

UIUC CS 460 final project by jloew2 and foti2

