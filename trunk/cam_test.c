/*
 * only for cam testing
 * compile : arm-linux-gcc fb-util.c oo_network.c bmplib.c font_8x8.c cam_test.c -o test
 * run : /.test
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



int main(void)
{

	int cam_fd ;
	int count ;
	
	struct timeval start_tv, end_tv;
	struct timezone tz;
	
	myfb = myfb_open();
	set_vfb_buf(4);
	printf(" set_ vfb buf () \n");

	camif_cfg.dst_x = 320 ;
	camif_cfg.dst_y = 240;

	camif_cfg.bpp = 16;
	camif_cfg.flip = 0 ;

	cam_fd = cam_init();
	
	if (ioctl(cam_fd, CMD_CAMERA_INIT, &camif_cfg)) {
		perror("ioctl");
		exit(1);	
	}

	write(cam_fd,"O",2); // cam start code 
	
	while(1)
	{
		count = read(cam_fd, (unsigned char*)vfb_list[0],320*240*2);
		show_vfb(vfb_list[0]);
	}

	if (cam_fd) close(cam_fd);
	myfb_close();
	free_vfb_buf(4);
	return 0;
	
}

