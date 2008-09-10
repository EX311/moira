#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "read_proc.h"

int main()
{
//		printf("MasterIpSet process start!\n");	
	char IsConnect;
	char buf[3];
	int res;
 	int stat;
	char IpInfo1[15]={0,};
	char IpInfo2[15]={0,};
	char myIp[3]={0,};
	int myIpfd;
	int cnt;
	myIpfd=open("./config_MyIp",O_RDONLY);

	cnt=read(myIpfd,myIp,sizeof(myIp));
//	printf("cnt=%d\n",cnt);

	IsConnect = get_IsConnect();

	switch(IsConnect)
	{
		case 0:	//0000
//			set_MyLocation(5);
			system("echo "" > /proc/board_status/AfterMasterIp/AfterMasterIp0");
			system("echo "" > /proc/board_status/AfterMasterIp/AfterMasterIp1");
			system("echo "" > /proc/board_status/AfterMasterIp/AfterMasterIp2");
			system("echo "" > /proc/board_status/AfterMasterIp/AfterMasterIp3");
			break;
	
		case 1:	//0001
//			set_MyLocation(2);
				get_IpInfo(0,IpInfo1);
//				printf("0 = %s\n",IpInfo1);
//				printf("My Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);

//				printf("place 0's Ip is %c%c%c\n",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				sprintf(buf,"%c%c%c",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(0,res);

//				printf("place 2's Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);
				sprintf(buf,"%c%c%c",myIp[0],myIp[1],myIp[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(2,res);
			
//				printf("AfterMasterIp set End!!!!\n");
			break;
	

		case 2:	//0010
//			set_MyLocation(1);
				get_IpInfo(1,IpInfo1);
//				printf("1 = %s\n",IpInfo1);
//				printf("My Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);

//				printf("place 0's Ip is %c%c%c\n",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				sprintf(buf,"%c%c%c",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(0,res);

//				printf("place 1's Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);
				sprintf(buf,"%c%c%c",myIp[0],myIp[1],myIp[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(1,res);

//				printf("AfterMasterIp set End!!!!\n");
			break;
		
		case 4: //0100
//			set_MyLocation(0);
				get_IpInfo(2,IpInfo1);
//				printf("2 = %s\n",IpInfo1);
//				printf("My Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);

//				printf("place 2's Ip is %c%c%c\n",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				sprintf(buf,"%c%c%c",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(2,res);

//				printf("place 0's Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);
				sprintf(buf,"%c%c%c",myIp[0],myIp[1],myIp[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(0,res);

//				printf("AfterMasterIp set End!!!!\n");
			break;
		
		case 8: //1000
//			set_MyLocation(0);
				get_IpInfo(1,IpInfo1);
//				printf("1 = %s\n",IpInfo1);
//				printf("My Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);

//				printf("place 1's Ip is %c%c%c\n",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				sprintf(buf,"%c%c%c",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(1,res);
		
//				printf("place 0's Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);
				sprintf(buf,"%c%c%c",myIp[0],myIp[1],myIp[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(0,res);

//				printf("AfterMasterIp set End!!!!\n");
			break;
		
		case 3: //0011
//			set_MyLocation(3);
				get_IpInfo(0,IpInfo1);
				get_IpInfo(1,IpInfo2);
//				printf("0 = %s\n",IpInfo1);
//				printf("1 = %s\n",IpInfo2);
//				printf("My Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);
				
//				printf("place 0's Ip is %c%c%c\n",IpInfo2[3],IpInfo2[4],IpInfo2[5]);
				sprintf(buf,"%c%c%c",IpInfo2[3],IpInfo2[4],IpInfo2[5]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(0,res);
			
//				printf("place 1's Ip is %c%c%c\n",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				sprintf(buf,"%c%c%c",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(1,res);
	
//				printf("place 2's Ip is %c%c%c\n",IpInfo2[0],IpInfo2[1],IpInfo2[2]);
				sprintf(buf,"%c%c%c",IpInfo2[0],IpInfo2[1],IpInfo2[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(2,res);
	
//				printf("place 3's Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);
				sprintf(buf,"%c%c%c",myIp[0],myIp[1],myIp[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(3,res);

//				printf("AfterMasterIp set End!!!!\n");
			break; 
		
		case 6: //0110
//			set_MyLocation(1);
				get_IpInfo(1,IpInfo1);
				get_IpInfo(2,IpInfo2);
//				printf("1 = %s\n",IpInfo1);
//				printf("2 = %s\n",IpInfo2);
//				printf("My Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);
				
//				printf("place 0's Ip is %c%c%c\n",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				sprintf(buf,"%c%c%c",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(0,res);

//				printf("place 1's Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);
				sprintf(buf,"%c%c%c",myIp[0],myIp[1],myIp[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(1,res);
	
//				printf("place 2's Ip is %c%c%c\n",IpInfo1[6],IpInfo1[7],IpInfo1[8]);
				sprintf(buf,"%c%c%c",IpInfo1[6],IpInfo1[7],IpInfo1[8]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(2,res);

//				printf("place 3's Ip is %c%c%c\n",IpInfo2[0],IpInfo2[1],IpInfo2[2]);
				sprintf(buf,"%c%c%c",IpInfo2[0],IpInfo2[1],IpInfo2[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(3,res);

//				printf("AfterMasterIp set End!!!!\n");
			break;
		
		case 12: //1100
//			set_MyLocation(0);
				get_IpInfo(2,IpInfo1);
				get_IpInfo(3,IpInfo2);
//				printf("2 = %s\n",IpInfo1);
//				printf("3 = %s\n",IpInfo2);
//				printf("My Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);
				
//				printf("place 0's Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);
				sprintf(buf,"%c%c%c",myIp[0],myIp[1],myIp[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(0,res);

//				printf("place 1's Ip is %c%c%c\n",IpInfo2[0],IpInfo2[1],IpInfo2[2]);
				sprintf(buf,"%c%c%c",IpInfo2[0],IpInfo2[1],IpInfo2[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(1,res);

//				printf("place 2's Ip is %c%c%c\n",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				sprintf(buf,"%c%c%c",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(2,res);

//				printf("place 3's Ip is %c%c%c\n",IpInfo2[6],IpInfo2[7],IpInfo2[8]);
				sprintf(buf,"%c%c%c",IpInfo2[6],IpInfo2[7],IpInfo2[8]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(3,res);
	
//				printf("AfterMasterIp set End!!!!\n");
			break;
		
		case 9: //1001
//			set_MyLocation(2);
				get_IpInfo(0,IpInfo1);
				get_IpInfo(3,IpInfo2);
//				printf("0 = %s\n",IpInfo1);
//				printf("3 = %s\n",IpInfo2);
//				printf("My Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);
		
//				printf("place 0's Ip is %c%c%c\n",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				sprintf(buf,"%c%c%c",IpInfo1[0],IpInfo1[1],IpInfo1[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(0,res);

//				printf("place 1's Ip is %c%c%c\n",IpInfo2[3],IpInfo2[4],IpInfo2[5]);
				sprintf(buf,"%c%c%c",IpInfo2[3],IpInfo2[4],IpInfo2[5]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(1,res);

//				printf("place 2's Ip is %c%c%c\n",myIp[0],myIp[1],myIp[2]);
				sprintf(buf,"%c%c%c",myIp[0],myIp[1],myIp[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(2,res);

//				printf("place 3's Ip is %c%c%c\n",IpInfo2[0],IpInfo2[1],IpInfo2[2]);
				sprintf(buf,"%c%c%c",IpInfo2[0],IpInfo2[1],IpInfo2[2]);
				res=strtol(buf,NULL,0);
				set_AfterMasterIp(3,res);

//				printf("AfterMasterIp set End!!!!\n");
			break;
	
	}
	close(myIpfd);
	return 1;

}

