/*
 * only for cam testing
 * compile : arm-linux-gcc fb-util.c oo_network.c bmplib.c font_8x8.c vidio_call.c -o vidio_call
 * run : /.vidio_call [ip]
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "oo.h"


#include "/home/amon/work/source/rebis-2.6/drivers/media/video/userapp.h" // linux source tree for rebis

#define CODEC_NAME  "/dev/preview"

#define FB_NAME    "/dev/fb0" 
#define TTY_NAME   "/dev/ttyS0" 

static camif_param_t camif_cfg;

struct myfb_info* myfb ;

static inline int cam_init(void)
{
	int dev_fp = -1;
	dev_fp = open(CODEC_NAME, O_RDWR);
	
	if (dev_fp < 0) {
		perror(CODEC_NAME);
		return -1;
	}

	printf(" cam init() \n");
	return dev_fp;
}



int main( int arvc, char** argv)
{

	int cam_fd ;
	int count ;
	pid_t pid;

	struct timeval start_tv, end_tv;
	struct timezone tz;
	
	////////////////////////////////////////////////////////
	
	int i, ret;
	int sock;
	char *ipaddr = argv[1];
			
	myfb = myfb_open();
	
	sock = tcp_client_connect(ipaddr, 8192);

	if (sock < 0) {
		perror(ipaddr);
	}

	sock = tcp_client_connect(ipaddr, ip2port(ipaddr));
	
	if (sock < 0) 
		fprintf(stderr, "%s connect error\n", ipaddr);
	
	
	set_vfb_buf(VFB_MAX);
	printf(" set_ vfb buf () \n");

	camif_cfg.dst_x = 320 ;
	camif_cfg.dst_y = 240;
	
	
//	camif_cfg.dst_x = 640 ;
//	camif_cfg.dst_y = 480;

	camif_cfg.bpp = 16;
	camif_cfg.flip = 0 ;

	cam_fd = cam_init();
	
	if (ioctl(cam_fd, CMD_CAMERA_INIT, &camif_cfg)) {
		perror("ioctl");
		exit(1);	
	}

	write(cam_fd,"O",2); // cam start code 
	printf("cam start\n");

//	if((pid = fork()) >0)
//	{
		while(1)
		{
			count = read(cam_fd, (unsigned char*)vfb_list[0],320*240*2);
			usleep(220000);
		//	for (i=1; i<VFB_MAX; i++)
				if (sock > 0)
				{
				fb_send(sock, vfb_list[0], myfb->fbfix.smem_len);
				//fb_send(sock[i], vfb_list[i], myfb->fbfix.smem_len);
				}

		//	show_vfb(vfb_list[0]);
		
		}
//	}//end if

	/* Stop Camera */
	write(cam_fd,"X",2);
	
	
	if (cam_fd) close(cam_fd);
	myfb_close();
	free_vfb_buf(VFB_MAX);
	return 0;
	
}

