#include "read_proc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int main(void)
{
	int master;
//	printf("start_UI process start\n");
	
//	printf("Which device is Master??\n");
	scanf("%d",&master);
	
	printf("select %d!!\n",master);
 	set_MasterLocation(master);
	printf("MasterLcation Set End!\n");	

	
	
		execl("./Master","MasterIpSet",0);

}
