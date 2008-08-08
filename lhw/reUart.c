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
	
	 // 자료 송수신
	 while ( 1)
	 {
		 poll_state = poll(                               // poll()을 호출하여 event 발생 여부 확인     
													  (struct pollfd*)&poll_events, // event 등록 변수
												    1,  // 체크할 pollfd 개수
												    1000   // time out 시간
											 );
     if ( 0 < poll_state)                             // 발생한 event 가 있음
			{     
				if ( poll_events.revents & POLLIN)            // event 가 자료 수신?
				{

					memset(buf,0,sizeof(buf));

					cnt = read( fd, buf, sizeof(buf));

					for(i=0;i<cnt;i++)
					{
						if(buf[i]==35)
							printf("disconnect ");
						else if(buf[i]==40)
							printf("connect ");
					}
			  	printf("%s\n",buf);
			  	}
		 	 	else if ( poll_events.revents & POLLERR)      // event 가 에러?
	   	 		{
	  			   printf( "통신 라인에 에러가 발생, 프로그램 종료");
				   break;
		  		}
	  		}
	 }
	 close( fd);
	 return 0;
}
