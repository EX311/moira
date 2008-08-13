#include "read_proc.h"

int main(void)
{
	{
	int IsConnect_place=2;
	char IsConnect_res;
	set_IsConnect(IsConnect_place);
	IsConnect_res=get_IsConnect();
	printf("IsConnect_res = %c\n",IsConnect_res);
	}
/*
	{
	int IpInfo_place;
	int ip=111;
	int cnt;
	char get_ip[4];
		for(IpInfo_place=0;IpInfo_place<4;IpInfo_place++,ip++)
		{
			cnt= set_IpInfo(IpInfo_place, ip);
			printf("set_IpInfo%d : ip=%s,cnt=%d\n",IpInfo_place,ip,cnt);
			
			cnt= get_IpInfo(IpInfo_place,get_ip);
			printf("get_IpInfo%d : ip=%s,cnt=%d\n",IpInfo_place,get_ip,cnt);
		}
	}


	{
	int AfterMasterIp_place;
	int ip=111;
	int cnt;
	char get_ip[4];
		for(AfterMasterIp_place=0;AfterMasterIp_place<4;AfterMasterIp_place++,ip++)
		{
			cnt= set_AfterMasterIp(AfterMasterIp_place, ip);
			printf("set_AfterMasterIp%d : ip=%d,cnt=%d\n",AfterMasterIp_place,ip,cnt);
			
			cnt= get_AfterMasterIp(AfterMasterIp_place,get_ip);
			printf("get_AfterMasterIp%d : ip=%d,cnt=%d\n",AfterMasterIp_place,get_ip,cnt);
		}
	
	}

	{
		int MyLocation=0;
				for(MyLocation=0;MyLocation<4;MyLocation++)
		{
	  	set_MyLocation(MyLocation);
			printf("MyLocation = %d\n",get_MyLocation());
		}

	}

	{
		
		int MasterLocation=0;
		for (MasterLocation=0;MasterLocation<4;MasterLocation++)
		{
			set_MasterLocation(MasterLocation);
			printf("MasterLocation= %d\n",get_MasterLocation());
		}
	}

	{
	int DeviceAttached=0;
	for(;DeviceAttached<4;DeviceAttached++)
	{
		set_DeviceAttached(DeviceAttached);
		printf("DeviceAttached = %d\n",get_DeviceAttached());
	}
	}
	*/
}
