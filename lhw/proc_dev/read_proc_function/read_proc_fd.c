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
	/*
	 * we need a library function to find out how many devices are attached with our device by calculating buf.
	 * buf needs to be converted to integer first.
	 * then we should convert decimal to binary to get this(1111 or 0001 ... 0101 )or we could just calculate.
	 * 1111 means we have four devices attached on top bottom left right side of IRDA sensor.
	 * 0001 means we have one devices attached on left side of IRDA.
	 * 0010 is top side.
	 * 0100 is right side.
	 * 1000 is bottom side.
	 */
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
