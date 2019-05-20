#include "parser.h"

regex_t r[NREGEX];
regmatch_t* match=NULL;
data_t data={"0", "0", "0", "0"};

unsigned char flag_writing=0;
unsigned char flag_request=0;


static unsigned char check(unsigned char* str) {
	unsigned char sum=0;
	for(str;*str;str++) {
		sum^=*str;
		}
	return(sum);
	}


/* to work on regex you can use these websites */
/* https://regex101.com/ */
/* http://www.rexegg.com/regex-quickstart.html */
void init_parser(void) {
	if(regcomp(&r[CHECK], "^\\$(.+?)\\*([0-9A-F]{2})\r\n$", REG_EXTENDED))
		exit(EXIT_FAILURE); /* g1 NMEA sentence g2 checksum */
	if(regcomp(&r[PMTK001], "^PMTK001,([^,]*),([0-3]?)$", REG_EXTENDED))
		exit(EXIT_FAILURE); /* g1 command g2 ACK */
	if(regcomp(&r[PMTK705], "^PMTK705,([^,]*),[^,]*,[^,]*,[^,]*$", REG_EXTENDED))
		exit(EXIT_FAILURE); /* g1 firmware release */
	if(regcomp(&r[GPRMC], "^GPRMC,[^,]*,([AV]{1}),([^,]*,[NS]?),([^,]*,[EW]?),[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,([ADENS]{1})$", REG_EXTENDED))
		exit(EXIT_FAILURE); /* g1 status g2 latitude g3 longitude g4 mode */
	if(regcomp(&r[GPGLL], "^GPGLL,([^,]*,[NS]?),([^,]*,[EW]?),[^,]*,([AV]{1}),([ADENS]{1})$", REG_EXTENDED))
		exit(EXIT_FAILURE); /* g1 latitude g2 longitude g3 status g4 mode */
	if(regcomp(&r[GPGGA], "^GPGGA,[^,]*,([^,]*,[NS]?),([^,]*,[EW]?),[0-8]?,[^,]*,[^,]*,([^,]*,M?),[^,]*,M?,[^,]*,[^,]*$", REG_EXTENDED))
		exit(EXIT_FAILURE); /* g1 latitude g2 longitude g3 fix g4 altitude */
	}

