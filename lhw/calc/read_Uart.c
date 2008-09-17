#include "./UART.h"

int main( void)
{  
      	 int  	i;
	 int    fd;
  	 int    cnt;
  	 char   buf[1024];
  	 int    poll_state;

   	 fd=initUart();
	 initPoll(fd);
	
	 while ( 1)
	 {
		poll_state = poll((struct pollfd*)&poll_events, 1, 1000);
     		if ( 0 < poll_state)                             
     		{     
			if ( poll_events.revents & POLLIN)            
			{
				memset(buf,0,sizeof(buf));
				cnt = read( fd, buf, sizeof(buf));
				printf("cnt=%d\n",cnt);
				if(buf[0] == 's')
		  			printf("sencer = %s\n",buf);
//				else 
//				printf("rxdata = %s\n",buf);
				else if(buf[0] == 'w')
					printf("Top disconnect\n");
				else if(buf[0] == 'a')
					printf("Left disconnect\n");
				else if(buf[0] == 'f')
					printf("Right disconnect\n");
				else if(buf[0] == 'x')
					printf("Bottom disconnect\n");
		  	}
	 	 	else if ( poll_events.revents & POLLERR)      
			{
	 	 		 printf( "통신 라인에 에러가 발생, 프로그램 종료");
				 break;
		 	}
	  	}
	 }
	 close( fd);
	 return 0;
}
