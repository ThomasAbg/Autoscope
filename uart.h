#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdint.h>
//#include <sys/stat.h>

#include "parser.h"

#ifndef UART_H
#define UART_H

#define UART_FILE	"/dev/serial0"

extern int uart;

uint16_t rx(void);
void init_uart(void);
void init_gps(void);
void quit_uart(void);

static void tx(unsigned char* tx_buffer);

//void main(void);

#endif /* UART_H */