static uint16_t parse_msg(char found[MAX_RX]) {
	unsigned char command[4]={};
	unsigned char status=0;
	unsigned char mode=0;
	unsigned char fix=0;
	unsigned char ack=0;
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
			case PMTK001 :
				flag_writing=1;
				/* ACK */
				ack=found[match[2].rm_so]	;
				/* command */
				start=match[1].rm_so;
				end=match[1].rm_eo;
				size=end-start;
				strncpy(command, &found[start], size);
				if(ack == '3') {
					flag_writing=0;
					return(atoi(command));
				} else {
					flag_writing=0;
					break;
					}
			case PMTK705 :
				flag_writing=1;
				/* firmware */
				start=match[1].rm_so;
				end=match[1].rm_eo;
				size=end-start;
				strncpy(data.firmware, &found[start], size);
				data.firmware[size]='\0';
				printf("data.firmware : %s\n", data.firmware);
				flag_writing=0;
				break;
			case GPRMC :
				flag_writing=1;
				/* datas validity */
				status=found[match[1].rm_so];
				mode=found[match[4].rm_so];
				if(status == 'V'
				|| mode == 'E'
				|| mode == 'N'
				|| mode == 'S') {
					data.latitude[0]='0';
					data.latitude[1]='\0';
					data.longitude[0]='0';
					data.longitude[1]='\0';
					printf("data.latitude : %s\n", data.latitude);
					printf("data.longitude : %s\n", data.longitude);
				} else if(status == 'A'
						&& (mode == 'A'
						|| mode == 'D')) {
					/* latitude */
					start=match[2].rm_so;
					end=match[2].rm_eo;
					size=end-start;
					strncpy(data.latitude, &found[start], size);
					data.latitude[size]='\0';
					printf("data.latitude : %s\n", data.latitude);
					/* longitude */
					start=match[3].rm_so;
					end=match[3].rm_eo;
					size=end-start;
					strncpy(data.longitude, &found[start], size);
					data.longitude[size]='\0';
					printf("data.longitude : %s\n", data.longitude);
					}
				flag_writing=0;
				break;
			case GPGLL :
				flag_writing=1;
				/* datas validity */
				status=found[match[3].rm_so];
				mode=found[match[4].rm_so];
				if(status == 'V'
				|| mode == 'E'
				|| mode == 'N'
				|| mode == 'S') {
					data.latitude[0]='0';
					data.latitude[1]='\0';
					data.longitude[0]='0';
					data.longitude[1]='\0';
					printf("data.latitude : %s\n", data.latitude);
					printf("data.longitude : %s\n", data.longitude);
				} else if(status == 'A'
						&& (mode == 'A'
						|| mode == 'D')) {
					/* latitude */
					start=match[1].rm_so;
					end=match[1].rm_eo;
					size=end-start;
					strncpy(data.latitude, &found[start], size);
					data.latitude[size]='\0';
					printf("data.latitude : %s\n", data.latitude);
					/* longitude */
					start=match[2].rm_so;
					end=match[2].rm_eo;
					size=end-start;
					strncpy(data.longitude, &found[start], size);
					data.longitude[size]='\0';
					printf("data.longitude : %s\n", data.longitude);
					}
				flag_writing=0;
				break;
			case GPGGA :
				flag_writing=1;
				/* datas validity */
				fix=found[match[3].rm_so];
				if(fix == '0'
				|| fix == '6'
				|| fix == '7'
				|| fix == '8') {
					data.latitude[0]='0';
					data.latitude[1]='\0';
					data.longitude[0]='0';
					data.longitude[1]='\0';
					data.altitude[0]='0';
					data.altitude[1]='\0';
					printf("data.latitude : %s\n", data.latitude);
					printf("data.longitude : %s\n", data.longitude);
					printf("data.altitude : %s\n", data.altitude);
				} else if(fix == '1'
						|| fix == '2'
						|| fix == '3'
						|| fix == '4'
						|| fix == '5') {
					/* latitude */
					start=match[1].rm_so;
					end=match[1].rm_eo;
					size=end-start;
					strncpy(data.latitude, &found[start], size);
					data.latitude[size]='\0';
					printf("data.latitude : %s\n", data.latitude);
					/* longitude */
					start=match[2].rm_so;
					end=match[2].rm_eo;
					size=end-start;
					strncpy(data.longitude, &found[start], size);
					data.longitude[size]='\0';
					printf("data.longitude : %s\n", data.longitude);
					/* altitude */
					start=match[4].rm_so;
					end=match[4].rm_eo;
					size=end-start;
					strncpy(data.altitude, &found[start], size);
					data.altitude[size]='\0';
					printf("data.altitude : %s\n", data.altitude);
					}
				flag_writing=0;
				break;
			default :
				continue;
				}
			}
		}
	return(0);
	}

uint16_t parse(char rx_buffer[MAX_RX]) {
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
			return(parse_msg(found));
		} else {
			printf("CKECKSUM KO\n");
			/* don't parse currupted data */
			}
	} else {
		/* don't treat datas that doesn't seems to be NMEA */
		}
	return(0);
	}

void quit_parser(void) {
	for(int i=0;i<NREGEX;i++) {
		regfree(&r[i]);
		}
	if(match) {
		free(match);
		match=NULL;
		}
	}

/*void main(void) {
	/* rx_buffer[256] 255 bytes + \0 * /
	unsigned char* rx_buffer="$PMTK705,AXN_2.31_3339_13101700,5632,PA6H,1.0*6B\r\n";
//	data_t data={};

	init_parser();
	printf("ok\n");

	parse(rx_buffer);
	rx_buffer="$GPRMC,000700.800,V,,,,,0.00,0.00,060180,,,N*4D\r\n";
	parse(rx_buffer);
	rx_buffer="$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W,A*07\r\n";
	parse(rx_buffer);
	rx_buffer="$GPGLL,4916.45,N,12311.12,W,225444,A,A*5C\r\n";
	parse(rx_buffer);

	rx_buffer="$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
	parse(rx_buffer);

	quit_parser();
	}*/
