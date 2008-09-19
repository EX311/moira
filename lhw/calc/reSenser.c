#include "./UART.h"

int main( void)
{  
      	 int  	i;
	 int    fd;
  	 int    cnt;
  	 char   buf[1024];
  	 int    poll_state;
	 char 	s_rxdata[15]={0,};
	 int 	senser=0;

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
				{
					s_rxdata[0]=buf[1];	s_rxdata[1]=buf[2];	s_rxdata[2]=buf[3];
					printf("senser_data=%s\n",s_rxdata);
							
						senser=atoi(s_rxdata);
						set_Senser(senser);

				
				}
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
