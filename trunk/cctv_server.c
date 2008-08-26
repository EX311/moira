/*

 * theMeal  cctv Server Program
 * code by amon 
 * compile : arm-linux-gcc fb-util.c oo_network.c bmplib.c font_8x8.c cctv_server.c -o cctv_server
 * run : /.cctv_server [my ip]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/fb.h>

#include "oo.h"

#include "/home/amon/work/source/rebis-2.6/drivers/media/video/userapp.h" // linux source tree for rebis

#define CODEC_NAME  "/dev/preview"
#define FB_NAME    "/dev/fb0" 
#define TTY_NAME   "/dev/ttyS0" 

extern struct myfb_info *myfb;

static camif_param_t camif_cfg;
struct myfb_info* myfb ;

static void sig(int sig)
{
	fflush(stderr);
	printf("Quit - Sig #%d caught\n", sig);
	fflush(stdout);
	exit(1);
}

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

camif_param_t cam_set(int width , int height , int bpp)
{

	static camif_param_t tmp;
	
	tmp.dst_x = width ;
	tmp.dst_y = height;
	
	tmp.bpp = bpp;
	tmp.flip = 0 ;

	return tmp;
}

int main(int argc, char *argv[])
{
	int ret, count, offset;
	int serv_sock, clnt_sock;
	//unsigned short *buff;
	char *myip;

	int cam_fd ;
	
	if (argc < 2) {
		printf("USAGE : %s myip\n", argv[0]);
		exit(1);
	}
	myip = argv[1];

	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);

#ifdef DEBUG
	fprintf(stderr, " *** server for theMeal ***\n\n");
#endif
	printf("theMeal: cctv_server started!\n");
	
/* initialize */
	myfb = myfb_open();
	set_vfb_buf(VFB_MAX);
	
	camif_cfg = cam_set(320,240,16);
	cam_fd = cam_init();

	
	if (ioctl(cam_fd, CMD_CAMERA_INIT, &camif_cfg)) {
		perror("ioctl");
		exit(1);	
	}

	write(cam_fd,"O",2); // cam start code 

	serv_sock = tcp_server_listen(ip2port(myip, 8002), 2);
	
	if (serv_sock < 0)
		exit(1);

//	buff = (unsigned short *)malloc(2);
	
/* main process */
	while (1) {
		clnt_sock = tcp_server_accept(serv_sock);
		if (clnt_sock < 0)
		{
			close(clnt_sock);
			sleep(1);
		}

		count = 0;
		offset = 0;

		while(1)
		{
			count = read(cam_fd, (unsigned char*)vfb_list[0],myfb->fbfix.smem_len);
			usleep(300000);
			fb_send(clnt_sock, vfb_list[0], myfb->fbfix.smem_len);

			//	show_vfb(vfb_list[0]);
		}
		close(clnt_sock);
	}
	
	/* Stop Camera */
	write(cam_fd,"X",2);
	
	
	if (cam_fd) close(cam_fd);
	free_vfb_buf(VFB_MAX);
	
	close(serv_sock);
	myfb_close();
	
#ifdef DEBUG
	fprintf(stderr, "\n *** All done completely. ***\n");
#endif
	exit(0);
	return 0;
}
