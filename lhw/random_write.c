#include "UART.h"

int main( void)
{  
   int    fd;
	 int ran_num;
	 char send_ip=123;
	 fd=initUart();	

	 srand((unsigned)time(NULL));// random 변수를 가져오기위한 함수

	 while(1)
	 {
		ran_num=1000+((rand()%1000)); // 1000~2000사이의 random한 숫잘 발생
//		printf("%d\n",ran_num);
 		write(fd,&send_ip,1);
		printf("send IP\n");
		msleep(ran_num);      // 1~2초 사이동안 랜덤하게 sleep
	 }
	 close( fd);
	 return 0;
}
