#include <stdio.h>
#include <stdlib.h>
#include "cube.h"
#include <termios.h>
#include <fcntl.h>
#include <strings.h>

void cube_push (unsigned char data[8][8])
{
	int x,y,i;

	i= 0;

	unsigned char buffer[200];

	buffer[i++] = 0xff; // escape
	buffer[i++] = 0x00; // reset to 0,0

	for (x=0;x<8;x++)
	{
		for (y=0;y<8;y++)
		{
			buffer[i++] = data[x][y];
			if (data[x][y] == 0xff)
			{
				buffer[i++] = data[x][y];
			}
		}
	}

	write(tty,&buffer,i);
}

int cube_init (void)
{
	
	//FILE *ftty;

	//ftty = fopen("/dev/ttyUSB0","a");

	
	struct termios io;

	char *tty_path = "/dev/ttyUSB0";

	//tty = open(tty_path, O_RDWR | O_NOCTTY | O_NDELAY); // <-  ORIGINAL
	tty = open(tty_path, O_RDWR); 
	
	
	if (tty <0) {perror(tty_path); exit(-1); }
	
	bzero(&io, sizeof(io));
	//io.c_cflag = B2400 | CRTSCTS | CS8 | CLOCAL | CREAD;
	//io.c_cflag = B2400 | CRTSCTS | PARENB | CS8 | CLOCAL | CREAD;
	io.c_cflag = B38400 | PARENB | CS8 | CLOCAL | CREAD;
	io.c_iflag = IGNPAR;
	io.c_oflag = 0;
	
	// set input mode (non-canonical, no echo,...)
	io.c_lflag &= ~OPOST;
	//io.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	
	io.c_cc[VTIME]    = 0;   // inter-character timer unused
	io.c_cc[VMIN]     = 0;   // blocking read until 5 chars received
	
	// Flush buffer
	tcflush(tty, TCIFLUSH);
	// write config to tty
	tcsetattr(tty,TCSANOW,&io);
	

	//fcntl(tty, F_SETFL, 0);
	


	return 1;
}
