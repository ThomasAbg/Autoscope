#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdint.h>

typedef struct _data_t {
	unsigned char latitude[242];
	unsigned char longitude[242];
	unsigned char firmware[242];
} data_t;

enum regex {		/* regex (?:) dont work with regex.h */
	CHECK,			/* NMEA sentence & checksum */
	PMTK705,		/* firmware datas */
	GPRMC,			/* GPS minimum datas */
	NREGEX
	};

regex_t r[NREGEX];
regmatch_t* match=NULL;

unsigned char check(unsigned char* str) {
	unsigned char sum=0;
	for(str;*str;str++) {
		sum^=*str;
		}
	return(sum);
	}

void init_parser(void) {
	if(regcomp(&r[CHECK], "^\\$(.+?)\\*([0-9A-F]{2})\r\n$", REG_EXTENDED))
		exit(EXIT_FAILURE); /* g1 NMEA sentence g2 checksum */
	if(regcomp(&r[PMTK705], "^PMTK705,([^,]*),[^,]*,[^,]*,[^,]*$", REG_EXTENDED))
		exit(EXIT_FAILURE); /* g1 firmware release */
	if(regcomp(&r[GPRMC], "^GPRMC,[^,]*,([AV]{1}),([^,]*,[NSEW]?),([^,]*,[NSEW]?),[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,([ADENS]{1})$", REG_EXTENDED))
		exit(EXIT_FAILURE); /* g1 status g2 latitude g3 longitude g4 mode */
	}

void parse_msg(char found[256], data_t* data) {
	unsigned char status=0;
	unsigned char mode=0;
	uint8_t start=0;
	uint8_t end=0;
	uint8_t size=0;
	for(int i=1;i<NREGEX;i++) {
		if(match) {
			free(match);
			match=NULL;
			}
		match=malloc(sizeof(*match) * (r[i].re_nsub+1));
		if(regexec(&r[i], found, r[i].re_nsub+1, match, 0) == 0) {
			switch(i) {
			case PMTK705 :
				/* firmware */
				start=match[1].rm_so;
				end=match[1].rm_eo;
				size=end-start;
				strncpy(data->firmware, &found[start], size);
				data->firmware[size]='\0';
				printf("data->firmware : %s\n", data->firmware);
				break;
			case GPRMC :
				/* datas validity */
				status=found[match[1].rm_so];
				mode=found[match[4].rm_so];
				if(status == 'V'
				|| mode == 'E'
				|| mode == 'N'
				|| mode == 'S') {
					data->latitude[0]=0;
					data->longitude[0]=0;
					printf("data->latitude : %s\n", data->latitude);
					printf("data->longitude : %s\n", data->longitude);
				} else if(status == 'A'
						&& (mode == 'A'
						|| mode == 'D')) {
					/* latitude */
					start=match[2].rm_so;
					end=match[2].rm_eo;
					size=end-start;
					strncpy(data->latitude, &found[start], size);
					data->latitude[size]='\0';
					printf("data->latitude : %s\n", data->latitude);
					/* longitude */
					start=match[3].rm_so;
					end=match[3].rm_eo;
					size=end-start;
					strncpy(data->longitude, &found[start], size);
					data->longitude[size]='\0';
					printf("data->longitude : %s\n", data->longitude);
					}
				break;
			default :
				continue;
				}
			}
		}
	}


void parse(char* rx_buffer, data_t* data) {
	printf("StringI : %s\n", rx_buffer);
	if(match) {
		free(match);
		match=NULL;
		}

	match=malloc(sizeof(*match) * (r[CHECK].re_nsub+1));
	if(regexec(&r[CHECK], rx_buffer, r[CHECK].re_nsub+1, match, 0) == 0) {
		unsigned char checksum[3]={};
		unsigned char found[256]={};
		uint8_t start=match[2].rm_so;
		uint8_t end=match[2].rm_eo;
		uint8_t size=end-start;

		strncpy(checksum, &rx_buffer[start], size);
		checksum[size]='\0';
		printf("checksum : %s\n", checksum);

		start=match[1].rm_so;
		end=match[1].rm_eo;
		size=end-start;

		strncpy(found, &rx_buffer[start], size);
		found[size]='\0';
		printf("found : %s\n", found);

		printf("check : %x\n", check(found));
		if(check(found) == (char)strtoul(checksum, NULL, 16)) {
			printf("CHECKSUM OK\n");
			parse_msg(found, data);
		} else {
			printf("CKECKSUM KO\n");
			/* don't parse currupted data */
			}
	} else {
		/* don't treat datas that doesn't seems to be NMEA */
		}
	}

void quit_regex(void) {
	for(int i=0;i<NREGEX;i++) {
		regfree(&r[i]);
		}
	if(match) {
		free(match);
		match=NULL;
		}
	exit(EXIT_SUCCESS);
	}

void main(void) {
	/* rx_buffer[256] 255 bytes + \0 */
	unsigned char* rx_buffer="$PMTK705,AXN_2.31_3339_13101700,5632,PA6H,1.0*6B\r\n";
	data_t data={};

	init_parser();
	printf("ok\n");

	parse(rx_buffer, &data);
	rx_buffer="$GPRMC,000700.800,V,,,,,0.00,0.00,060180,,,N*4D\r\n";
	parse(rx_buffer, &data);
	rx_buffer="$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W,A*07\r\n";
	parse(rx_buffer, &data);

	quit_regex();
	}
