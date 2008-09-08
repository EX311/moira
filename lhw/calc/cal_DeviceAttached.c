#include <stdio.h>
#include "read_proc.h"
#include <stdlib.h>

int main(void)
{

	char res;
	res = get_IsConnect();
	switch(res)
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
	execl(".././fork_calc","fork_calc",0);

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
	return res;
}
