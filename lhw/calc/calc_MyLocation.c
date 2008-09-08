#include <stdio.h>
#include "read_proc.h"
#include <stdlib.h>
#include <string.h>

int main(void)
{
	char IsConnect;
 	int stat;
	IsConnect = get_IsConnect();
//	printf("%c\n",IsConnect);
//	stat=atoi(&IsConnect);

	switch(IsConnect)
	{
		case 0:	//0000
			set_MyLocation(5);
			return 5;	//disconnect
			break;
	
		case 1:	//0001
			set_MyLocation(2);
			return 2;
			break;
		
		case 2:	//0010
			set_MyLocation(1);
			return 1;
			break;
		
		case 4: //0100
			set_MyLocation(0);
			return 0;
			break;
		
		case 8: //1000
			set_MyLocation(0);
			return 0;
			break;
		
		case 3: //0011
			set_MyLocation(3);
			return 3;
			break; 
		
		case 6: //0110
			set_MyLocation(1);
			return 1;
			break;
		
		case 12: //1100
			set_MyLocation(0);
			return 0;
			break;
		
		case 9: //1001
			set_MyLocation(2);
			return 2;
			break;
	
	}
	return 1;

}
