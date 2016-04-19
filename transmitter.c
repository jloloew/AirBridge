/*!
 * Transmit data via an audible AM radio signal.
 * 
 * Based off this MIT-licensed GitHub project:
 * https://github.com/fulldecent/system-bus-radio
 * 
 * (C) 2016 Justin Loew
 */

#include "transmitter.h"

#include <stdio.h>
#include <emmintrin.h>
#include <fcntl.h>
#include <mach/mach_traps.h>
#include <mach/mach_time.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#define FREQ_HI 2093 // C7
#define FREQ_LO 1047 // C6
#define FREQ_DIV 1568 // G6
#define BIT_INTERVAL 0.98
#define DIV_INTERVAL 0.02

#define STDIN_BUF_SIZE 100

static __m128i reg;
static __m128i reg_zero;
static __m128i reg_one;
static mach_port_t clock_port;
static mach_timespec_t remain;
static char verbose = TRUE;

static inline void square_am_signal(float time, float frequency)
{
	if (verbose) {
		printf("Playing / %0.3f seconds / %4.0f Hz\n", time, frequency);
	}
    uint64_t period = NSEC_PER_SEC / frequency;

    uint64_t start = mach_absolute_time();
    uint64_t end = start + time * NSEC_PER_SEC;

    while (mach_absolute_time() < end) {
        uint64_t mid = start + period / 2;
        uint64_t reset = start + period;
        while (mach_absolute_time() < mid) {
            _mm_stream_si128(&reg, reg_one);
            _mm_stream_si128(&reg, reg_zero);
        }
        clock_sleep_trap(clock_port, TIME_ABSOLUTE, reset / NSEC_PER_SEC,
				reset % NSEC_PER_SEC, &remain);
        start = reset;
    }
}

void transmit_string(const char *message, size_t len)
{
	// safety first
	if (!message) {
		return;
	}
	// transmit string
	for (size_t i = 0; i < len; i++) {
		// send each char in the message
		char c = message[i];
		printf("%c", c);
		for (int j = 0; j < 8; j++) {
			// send each bit of the char, one at a time
			// @TODO: switch to Machester coding to encode clock
			// insert a divider frequency to distinguish identical bits
			square_am_signal(DIV_INTERVAL, FREQ_DIV);
			
			float freq = (c & 0x80) ? FREQ_HI : FREQ_LO;
			c <<= 1;
			square_am_signal(BIT_INTERVAL, freq); // send the actual bit
		}
	}
}

static void _transmit_file_des(int fd)
{
	char buf[STDIN_BUF_SIZE] = { 0 };
	ssize_t size = read(fd, buf, sizeof(buf));
	while (size > 0) {
		// transmit one chunk at a time
		transmit_string(buf, size);
		// try to read some more
		size = read(STDIN_FILENO, buf, sizeof(buf));
	}
	// check for errors
	if (size < 0) {
		printf("\n\nError encountered while reading from stdin.\n");
		exit(EXIT_FAILURE);
	}
}

void transmit_stdin(void)
{
	printf("Reading from standard input. Start typing, finish with ^D.\n");
	_transmit_file_des(STDIN_FILENO);
	printf("Finished reading from standard input.\n");
}

void transmit_file(const char *filename)
{
	printf("Reading from file...\n");
	// safety first
	if (!filename) {
		return;
	}
	// open file
	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		printf("Error opening file %s\n", filename);
		exit(EXIT_FAILURE);
	}
	// read file
	_transmit_file_des(fd);
	close(fd);
}

/// Transmit Mary Had a Little Lamb.
void transmit_song(void)
{
	while (TRUE) {
        square_am_signal(0.400, 2673);
        square_am_signal(0.400, 2349);
        square_am_signal(0.400, 2093);
        square_am_signal(0.400, 2349);
        square_am_signal(0.400, 2673);
        square_am_signal(0.400, 2673);
        square_am_signal(0.790, 2673);
        square_am_signal(0.400, 2349);
        square_am_signal(0.400, 2349);
        square_am_signal(0.790, 2349);
        square_am_signal(0.400, 2673);
        square_am_signal(0.400, 3136);
        square_am_signal(0.790, 3136);
        square_am_signal(0.400, 2673);
        square_am_signal(0.400, 2349);
        square_am_signal(0.400, 2093);
        square_am_signal(0.400, 2349);
        square_am_signal(0.400, 2673);
        square_am_signal(0.400, 2673);
        square_am_signal(0.400, 2673);
        square_am_signal(0.400, 2673);
        square_am_signal(0.400, 2349);
        square_am_signal(0.400, 2349);
        square_am_signal(0.400, 2673);
        square_am_signal(0.400, 2349);
        square_am_signal(0.790, 2093);
    }
}

void transmitter_init(void)
{
	mach_timebase_info_data_t theTimeBaseInfo;
    mach_timebase_info(&theTimeBaseInfo);
    printf("TESTING TIME BASE: the following should be 1 / 1\n");
    printf("\tMach base: %u / %u nanoseconds\n",
			theTimeBaseInfo.numer, theTimeBaseInfo.denom);

    uint64_t start = mach_absolute_time();
    uint64_t end = mach_absolute_time();
	printf("TESTING TIME TO EXECUTE mach_absolute_time()\n");
	printf("\tResult: %lld nanoseconds\n", end - start);
	
	printf("\n");
	
	// init registers
    reg_zero = _mm_set_epi32( 0,  0,  0,  0);
    reg_one  = _mm_set_epi32(-1, -1, -1, -1);
}
