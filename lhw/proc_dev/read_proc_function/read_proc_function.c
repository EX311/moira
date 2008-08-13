#include "read_proc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG 1

char get_IsConnect(void)
{
	int fd;
	char buf[4]={0,};
	int cnt;
	fd=IsConnect_fd_return();
	cnt = read(fd,buf,strlen(buf));

#ifdef DEBUG
	printf("get_IsConnect return value = %c\n",buf[0]);
#endif
	return buf[0];
}

int set_IsConnect(int  place)
{
	int fd;
	char buf[4];

	sprintf(buf,"%d",place);

	fd=IsConnect_fd_return();
	write(fd,buf,strlen(buf));
	
#ifdef DEBUG
	printf("set_IsConnect set value = %d\n",place);
#endif
	return 1;
}

int get_IpInfo(int place, char* get_data)
{
	int fd;
	int cnt;
	int i;

	fd=IpInfo_fd_return(place);
	cnt = read(fd,get_data,strlen(get_data));
	#ifdef DEBUG
	printf("get_IpInfo return value = %s\n",get_data);
	#endif

return cnt;
}

int set_IpInfo(int place, int ip)
{
	int fd;
	char buf[1024]={0,};
	int cnt;
	int i;

	sprintf(buf,"%d",ip);

	fd=IpInfo_fd_return(place);
	write(fd,&buf,strlen(buf));
#ifdef DEBUG
	printf("set_IpInfo set value = %s\n",buf);
#endif
	return 1;
}

int get_AfterMasterIp(int place,char* get_data)
{
	int fd;
	int cnt;
	int i;

	fd=AfterMasterIp_fd_return(place);
	cnt = read(fd,get_data,strlen(get_data));
#ifdef DEBUG
	printf("get_AfterMasterIp return value = %s\n",get_data);
#endif

return 0;

}

int set_AfterMasterIp(int place, int ip)
{
	int fd;
	char buf[1024];
	int cnt;
	int i;
	sprintf(buf,"%d",ip);

	fd=AfterMasterIp_fd_return(place);
	write(fd,buf,strlen(buf));
#ifdef DEBUG
		printf("set_AfterMasterIp set value = %s\n",buf);
#endif

	return 0;

}

int get_MyLocation(void)
{
	int fd;
	char buf;
	int cnt;
	int i;
	
	fd=MyLocation_fd_return();
	cnt = read(fd,&buf,sizeof(buf));
	#ifdef DEBUG
	printf("get_MyLocation return value = %c\n",buf);
#endif

	return 0;

}

int set_MyLocation(int MyLocation)
{
	int fd;
	char buf;
	int cnt;
	int i;
	
	sprintf(&buf,"%d",MyLocation);
	fd=MyLocation_fd_return();
	write(fd,&buf,1);
#ifdef DEBUG
		printf("set_MyLocation set value = %c\n",buf);
#endif

	return 0;

}

int get_MasterLocation(void)
{
	int fd;
	char buf;
	int cnt;

	fd=MasterLocation_fd_return();
	cnt = read(fd,&buf,sizeof(buf));
	#ifdef DEBUG
	printf("get_IsConnect return value = %c\n",buf);
#endif

	return 0;

}

int set_MasterLocation(int MasterLocation)
{
	int fd;
	char buf;
	int cnt;
	int i;

	sprintf(&buf,"%d",MasterLocation);
	fd=MasterLocation_fd_return();
	write(fd,&buf,1);
	#ifdef DEBUG
		printf("set_MasterLocation set value = %c\n",buf);
#endif

	return 0;

}

int get_DeviceAttached(void)
{
	int fd;
	int cnt;
	char buf;
	int i;

	fd=DeviceAttached_fd_return();
	cnt = read(fd,buf,sizeof(buf));
	#ifdef DEBUG
	printf("get_DeviceAttached return value = %c\n",buf);
#endif

	return 0;

}

int set_DeviceAttached(int DeviceAttached)
{
	int fd;
	char buf;
	int cnt;
	int i;

	sprintf(&buf,"%d",DeviceAttached);
	fd=DeviceAttached_fd_return();
	write(fd,&buf,1);
#ifdef DEBUG
		printf("set_DeviceAttached set value = %c\n",buf);
#endif

	return 0;

}

