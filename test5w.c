//////////////////////////////////////////////////////////////////AUTOSCOPE_CORE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define GPS_PID_FILE	"./test.pid"		///var/run/mtk3339d.pid
#define GPS_FIFO_FILE	"/tmp/gps_fifo"
#define MAX_BUF			1024				//256

void get_gps_pid(pid_t* gps_pid) {
	FILE* f_gps_pid;
	char buf[6]={}; //pid up to 5 digits + \0 | limits.h LINE_MAX
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
	char* gps_fifo = GPS_FIFO_FILE;
	char buf[MAX_BUF];

	/*request to send data*/
	kill(*gps_pid, SIGUSR1);

	/*read through fifo*/
	fd = open(gps_fifo, O_RDONLY);
	read(fd, buf, MAX_BUF);
	printf("Received message : %s\n", buf);
	close(fd);
	}

void main(void) {
	pid_t gps_pid=0;
	get_gps_pid(&gps_pid);
	printf("GPS daemon PID : %d\n", gps_pid);

	read_gps_data(&gps_pid);
	}

