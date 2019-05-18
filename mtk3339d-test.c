//////////////////////////////////////////////////////////////////AUTOSCOPE_CORE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define GPS_PID_FILE		"/var/run/mtk3339d.pid"
#define GPS_LATITUDE_FILE	"/tmp/gps_latitude"
#define GPS_LONGITUDE_FILE	"/tmp/gps_longitude"
#define GPS_ALTITUDE_FILE	"/tmp/gps_altitude"
#define GPS_FIRMWARE_FILE	"/tmp/gps_firmware"

#define MAX_SENTENCE		242

void get_gps_pid(pid_t* gps_pid) {
	FILE* f_gps_pid;
	unsigned char buf[6]={}; /* pid up to 5 digits + \0 | limits.h LINE_MAX */
	if(!(f_gps_pid=fopen(GPS_PID_FILE, "r"))) {
		printf("ERROR: Can't open GPS daemon PID file\n");
		exit(EXIT_FAILURE);
		}

	fgets(buf, 6, f_gps_pid);
	*gps_pid=atoi(buf);
	fclose(f_gps_pid);
//	printf("atoi : %d\n", *gps_pid);
	}

void read_gps_data(pid_t* gps_pid){
	int fd;
//	unsigned char* gps_fifo = GPS_FIFO_FILE;
	unsigned char buf[MAX_SENTENCE];

	/*request to send data*/
	kill(*gps_pid, SIGUSR1);

	/*read latitude*/
	fd = open(GPS_LATITUDE_FILE, O_RDONLY);
	read(fd, buf, MAX_SENTENCE);
	printf("Received latitude : %s\n", buf);
	close(fd);
	/*read longitude*/
	fd = open(GPS_LONGITUDE_FILE, O_RDONLY);
	read(fd, buf, MAX_SENTENCE);
	printf("Received longitude : %s\n", buf);
	close(fd);
	/*read altitude*/
	fd = open(GPS_ALTITUDE_FILE, O_RDONLY);
	read(fd, buf, MAX_SENTENCE);
	printf("Received altitude : %s\n", buf);
	close(fd);
	/*read firmware*/
	fd = open(GPS_FIRMWARE_FILE, O_RDONLY);
	read(fd, buf, MAX_SENTENCE);
	printf("Received firmware : %s\n", buf);
	close(fd);
	}

void main(void) {
	pid_t gps_pid=0;
	get_gps_pid(&gps_pid);
	printf("GPS daemon PID : %d\n", gps_pid);

	read_gps_data(&gps_pid);
	}

