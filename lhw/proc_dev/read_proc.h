#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>

#define DEBUG 1

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

int IpInfo_fd_return(void)
{
   int    fd;
	 fd = open( "/proc/board_status/IpInfo", O_RDWR | O_NOCTTY | O_NONBLOCK );    
	 if ( 0 > fd)
	 {        
		 printf("/proc/board_status/IpInfo open errorn");
		 return -1;
	 }
	 return fd;
}


int AfterMasterIp_fd_return(void)
{
   int    fd;
	 fd = open( "/proc/board_status/AfterMasterIp", O_RDWR | O_NOCTTY | O_NONBLOCK );    
	 if ( 0 > fd)
	 {        
		 printf("/proc/board_status/AfterMasterIp open errorn");
		 return -1;
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

#ifdef DEBUG
int main(void)
{
	int fd;
	char buf[1024];
	int cnt;
	fd=IsConnect_fd_return();
	cnt = read(fd,buf,sizeof(buf));
	printf("%s\n",buf);
	
	fd=IpInfo_fd_return();
	cnt = read(fd,buf,sizeof(buf));
	printf("%s\n",buf);
	
	fd=AfterMasterIp_fd_return();
	cnt = read(fd,buf,sizeof(buf));
	printf("%s\n",buf);
	
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
