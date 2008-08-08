#include "board_status.h"

extern void initall()
{

}

extern unsigned char get_connection_status()
{
	return bs.is_connect;
}

extern unsigned char *get_ip_info(unsigned int ir_loc)
{
	return bs.ip_info[ir_loc];
}

extern unsigned char *get_mastered_ip_info(unsigned int loc)
{
	return bs.mastered_ip_info[loc];
}

extern unsigned int get_my_location()
{
	return bs.mu_location;
}

extern unsigned int get_master_location()
{
	return bs.master_location;
}

extern unsigned int get_device_attached()
{
	return bs.device_attached;
}

extern void set_connection_status(unsigned char connect)
{
	bs.is_connect = connect;
}

extern void set_ip_info(unsigned char *ip,unsigned int ir_loc)
{
	int len;
	len = strlen(ip) +1;
	if(bs.ip_info[ir_loc]!=NULL)
		kfree(bs.ip_info[ir_loc]);
	bs.ip_info[ir_loc] = kmalloc(len,GFP_KERNEL);
	strcpy(bs.ip_info[ir_loc],ip);
}

extern void set_mastered_ip_info(unsigned char *ip,unsigned int loc)
{
	int len;
	len = strlen(ip) +1;
	if(bs.mastered_ip_info[loc]!=NULL)
		kfree(bs.mastered_ip_info[loc]);
	bs.mastered_ip_info[loc] = kmalloc(len,GFP_KERNEL);
	strcpy(bs.mastered_ip_info[loc],ip);
}

extern void set_my_location(unsigned int loc)
{
	bs.my_location = loc;
}

extern void set_master_location(unsigned int loc)
{
	bs.master_location = loc;
}

extern void set_device_attached(unsigned int num_device)
{
	bs.device_attached = num_device;
}	


