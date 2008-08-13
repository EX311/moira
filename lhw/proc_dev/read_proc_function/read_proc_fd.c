#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include "read_proc.h"

#define DEBUG 0

int IsConnect_fd_return(void)
{
   int    fd;
	 fd = open( "/proc/board_status/IsConnect", O_RDWR | O_NOCTTY | O_NONBLOCK );    
	 if ( 0 > fd)
	 {        
		 printf("/proc/board_status/IsConnect open errorn");
		 return -1;
	 }
	 return fd;
}

int IpInfo_fd_return(int num)
{
   int    fd;
	 switch(num)
	 {
		 case 0:
	 	 	fd = open( "/proc/board_status/IpInfo/IpInfo0", O_RDWR | O_NOCTTY | O_NONBLOCK );    
	 		if ( 0 > fd) {	
	  		printf("/proc/board_status/IpInfo/IpInfo0 open errorn");
			 	return -1;
			}
			break;
		 case 1:
	 	 	fd = open( "/proc/board_status/IpInfo/IpInfo1", O_RDWR | O_NOCTTY | O_NONBLOCK );    
	 		if ( 0 > fd) {	
	  		printf("/proc/board_status/IpInfo/IpInfo1 open errorn");
			 	return -1;
			}
			break;
		 case 2:
	 	 	fd = open( "/proc/board_status/IpInfo/IpInfo2", O_RDWR | O_NOCTTY | O_NONBLOCK );    
	 		if ( 0 > fd) {	
	  		printf("/proc/board_status/IpInfo/IpInfo2 open errorn");
			 	return -1;
			}
			break;
		 case 3:
	 	 	fd = open( "/proc/board_status/IpInfo/IpInfo3", O_RDWR | O_NOCTTY | O_NONBLOCK );    
	 		if ( 0 > fd) {	
	  		printf("/proc/board_status/IpInfo/IpInfo3 open errorn");
			 	return -1;
			}
			break;
	 } 
	 
	 
	 return fd;
}


int AfterMasterIp_fd_return(int num)
{
   int    fd;

	 switch(num)
	 {
		case 0:
		 fd = open( "/proc/board_status/AfterMasterIp/AfterMasterIp0", O_RDWR | O_NOCTTY | O_NONBLOCK );    
		 if ( 0 > fd)
		 {        
			 printf("/proc/board_status/AfterMasterIp/AfterMasterIp0 open errorn");
			 return -1;
		 }
		 break;
	
		case 1:
		 fd = open( "/proc/board_status/AfterMasterIp/AfterMasterIp1", O_RDWR | O_NOCTTY | O_NONBLOCK );    
		 if ( 0 > fd)
		 {        
			 printf("/proc/board_status/AfterMasterIp/AfterMasterIp1 open errorn");
			 return -1;
		 }
		 break;

		case 2:
		 fd = open( "/proc/board_status/AfterMasterIp/AfterMasterIp2", O_RDWR | O_NOCTTY | O_NONBLOCK );    
		 if ( 0 > fd)
		 {        
			 printf("/proc/board_status/AfterMasterIp/AfterMasterIp2 open errorn");
			 return -1;
		 }
		 break;

		case 3:
		 fd = open( "/proc/board_status/AfterMasterIp/AfterMasterIp3", O_RDWR | O_NOCTTY | O_NONBLOCK );    
		 if ( 0 > fd)
		 {        
			 printf("/proc/board_status/AfterMasterIp/AfterMasterIp3 open errorn");
			 return -1;
		 }
		 break;

	 }
	 return fd;
}


int MyLocation_fd_return(void)
{
   int    fd;
	 fd = open( "/proc/board_status/MyLocation", O_RDWR | O_NOCTTY | O_NONBLOCK );    
	 if ( 0 > fd)
	 {        
		 printf("/proc/board_status/MyLocation open errorn");
		 return -1;
	 }
	 return fd;
}


int MasterLocation_fd_return(void)
{
   int    fd;
	 fd = open( "/proc/board_status/MasterLocation", O_RDWR | O_NOCTTY | O_NONBLOCK );    
	 if ( 0 > fd)
	 {        
		 printf("/proc/board_status/MasterLocation open errorn");
		 return -1;
	 }
	 return fd;
}


int DeviceAttached_fd_return(void)
{
   int    fd;
	 fd = open( "/proc/board_status/DeviceAttached", O_RDWR | O_NOCTTY | O_NONBLOCK );    
	 if ( 0 > fd)
	 {        
		 printf("/proc/board_status/DeviceAttached open errorn");
		 return -1;
	 }
	 return fd;
}

#if DEBUG 

int main(void)
{
	int fd;
	char buf[1024];
	int cnt;
	int i;
	fd=IsConnect_fd_return();
	cnt = read(fd,buf,sizeof(buf));
	printf("%s\n",buf);
	
	for(i=0;i<4;i++)
	{
	fd=IpInfo_fd_return(i);
	cnt = read(fd,buf,sizeof(buf));
	printf("%s\n",buf);
	}

	for(i=0;i<4;i++)
	{
	fd=AfterMasterIp_fd_return(i);
	cnt = read(fd,buf,sizeof(buf));
	printf("%s\n",buf);
	}

	fd=MyLocation_fd_return();
	cnt = read(fd,buf,sizeof(buf));
	printf("%s\n",buf);
	
	fd=MasterLocation_fd_return();
	cnt = read(fd,buf,sizeof(buf));
	printf("%s\n",buf);
	
	fd=DeviceAttached_fd_return();
	cnt = read(fd,buf,sizeof(buf));
	printf("%s\n",buf);
}
#endif
