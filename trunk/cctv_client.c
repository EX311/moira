/*
 *  cctv_client 
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <signal.h>

#include "oo.h"



#define FB_NAME    "/dev/fb0" 
#define TTY_NAME   "/dev/ttyS0" 


struct myfb_info* myfb ;
char *taget_ip ;
char *ipaddr[VFB_MAX] = {"192.168.77.20","192.168.77.30","192.168.77.55","192.168.77.77"};

static FILE * tty_fp;
unsigned int event = 0; 

unsigned int cam_id ;

void change_view_handler(int n) 
{
	event = 1;
}





int main( int arvc, char** argv)
{

	int count = 0 ;
	int offset = 0 ;
	pid_t child_id;
	int key_buf;

	int i, ret;
	int sock;
	
	cam_id = 1 ;

	myfb = myfb_open();
	set_vfb_buf(VFB_MAX);
	
	
	if(child_id = fork()) {
		tty_fp = fopen(TTY_NAME, "w+");
		setvbuf(tty_fp, NULL, _IONBF, 0);
		while(1) {
			key_buf = getchar();		
			//read(tty_fp, &key_buf, 1);
			if(key_buf == 's')
				kill(child_id, SIGUSR1);
		}
	}

	signal(SIGUSR1, change_view_handler);
	fflush(stdout);

	while(1)
	{
	
	sock = tcp_client_connect(ipaddr[cam_id], ip2port(ipaddr[cam_id],8002));
	
	if (sock < 0) 
		fprintf(stderr, "%s connect error\n", ipaddr[cam_id]);
	

	while(1)
	{
		//ret = read(sock,(unsigned char*)vfb_list[cam_id] + offset, myfb->fbfix.smem_len);
		ret = read(sock,(unsigned char*)vfb_list[cam_id] , myfb->fbfix.smem_len);
		usleep(10000);
		//ret = read(sock,(unsigned char*)vfb_list[cam_id] + offset, BUFSIZ);
		printf(" 	debug :: ret is %d \n",ret);
		count += ret;
		//count += ret/sizeof(unsigned short);
		//offset += ret/sizeof(unsigned short);


	//	usleep(10000);
		if( count == myfb->fbfix.smem_len)
		{
			printf("	 debug: call vfb_show \n ");
			show_vfb(vfb_list[cam_id]);
			count = 0;
		}
/*
		if(event) {
			event = 0;
			cam_id = (cam_id + 1)%2;
			break; 
			}
*/		
	}

	
	}


	close(sock);
	myfb_close();
	free_vfb_buf(VFB_MAX);
	return 0;
	
}

