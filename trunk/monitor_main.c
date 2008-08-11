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

#include "font.h"
#include "oo.h"
#include "fb-util.h"

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
extern unsigned short *vfb_list[VFB_MAX];

static void sig(int sig)
{
	fflush(stderr);
	printf("Quit - Sig #%d caught\n", sig);
	exit(2);
}

void put_char(int x, int y, int c, int colidx)
{
	int i,j,bits;
	for (i = 0; i < font_vga_8x8.height; i++) {
		bits = font_vga_8x8.data [font_vga_8x8.height * c + i];
		for (j = 0; j < font_vga_8x8.width; j++, bits <<= 1)
			if (bits & 0x80)
				drow_pixel(x+j, y+i, white);
	}
}

void put_string(int x, int y, char *s, unsigned colidx)
{
	int i;
	for (i = 0; *s; i++, x += font_vga_8x8.width, s++)
		put_char (x, y, *s, colidx);
}

void put_string_center(int x, int y, char *s, unsigned colidx)
{
	size_t sl = strlen (s);
	put_string (x - (sl / 2) * font_vga_8x8.width, y - font_vga_8x8.height / 2, s, colidx);
}

void clear_screen(void)
{
	memset(myfb->fb, 0, myfb->fbvar.xres*myfb->fbvar.yres*16/8);
	put_string (myfb->fbvar.xres/4, myfb->fbvar.yres/4-25, ">> theMeal Project Monitor program", 1);
	put_string (myfb->fbvar.xres/4, myfb->fbvar.yres/4-10, ">> Code by amoolove", 2);
	put_string (myfb->fbvar.xres/2+150 , myfb->fbvar.yres/2 + myfb->fbvar.yres/4, "theMeal Project 2008", 3);
}

void show_grid(void)
{
	drow_line(myfb->fbvar.xres/2, myfb->fbvar.yres/2-YRES, myfb->fbvar.xres/2, myfb->fbvar.yres/2+YRES, white);
	drow_line(myfb->fbvar.xres/2-XRES, myfb->fbvar.yres/2, myfb->fbvar.xres/2+XRES, myfb->fbvar.yres/2, white);
	drow_rect(myfb->fbvar.xres/2-XRES, myfb->fbvar.yres/2-YRES, myfb->fbvar.xres/2+XRES, myfb->fbvar.yres/2+YRES, white);
}

int main(int argc, char *argv[])
{
	int i, ret;
	int serv_sock, clnt_sock;
	struct oo_fb_data *fb_data;
	char buff[BUFSIZE];

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

		printf("clnt sock #%d\n", clnt_sock);
		while ( (ret = read(clnt_sock, buff, BUFSIZE)) != 0) {
			printf("READ: #%d\n", ret);
			for (i=0; i<ret/sizeof(struct oo_fb_data); i++) {
				fb_data = (struct oo_fb_data *)(buff+i*sizeof(struct oo_fb_data));
				*(myfb->fb+fb_data->offset) = fb_data->pix_data;
			}
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
