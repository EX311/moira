/*
 * fbbmp.c : bmp drawing on frame buffer example
 *
 * Copyright(C) 2002 holelee
 * modified by amoolove
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
	int i, j, ret;
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
	if (sock[0] < 0) {
		perror(ipaddr[0]);
	}

	for (i=1; i<VFB_MAX; i++) {
		sock[i] = tcp_client_connect(ipaddr[i], ip2port(ipaddr[i], 8000));
		if (sock[i] < 0) 
			fprintf(stderr, "%s connect error\n", ipaddr[i]);
	}

	bh = bmp_open(argv[1]);
	if (bh == NULL) {
		perror("bmp_open");
		exit(1);
	}

	set_vfb_buf(VFB_MAX);
	buf_monitor = alloc_net_buf(sizeof(struct oo_fb_data)*bmp_width(bh)*bmp_height(bh)*16/8);
	/* main loop */
	buf_bmp(bh, 0, 0);
	for (i=1; i<VFB_MAX; i++) {
		if (sock[i] > 0) {
			fb_send(sock[i], vfb_list[i], myfb->fbfix.smem_len);
		}
	}
	show_vfb(vfb_list[0]);
	monitor_bmp(bh, 0, 0, buf_monitor);
	ret = write(sock[0], buf_monitor, sizeof(struct oo_fb_data)*bmp_width(bh)*bmp_height(bh)*16/8);

	/* clean up */
	free_net_buf(buf_monitor);
	free_vfb_buf(VFB_MAX);
	bmp_close(bh);

	for (i=0; i<VFB_MAX; i++)
		if (sock[i] > 0)
			close(sock[i]);

	myfb_close();
	printf("All Done!\n");
	exit(0);
	return 0;
}

