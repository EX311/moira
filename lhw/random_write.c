#include "UART.h"

int main( void)
{  
   int    fd;
	 int ran_num;
	 char send_ip=123;
	 fd=initUart();	

	 srand((unsigned)time(NULL));// random ������ ������������ �Լ�

	 while(1)
	 {
		ran_num=1000+((rand()%1000)); // 1000~2000������ random�� ���� �߻�
//		printf("%d\n",ran_num);
 		write(fd,&send_ip,1);
		printf("send IP\n");
		msleep(ran_num);      // 1~2�� ���̵��� �����ϰ� sleep
	 }
	 close( fd);
	 return 0;
}
