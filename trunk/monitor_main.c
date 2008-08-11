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

static void sig(int sig)
{
	fflush(stderr);
	printf("Quit - Sig #%d caught\n", sig);
	exit(2);
}

void put_char(int x, int y, int c, int colidx, unsigned short *pfbmap, struct fb_var_screeninfo *fbvar)
{
	int i,j,bits;
	for (i = 0; i < font_vga_8x8.height; i++) {
		bits = font_vga_8x8.data [font_vga_8x8.height * c + i];
		for (j = 0; j < font_vga_8x8.width; j++, bits <<= 1)
			if (bits & 0x80)
				*(pfbmap+(y+i)*fbvar->xres+(x+j)) = makepixel(white);
	}
}

void put_string(int x, int y, char *s, unsigned colidx, unsigned short *pfbmap, struct fb_var_screeninfo *fbvar)
{
	int i;
	for (i = 0; *s; i++, x += font_vga_8x8.width, s++)
		put_char (x, y, *s, colidx, pfbmap, fbvar);
}

void put_string_center(int x, int y, char *s, unsigned colidx, unsigned short *pfbmap, struct fb_var_screeninfo *fbvar)
{
	size_t sl = strlen (s);
	put_string (x - (sl / 2) * font_vga_8x8.width, y - font_vga_8x8.height / 2, s, colidx, pfbmap, fbvar);
}

void clear_screen(unsigned short *pfbmap, struct fb_var_screeninfo *fbvar)
{
	int i, j;
	for (i = 0; i < fbvar->yres; i++)
		for (j = 0; j < fbvar->xres; j++)
			*(pfbmap+i*fbvar->xres+j) = makepixel(black);
	put_string (fbvar->xres/4, fbvar->yres/8, ">> theMeal Project Monitor program", 1, pfbmap, fbvar);
	put_string (fbvar->xres/4, fbvar->yres/8+20, ">> Code by amoolove", 2, pfbmap, fbvar);
	put_string (fbvar->xres/2+150 , fbvar->yres/2 + fbvar->yres/4, "theMeal Project 2008", 3, pfbmap, fbvar);
}

void show_grid(unsigned short *pfbmap, struct fb_var_screeninfo *fbvar)
{
	int i, j;

	for (i = (fbvar->yres/2)-YRES; i<= (fbvar->yres/2)+YRES; i++)
		for (j = (fbvar->xres/2)-XRES; j<= (fbvar->xres/2)+XRES; j+=XRES)
			*(pfbmap+i*fbvar->xres+j) = makepixel(white);
	for (i = (fbvar->yres/2)-YRES; i<= (fbvar->yres/2)+YRES; i+=YRES)
		for (j = (fbvar->xres/2)-XRES; j<= (fbvar->xres/2)+XRES; j++)
			*(pfbmap+i*fbvar->xres+j) = makepixel(white);
}

int main(int argc, char *argv[])
{
	int i, ret;
	int fb_fd, serv_sock, clnt_sock;
	unsigned short *pfbmap;
	struct oo_fb_data *fb_data;
	struct fb_var_screeninfo fbvar;
	char buff[BUFSIZE];

	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);

	printf(" *** Monitor for theMeal ***\n\n");

	fb_fd = open(FBDEVFILE, O_RDWR);
	if (fb_fd < 0) {
		perror("fb open");
		exit(1);
	}

	ret = ioctl(fb_fd, FBIOGET_VSCREENINFO, &fbvar);
	if (ret < 0) {
		perror("fb ioctl");
		exit(1);
	}

	if (fbvar.bits_per_pixel != 16) {
		fprintf(stderr, "bpp is NOT 16\n");
		exit(1);
	}

	pfbmap = (unsigned short *)mmap(0, fbvar.xres*fbvar.yres*16/8, PROT_READ|PROT_WRITE, MAP_SHARED, fb_fd, 0);
	if ( (unsigned)pfbmap == (unsigned)-1) {
		perror("fb mmap");
		exit(1);
	}
	
	serv_sock = tcp_server_listen(PORT, 2);
	if (serv_sock < 0)
		exit(1);
/* initialize */
	clear_screen(pfbmap, &fbvar);
	show_grid(pfbmap, &fbvar);
	
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
				*(pfbmap+fb_data->offset) = fb_data->pix_data;
			}
			show_grid(pfbmap, &fbvar);
		}
		close(clnt_sock);
	}
	
	close(serv_sock);
	munmap(pfbmap, fbvar.xres*fbvar.yres*16/8);
	close(fb_fd);
	
	printf("\n *** All done completely. ***\n");
	exit(0);
	return 0;
}
