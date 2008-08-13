/*
 * fbbmp.c : bmp drawing on frame buffer example
 *
 * Copyright(C) 2002 holelee
 *
 */

#include <stdio.h>
#include <stdlib.h> /* for exit */
#include <string.h>
#include <unistd.h> /* for open/close .. */
#include <fcntl.h>  /* for O_RDONLY */
#include <sys/ioctl.h> /* for ioctl */
#include <sys/mman.h> /* for mmap */
#include <linux/fb.h>  /* for fb_var_screeninfo, FBIOGET_VSCREENINFO */
#include "oo.h"

typedef unsigned char ubyte;
struct color black = {0,0,0};

extern struct myfb_info *myfb;

int main(int argc, char *argv[])
{
	int i, ret;
	int sock[VFB_MAX];
	char *ipaddr[VFB_MAX] = {"192.168.1.10", "192.168.77.30", "192.168.77.55", "192.168.77.77"};
	struct oo_fb_data *buf_monitor;
	bmphandle_t bh;

	if (argc < 2) {
		printf("USAGE %s bmpfile\n", argv[0]);
		exit(1);
	}

	myfb = myfb_open();

	sock[0] = tcp_client_connect(ipaddr[0], 8192);
	if (sock < 0) {
		perror("tcp connect");
		exit(1);
	}
	sock[1] = tcp_client_connect(ipaddr[1], ip2port(ipaddr[1]));
	if (sock < 0) {
		perror("tcp connect");
		exit(1);
	}
	sock[2] = tcp_client_connect(ipaddr[2], ip2port(ipaddr[2]));
	if (sock < 0) {
		perror("tcp connect");
		exit(1);
	}
/*
	sock[3] = tcp_client_connect(ipaddr[3], ip2port(ipaddr[3]));
	if (sock < 0) {
		perror("tcp connect");
		exit(1);
	}
*/
	bh = bmp_open(argv[1]);
	if (bh == NULL) {
		perror("bmp_open");
		exit(1);
	}

	clear_screen();

	set_vfb_buf(VFB_MAX);
	buf_monitor = alloc_net_buf(sizeof(struct oo_fb_data)*bmp_width(bh)*bmp_height(bh)*16/8);
	/* main loop */
	buf_bmp(bh, 0, 0);

	fb_send(sock[1], vfb_list[1], myfb->fbfix.smem_len);
	fb_send(sock[2], vfb_list[2], myfb->fbfix.smem_len);
//	fb_send(sock[3], vfb_list[3], myfb->fbfix.smem_len);

	monitor_bmp(bh, 0, 0, buf_monitor);
	ret = write(sock[0], buf_monitor, sizeof(struct oo_fb_data)*bmp_width(bh)*bmp_height(bh)*16/8);

	show_vfb(vfb_list[0]);
	/* clean up */
	free_net_buf(buf_monitor);
	free_vfb_buf(VFB_MAX);
	bmp_close(bh);

	for (i=0; i<VFB_MAX; i++)
		close(sock[i]);

	myfb_close();
	printf("All Done!\n");
	exit(0);
	return 0;
}

