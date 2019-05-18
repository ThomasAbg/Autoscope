#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdint.h>

#ifndef PARSER_H
#define PARSER_H

#define MAX_RX			256
#define MAX_SENTENCE	242

typedef struct _data_t {
	unsigned char latitude[MAX_SENTENCE];
	unsigned char longitude[MAX_SENTENCE];
	unsigned char altitude[MAX_SENTENCE];
	unsigned char firmware[MAX_SENTENCE];
//	unsigned char date[MAX_SENTENCE];
//	unsigned char time[MAX_SENTENCE];
} data_t;

enum regex {		/* regex (?:) dont work with regex.h */
	CHECK,			/* NMEA sentence & checksum */
	PMTK001,		/* ACK of PMTK commands */
	PMTK705,		/* firmware datas */
	GPRMC,			/* GPS minimum datas */
	GPGLL,			/* GPS latitude & longitude */
	GPGGA,			/* fix & minimum datas & satellites status */
	NREGEX			/* number of regex, keep it last */
	};

extern regex_t r[NREGEX];
extern regmatch_t* match;
extern data_t data;

extern unsigned char flag_writing;
extern unsigned char flag_request;

void init_parser(void);
uint16_t parse(char rx_buffer[MAX_RX]);
void quit_parser(void);

static unsigned char check(unsigned char* str);
static uint16_t parse_msg(char found[MAX_RX]);

//void main(void);

#endif /* PARSER_H */
