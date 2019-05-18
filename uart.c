#include "uart.h"
//#include "parser.h"

int uart=-1;


static void tx(unsigned char* tx_buffer) {
	if(uart != -1) {
		if(write(uart, &tx_buffer[0], strlen(tx_buffer)) < 0) {
			printf("UART TX error\n");
			}
		}
	}
	
/* create a buffer and send it to the parser */
uint16_t rx(void) {
	if(uart != -1) {
		unsigned char rx_buffer[MAX_RX]={};
		int rx_length = read(uart, (void*)rx_buffer, MAX_RX-1);
		if(rx_length) {
			rx_buffer[rx_length] = '\0';
			printf("%i bytes read : %s", rx_length, rx_buffer);
			return(parse(rx_buffer));
			}
		}
	return(0);
	}

void init_uart(void) {
	struct termios tc;

	uart=open(UART_FILE, O_RDWR | O_NOCTTY);
	if(uart == -1) {
		printf("ERROR: Can't open UART. Ensure it is not in use by another application\n");
		exit(EXIT_FAILURE);
		}

	tcgetattr(uart, &tc);

	/* input flags = 0 */
	tc.c_iflag &= ~(IGNBRK | IGNPAR | PARMRK | INPCK | ISTRIP | INLCR | ICRNL | IGNCR | IXON | IXOFF);
	/* output flags = 0 */
	tc.c_oflag &= ~(OPOST | ONLCR | OCRNL | OFILL);
	/* control flags = 0xcbd */
	tc.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS);
	tc.c_cflag |= (CLOCAL | CREAD | HUPCL | B9600);
	tc.c_cflag |= CS8; /* need to be single */
	/* local flags = 0 */
	tc.c_lflag &= ~(ISIG | ECHO | ECHONL | NOFLSH | IEXTEN);

	tcflush(uart, TCIFLUSH);
	tcsetattr(uart, TCSANOW, &tc);
	}

void init_gps(void) {
	do {
		/* GGA every one fix & GLL, RMC, VTG, GSA, GSV never */
		tx("$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
	} while(rx() != 314);
	/* request firmware datas */
	tx("$PMTK605*31\r\n");
	}

void quit_uart(void) {
	close(uart);
	}

/*void main(void) {
	init_uart();


	tx("$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");

	while(1) {
		rx();
		}

	quit_uart();
	}*/
