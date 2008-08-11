#include <string.h>
#include "board_status.h"

void initall()
{
	int i;
	bs.is_connect = 0;
	bs.device_attached = 0;

	/* 4 is initial value.. location number must be in range of 0-3 after mastering. */
	bs.my_location = 4;
	
	/* same reason */
	bs.master_location = 4; 

	for(i=0;i<3;i++){
		bs.ip_info[i] = NULL;
		bs.mastered_ip_info[i] = NULL;
	}
	

}

unsigned char get_connection_status()
{
	return bs.is_connect;
}

unsigned char *get_ip_info(unsigned int ir_loc)
{
	return bs.ip_info[ir_loc];
}

unsigned char *get_mastered_ip_info(unsigned int loc)
{
	return bs.mastered_ip_info[loc];
}

unsigned int get_my_location()
{
	return bs.mu_location;
}

unsigned int get_master_location()
{
	return bs.master_location;
}

unsigned int get_device_attached()
{
	return bs.device_attached;
}

void set_connection_status(unsigned char connect)
{
	bs.is_connect = connect;
}

void set_ip_info(unsigned char *ip,unsigned int ir_loc)
{
	int len;
	len = strlen(ip) +1;
	if(bs.ip_info[ir_loc]!=NULL)
		kfree(bs.ip_info[ir_loc]);
	bs.ip_info[ir_loc] = kmalloc(len,GFP_KERNEL);
	strcpy(bs.ip_info[ir_loc],ip);
}

void set_mastered_ip_info(unsigned char *ip,unsigned int loc)
{
	int len;
	len = strlen(ip) +1;
	if(bs.mastered_ip_info[loc]!=NULL)
		kfree(bs.mastered_ip_info[loc]);
	bs.mastered_ip_info[loc] = kmalloc(len,GFP_KERNEL);
	strcpy(bs.mastered_ip_info[loc],ip);
}

void set_my_location(unsigned int loc)
{
	bs.my_location = loc;
}

void set_master_location(unsigned int loc)
{
	bs.master_location = loc;
}

void set_device_attached(unsigned int num_device)
{
	bs.device_attached = num_device;
}	


