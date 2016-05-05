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


typedef enum {
	TransmitterModePrintUsage,
	TransmitterModeSendStdin,
	TransmitterModeSendFile,
	TransmitterModeSendStrings,
	TransmitterModePlayDemo
} TransmitterMode;

static void print_usage(const char *my_name);
static TransmitterMode parse_args(int argc, char *argv[]);


/// Print usage and exit.
static void print_usage(const char *my_name)
{
	printf("Usage:\n");
	printf("  %s [-h | --help]         Print this help message and exit.\n", my_name);
	printf("  %s -f -                  Read from standard input.\n", my_name);
	printf("  %s -f file               Transmit the contents of the named file.\n", my_name);
	printf("  %s -s str1 [str2 ...]    Transmit the given string(s).\n", my_name);
	printf("  %s --song                Play Mary Had a Little Lamb.\n", my_name);
	exit(EXIT_FAILURE);
}

static TransmitterMode parse_args(int argc, char *argv[])
{
	char *flag_str = "";
	if (argc == 1) {
		// no args given
		return TransmitterModePrintUsage;
	} else if (argc == 2) {
		// check for help flag
		flag_str = "-h";
		if (!strncmp(argv[1], flag_str, strlen(flag_str))) {
			return TransmitterModePrintUsage;
		}
		flag_str = "--help";
		if (!strncmp(argv[1], flag_str, strlen(flag_str))) {
			return TransmitterModePrintUsage;
		}
		// check for the demo song
		flag_str = "--song";
		if (!strncmp(argv[1], flag_str, strlen(flag_str))) {
			return TransmitterModePlayDemo;
		}
		// must be invalid args
		return TransmitterModePrintUsage;
	} else if (argc == 3) {
		// check if the user wants us to send a file
		flag_str = "-f";
		if (!strncmp(argv[1], flag_str, strlen(flag_str))) {
			// check if the file is stdin
			flag_str = "-";
			if (!strncmp(argv[1], flag_str, strlen(flag_str))) {
				return TransmitterModeSendStdin;
			} else {
				return TransmitterModeSendFile;
			}
		}
	}
	
	// check if the user wants us to send strings
	if (argc >= 3) {
		flag_str = "-s";
		if (!strncmp(argv[1], flag_str, strlen(flag_str))) {
			return TransmitterModeSendStrings;
		}
	}
	
	// must be invalid args
	return TransmitterModePrintUsage;
}

int main(int argc, char *argv[])
{
	const TransmitterMode mode = parse_args(argc, argv);
	if (mode == TransmitterModePrintUsage) {
		print_usage(argv[0]);
		return 1;
	}
	
	// we know we'll be using the radio at this point, so initialize the transmitter
	transmitter_init();
	printf("Welcome to AirBridge!\n");
	printf("Please tune your radio to AM 1580 kHz and place the antenna over this computer's CPU.\n\n");
	
	if (mode == TransmitterModePlayDemo) {
		transmit_song();
		return 0;
	}
	
	// we know we'll be sending data at this point, so send calibration data
	printf("Sending calibration data\n");
	transmitter_send_calibration();
	
	switch (mode) {
		case TransmitterModeSendStdin:
			printf("\nSending stdin\n");
			transmit_stdin();
			break;
		case TransmitterModeSendFile:
			printf("\nSending file: %s\n", argv[2]);
			transmit_file(argv[2]);
			break;
		case TransmitterModeSendStrings:
			// transmit each string, separated by a space
			printf("\nSending string: %s\n", argv[2]);
			transmit_string(argv[2], strlen(argv[2]));
			for (int i = 3; i < argc; i++) {
				transmit_string(" ", strlen(" "));
				printf("\nSending string: %s\n", argv[i]);
				transmit_string(argv[i], strlen(argv[i]));
			}
			break;
		default:
			printf("\nPooperdoodle!\n");
			return -1;
	}
	
	// done transmitting, so we send a terminal and exit
	printf("\nSending terminal data\n");
	transmitter_send_terminal();
	return 0;
}
