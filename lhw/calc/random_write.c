#include <stdio.h>
#include "UART.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>


int main( void)
{  
	 int tmp;
   int ip_fd; 
	 int    fd;
	 int ran_num;
	 int m_ip, t_ip, b_ip, l_ip, r_ip;
	 char buf[3]={0,};
	 char send_data[17]={0,};
	 char ip_temp[15]={0,};
	 char ipBuf[15]={0,};
	 char ip;
	 int cnt;
	 int i;
	 ip_fd=open("./config_MyIp",O_RDONLY);
	 // sleep(1);
	tmp = read(ip_fd,(char*)buf,sizeof(buf)+1);
	//	printf("%d\n",tmp);
	 if (tmp <0)
	 {
		 perror ("read\n");
		 exit(1);
	 }
		 m_ip=atoi(buf);


		fd=initUart();	

	 srand((unsigned)time(NULL));
	 while(1)
	 {
//		printf("%d\n",ip);
	  
		strncpy(ip_temp+12,buf,3);
		cnt=get_IpInfo(3,ipBuf);
		if(cnt==1)
			{	ipBuf[0]='0'; ipBuf[1]='0'; ipBuf[2]='0';}
		strncpy(ip_temp,ipBuf,3);

		cnt=get_IpInfo(2,ipBuf);
		if(cnt==1)
			{	ipBuf[0]='0'; ipBuf[1]='0'; ipBuf[2]='0'; }
		strncpy(ip_temp+3,ipBuf,3);

		cnt=get_IpInfo(1,ipBuf);
		if(cnt==1)
			{	ipBuf[0]='0'; ipBuf[1]='0'; ipBuf[2]='0'; }
		strncpy(ip_temp+6,ipBuf,3);

		cnt=get_IpInfo(0,ipBuf);
		if(cnt==1)
			{	ipBuf[0]='0'; ipBuf[1]='0'; ipBuf[2]='0'; }
		strncpy(ip_temp+9,ipBuf,3);

		send_data[0]='f';
		strncpy(send_data+1,ip_temp,15);
		send_data[16]='e';

		ran_num=((rand()%50000))+50000;
 		
		for(i=0;i<17;i++)
//		printf("%c",send_data[i]);

//		printf("\n");
		write(fd,send_data,sizeof(send_data));
		usleep(ran_num);
	 }
	 close(ip_fd);
	 close( fd);
	 return 0;
}
