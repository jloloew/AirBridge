/*!
 * Transmit data via an audible AM radio signal.
 *
 * Based off this MIT-licensed GitHub project:
 * https://github.com/fulldecent/system-bus-radio
 *
 * (C) 2016 Justin Loew
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "transmitter.h"

/// Print usage and exit.
void print_usage(const char *my_name)
{
	printf("Usage:\n");
	printf("\t%s\t\tRead from standard input.\n", my_name);
	printf("\t%s file\t\t Transmit the contents of the named file.\n", my_name);
	printf("\t%s -s arg1 [arg2 ...]\t\tTransmit the given string(s).\n", my_name);
	printf("\t%s --song\t\t Play Mary Had a Little Lamb.\n", my_name);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	transmitter_init();
	transmitter_send_calibration();
	
	printf("Welcome to AirBridge!\n");
	printf("Please tune your radio to AM 1580 kHz and place the antenna over this computer's CPU.\n\n");
	
	// What do we need to transmit?
	if (argc == 1) {
		// transmit stdin
		transmit_stdin();
		return 0;
	} else if (argc == 2) {
		// check if we should play the default song instead
		if (!strncmp(argv[1], "--song", strlen("--song"))) {
			transmit_song();
			return 0;
		}
		
		// command-line arg is the name of a file to transmit
		transmit_file(argv[1]);
		return 0;
	} else {
		// transmit a string
		if (strncmp("-s", argv[1], 2)) {
			// transmit each arg, separated by a space
			transmit_string(argv[2], strlen(argv[2]));
			for (int i = 3; i < argc; i++) {
				transmit_string(" ", 1);
				transmit_string(argv[i], strlen(argv[i]));
			}
			return 0;
		}
	}
	// something went wrong somewhere
	print_usage(argv[0]);
}
