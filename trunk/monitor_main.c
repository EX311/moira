/* 
 * theMeal Project Monitor Program
 * code by amoolove
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "oo.h"

#define	MIN(x, y)	((x) > (y) ? (y) : (x))
#define	PORT	8192
#define	BUFSIZE	8192*16

#define	WHITE	0xff
#define	BLACK	0x0
#define	XRES	320
#define	YRES	240

typedef unsigned char ubyte;
struct color white = {0xff, 0xff, 0xff};
struct color black = {0,0,0};

extern struct myfb_info *myfb;

static void sig(int sig)
{
	fflush(stderr);
	printf("Quit - Sig #%d caught\n", sig);
	exit(2);
}

void put_monitor(void)
{
	put_string (myfb->fbvar.xres/4, myfb->fbvar.yres/4-25, ">> theMeal Project Monitor program", white);
	put_string (myfb->fbvar.xres/4, myfb->fbvar.yres/4-10, ">> Code by amoolove", white);
	put_string (myfb->fbvar.xres/2+150 , myfb->fbvar.yres/2 + myfb->fbvar.yres/4, "theMeal Project 2008", white);
}

void show_grid(void)
{
	put_monitor();
	drow_line(myfb->fbvar.xres/2, myfb->fbvar.yres/2-YRES, myfb->fbvar.xres/2, myfb->fbvar.yres/2+YRES, white);
	drow_line(myfb->fbvar.xres/2-XRES, myfb->fbvar.yres/2, myfb->fbvar.xres/2+XRES, myfb->fbvar.yres/2, white);
	drow_rect(myfb->fbvar.xres/2-XRES, myfb->fbvar.yres/2-YRES, myfb->fbvar.xres/2+XRES, myfb->fbvar.yres/2+YRES, white);
}

int main(int argc, char *argv[])
{
	int ret, cnt, i;
	int serv_sock, clnt_sock;
	unsigned short *start_fbp;
	char buff[BUFSIZE];
	struct oo_fb_data *buf_data;

	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);

	printf(" *** Monitor for theMeal ***\n\n");
	
	myfb = myfb_open();

	serv_sock = tcp_server_listen(PORT, 2);
	if (serv_sock < 0)
		exit(1);
/* initialize */
	clear_screen();
	show_grid();
	
/* main process */
	while (1) {
		clnt_sock = tcp_server_accept(serv_sock);
		if (clnt_sock < 0)
			exit(1);
#ifdef DEBUG
		printf("CONN SOCK #%d\n", clnt_sock);
#endif
		cnt = 0;
		start_fbp = myfb->fb + (272 * myfb->fbvar.xres + 320);

		while ( (ret = read(clnt_sock, buff, BUFSIZE)) != 0) {
			cnt += ret;
#ifdef DEBUG
			printf("READ: #%d Total: #%d\n", ret, cnt);
#endif
			for (i=0; i<ret; i+=sizeof(struct oo_fb_data)) {
				buf_data = (struct oo_fb_data *)&(buff[i]);
				*(start_fbp+buf_data->offset) = buf_data->pix_data;
				memcpy(start_fbp+(buf_data->offset), &(buf_data->pix_data), sizeof(unsigned short));
			}
			cnt = 0;
			show_grid();
		}
		close(clnt_sock);
	}
	
	close(serv_sock);
	myfb_close();
	
	printf("\n *** All done completely. ***\n");
	exit(0);
	return 0;
}
