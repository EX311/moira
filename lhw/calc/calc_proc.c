#include "read_proc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
	char IsConnect;
//	int stat_val;
//	pid_t child_pid;
	IsConnect = get_IsConnect();
	
//	printf("MyLocation Set Start!!\n");
	switch(IsConnect)
	{
		case 0:	//0000
			set_MyLocation(5);
			break;
	
		case 1:	//0001
			set_MyLocation(2);
			break;
		
		case 2:	//0010
			set_MyLocation(1);
			break;
		
		case 4: //0100
			set_MyLocation(0);
			break;
		
		case 8: //1000
			set_MyLocation(0);
			break;
		
		case 3: //0011
			set_MyLocation(3);
			break; 
		
		case 6: //0110
			set_MyLocation(1);
			break;
		
		case 12: //1100
			set_MyLocation(0);
			break;
		
		case 9: //1001
			set_MyLocation(2);
			break;
	
	}
	printf("IsConnect=");
	system("cat /proc/board_status/AfterMasterIp/AfterMasterIp0");

//	printf("MyLocation Set END!!\n");
//	printf("DeviceAttached Set Start!!\n");

	switch(IsConnect)
	{
		case 0:	//0000
			set_DeviceAttached(0);
			break;

		case 1:	//0001
		case 2:	//0010
		case 4: //0100
		case 8: //1000
			set_DeviceAttached(1);
			break;
		
		case 3: //0011
		case 6: //0110
		case 12: //1100
		case 9: //1001
			set_DeviceAttached(2);
			break;
	
	}
//	printf("DeviceAttached Set Start!!\n");
	printf("DeviceAttached=");
	system("cat /proc/board_status/AfterMasterIp/DeviceAttached");

	execl("/root/Master","MasterIpSet",0);
	 
//	 child_pid=wait(&stat_val);
//execl("./UI","UI",0);

	/*
if( res == (char)0)
	{
		printf("set 0\n");
		set_DeviceAttached(0);
	}
	


else if(res == (char)1 || res == (char)2 || res == (char)4 || res== (char)8)
	{
		printf("set 1\n");
		set_DeviceAttached(1);
	}
	else if(res == (char)3 || res == (char)6 || res == (char)12 || res == (char)9 )
	{
		printf("set 2\n");
		set_DeviceAttached(2);
	}
	
	else
		printf("DeviceAttached error\n");

		*/
	return IsConnect;



}
