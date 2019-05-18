//////////////////////////////////////////////////////////////////////GPS_DAEMON
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
//#include <sys/types.h>

#include "parser.h"
#include "uart.h"

#define GPS_LATITUDE_FILE	"/tmp/gps_latitude"
#define GPS_LONGITUDE_FILE	"/tmp/gps_longitude"
#define GPS_ALTITUDE_FILE	"/tmp/gps_altitude"
#define GPS_FIRMWARE_FILE	"/tmp/gps_firmware"

//unsigned char* gps_fifo="/tmp/gps_fifo";

void send_gps_datas(int sig) {
	int fd;
	printf("SIGUSR1 received\n");

	if(flag_writing) {
		flag_request=1;
	} else {
		/* send latitude */
		fd = open(GPS_LATITUDE_FILE, O_WRONLY);
		write(fd, data.latitude, sizeof(data.latitude));
		printf("Send latitude : %s\n", data.latitude);
		close(fd);
		/* send longitude */
		fd = open(GPS_LONGITUDE_FILE, O_WRONLY);
		write(fd, data.longitude, sizeof(data.longitude));
		printf("Send longitude : %s\n", data.longitude);
		close(fd);
		/* send altitude */
		fd = open(GPS_ALTITUDE_FILE, O_WRONLY);
		write(fd, data.altitude, sizeof(data.altitude));
		printf("Send altitude : %s\n", data.altitude);
		close(fd);
		/* send firmware */
		fd = open(GPS_FIRMWARE_FILE, O_WRONLY);
		write(fd, data.firmware, sizeof(data.firmware));
		printf("Send firmware : %s\n", data.firmware);
		close(fd);

		flag_request=0;
		}
	}

void quit_ipc(void) {
	unlink(GPS_LATITUDE_FILE);
	unlink(GPS_LONGITUDE_FILE);
	unlink(GPS_ALTITUDE_FILE);
	unlink(GPS_FIRMWARE_FILE);
	}

void quit(int sig) {
	quit_ipc();
	quit_parser();
	quit_uart();
	printf("DONE\n");
	exit(EXIT_SUCCESS);
	}

void init_ipc(void) {
	printf("PID : %d\n", getpid());

	if(signal(SIGUSR1, send_gps_datas) == SIG_ERR) {
		printf("ERROR: Can't catch SIGUSR1\n");
		exit(EXIT_FAILURE);
		}
	if(signal(SIGTERM, quit) == SIG_ERR) {
		printf("ERROR: Can't catch SIGTERM\n");
		exit(EXIT_FAILURE);
		}

	mkfifo(GPS_LATITUDE_FILE, 0666);
	mkfifo(GPS_LONGITUDE_FILE, 0666);
	mkfifo(GPS_ALTITUDE_FILE, 0666);
	mkfifo(GPS_FIRMWARE_FILE, 0666);
	}

void main(void) {
	init_ipc();
	init_parser();
	init_uart();
	init_gps();

	while(1) {
		if(flag_request) {
			send_gps_datas(SIGUSR1);
		} else {
			rx();
			}
		}
	}
