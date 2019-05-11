#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
//#include <sys/stat.h>

#define UART	"/dev/serial0"

void tx(int* uart) {
	unsigned char tx_buffer[20];
	unsigned char *p_tx_buffer;
	
	p_tx_buffer = &tx_buffer[0];
	*p_tx_buffer++ = 'H';
	*p_tx_buffer++ = 'e';
	*p_tx_buffer++ = 'l';
	*p_tx_buffer++ = 'l';
	*p_tx_buffer++ = 'o';
	
	if(*uart != -1) {
		int count = write(*uart, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
		if(count < 0) {
			printf("UART TX error\n");
			}
		}
	}

void parse(void* rx_buffer, rx_length) {
	
	}

//create a buffer and send it to parser for each read
void rx(int* uart) {
	if(*uart != -1) {
		unsigned char rx_buffer[256];
		int rx_length = read(*uart, (void*)rx_buffer, 255);
		if(rx_length) {
			//Bytes received
			rx_buffer[rx_length] = '\0';
			printf("%i bytes read : %s", rx_length, rx_buffer);
//			parse((void*)rx_buffer, rx_length);
			}
		}
	}

void init(int* uart) {
	struct termios tc;

	*uart=open(UART, O_RDWR | O_NOCTTY);// | O_NDELAY);
	if(*uart == -1) {
		printf("ERROR: Can't open UART. Ensure it is not in use by another application\n");
		exit(EXIT_FAILURE);
		}

	tcgetattr(*uart, &tc);

	/* input flags = 0 */
	tc.c_iflag &= ~(IGNBRK | IGNPAR | PARMRK | INPCK | ISTRIP | INLCR | ICRNL | IGNCR | IXON | IXOFF);
	/* output flags = 0 */
	tc.c_oflag &= ~(OPOST | ONLCR | OCRNL | OFILL);
	/* control flags = cbd */
	tc.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS);
	tc.c_cflag |= (CLOCAL | CREAD | HUPCL | B9600);
	tc.c_cflag |= CS8; /* need to be single */
	/* local flags = 0 */
	tc.c_lflag &= ~(ISIG | ECHO | ECHONL | NOFLSH | IEXTEN);

	tcflush(*uart, TCIFLUSH);
	tcsetattr(*uart, TCSANOW, &tc);
	}

void main(void) {
	int uart=-1;
	init(&uart);

	tx(&uart);
	while(1) {
		rx(&uart);
		}

	close(uart);
	}
