#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <termios.h>                   // B115200, CS8 등 상수 정의
#include <fcntl.h>                     // O_RDWR , O_NOCTTY 등의 상수 정의

#ifndef UART_H__
#define UART_H__


void msleep(unsigned int useconds)
{
   int i;
   for(i=0;i<1000;i++)
   usleep(useconds);
};

static struct termios    newtio;
static struct pollfd     poll_events;      // 체크할 event 정보를 갖는 struct
 
int initUart(void)
{
   int    fd;
 // 시리얼 포트를 open
	 fd = open( "/dev/tts/1", O_RDWR | O_NOCTTY | O_NONBLOCK );        // 디바이스를 open 한다.
	 if ( 0 > fd)
	 {        
		 printf("open errorn");
		 return -1;
	 }
	 // 시리얼 포트 통신 환경 설정
	 memset( &newtio, 0, sizeof(newtio) );
	 newtio.c_cflag       = B9600 | CS8 | CLOCAL | CREAD;
	 newtio.c_oflag       = 0;
	 newtio.c_lflag       = 0;
	 newtio.c_cc[VTIME]   = 0;
	 newtio.c_cc[VMIN]    = 1;
	      
	 tcflush  (fd, TCIFLUSH );
	 tcsetattr(fd, TCSANOW, &newtio );
	 fcntl(fd, F_SETFL, FNDELAY); 
	
	 // poll 사용을 위한 준비
	                                         
		return fd;
}

int initPoll(int fd)
{
	 poll_events.fd        = fd;
	 poll_events.events    = POLLIN | POLLERR;          // 수신된 자료가 있는지, 에러가 있는지
	 poll_events.revents   = 0;

}


#endif
