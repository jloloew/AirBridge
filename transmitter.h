
#ifndef _TRANSMITTER_H_
#define _TRANSMITTER_H_

#include <unistd.h>

void transmit_file(const char *filename);
void transmit_stdin(void);
void transmit_song(void);
void transmit_string(const char *message, size_t len);
void transmitter_init(void);
void transmitter_send_calibration(void);

#endif // _TRANSMITTER_H_
