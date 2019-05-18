//////////////////////////////////////////////////////////////////////GPS_DAEMON
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
//#include <sys/types.h>

unsigned char* gps_fifo="/tmp/gps_fifo";
unsigned char flag_sending=0;

void send_gps_datas(int sig) {
	int fd;
	unsigned char* message="Hi";

	printf("SIGUSR1 received\n");
	flag_sending=1;

	fd = open(gps_fifo, O_WRONLY);
	write(fd, message, sizeof(message));
	printf("Send message : %s\n", message);
	close(fd);

	flag_sending=0;
	}

void end(int sig) {
	unlink(gps_fifo);
	printf("DONE\n");
	exit(0);	
	}

void main(void) {
	printf("PID : %d\n", getpid());

	if(signal(SIGUSR1, send_gps_datas) == SIG_ERR) {
		printf("ERROR: Can't catch SIGUSR1\n");
		exit(EXIT_FAILURE);
		}
	if(signal(SIGTERM, end) == SIG_ERR) {
		printf("ERROR: Can't catch SIGTERM\n");
		exit(EXIT_FAILURE);
		}

	mkfifo(gps_fifo, 0666);

	while(1) {}
	}
