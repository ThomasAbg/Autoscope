#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdint.h>

typedef struct _data_t {
	unsigned char* lattitude;
	unsigned char* longitude;
	unsigned char* firmware;
} data_t;

unsigned char check(unsigned char* str) {
	unsigned char sum=0;
	for(str;*str;str++) {
		sum^=*str;
		}
	return(sum);
	}

void parse(char* rx_buffer, int rx_length, data_t* data) {
//	printf("&rx : %p\n", rx_buffer);
	printf("StringI : %s\n", rx_buffer);
	int ret=0;

	regex_t r_check;
	regex_t r_test;
	regmatch_t* match=NULL;

	if(regcomp(&r_check, "^\\$(.+?)\\*([1-9A-F]{2})\r\n$", REG_EXTENDED)) exit(EXIT_FAILURE); //g1 crc
//	if(regcomp(&r_test, "o\\$(ok)$", REG_EXTENDED)) exit(EXIT_FAILURE); //g1 ok
//	printf("ok\n");

	match=malloc(sizeof(*match) * (r_check.re_nsub+1));
	if(regexec(&r_check, rx_buffer, r_check.re_nsub+1, match, 0) == 0) {

		unsigned char checksum[3]={};
		unsigned char* found=NULL;
		uint8_t start=match[2].rm_so;
		uint8_t end=match[2].rm_eo;
		uint8_t size=end-start;

		strncpy(checksum, &rx_buffer[start], size);
		checksum[size] = '\0';
//		printf("checksum : %s\n", checksum);

		start=match[1].rm_so;
		end=match[1].rm_eo;
		size=end-start;
		found=malloc(sizeof(*found) * (size+1));
		if(!found) exit(EXIT_FAILURE);

		strncpy(found, &rx_buffer[start], size);
		found[size] = '\0';
		printf("found : %s\n", found);

		printf("check : %x\n", check(found));
//		printf("atoi : %x\n", (char)strtoul(checksum, NULL, 16));
		if(check(found) == (char)strtoul(checksum, NULL, 16)) {
			printf("CHECKSUM OK\n");
//			parse_cmd(found, &data);
		} else {
			printf("CKECKSUM KO\n");
			}
	
//		for(int i=0;i<r_check.re_nsub+1;i++)
//			printf("Match%d : %s\n", i, found);
		free(found);


/*		for(int i=0;i<r_check.re_nsub+1;i++) {
			int start=match[i].rm_so;
			int end=match[i].rm_eo;
			int size=end-start;
			char* found=NULL;

			found=malloc(sizeof(*found) * (size+1));
			if(!found) exit(EXIT_FAILURE);
			strncpy(found, &rx_buffer[start], size);
			found[size] = '\0';
			printf("Match%d : %s\n", i, found);
			free(found);
			}*/
		}



//	printf("ret : %d\n", ret);
//	printf("begin : %d\n", match[1].rm_so);
//	printf("end : %d\n", match[1].rm_eo);
//	printf("nsub : %ld\n", r_check.re_nsub);
	









/*	match=malloc(sizeof(*match) * r_crc.re_nsub);
//	if(!regexec(&r_crc, rx_buffer, 1, &match, 0)) {
	ret=regexec(&r_crc, rx_buffer, match->rm_so, match, 0);
//		}

*/
	regfree(&r_check);
//	regfree(&r_test);
	free(match);
	}


void main(void) {
	//rx_buffer[256] 255 bytes + \0
	unsigned char* rx_buffer=/*"oo$ok";//*/"$PMTK705,AXN_2.31_3339_13101700,5632,PA6H,1.0*6B\r\n";
	unsigned int rx_length=/*5;//*/50;

	data_t data;	

//	printf("&rx : %p\n", rx_buffer);
	parse(rx_buffer, rx_length, &data);
	}
