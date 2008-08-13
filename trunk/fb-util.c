/*
 * fb utils 
 */

#include <stdio.h>
#include <stdlib.h> /* for exit */
#include <unistd.h> /* for open/close .. */
#include <fcntl.h> /* for O_RDWR */
#include <sys/ioctl.h> /* for ioctl */
#include <linux/fb.h> /* for fb_var_screeninfo, FBIOGET_VSCREENINFO */
#include <sys/mman.h> /* for mmap */
#include <string.h> /*for memcpy */

#include "oo.h"

#define DEBUG 0

/*
 *
#if DEBUG
printf(" message \n");
#endif

*/

struct myfb_info *myfb;
unsigned short *vfb_list[VFB_MAX];

unsigned short makepixel(struct color color)
{
	unsigned char r = color.r;
	unsigned char g = color.g;
	unsigned char b = color.b;

	return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}

void drow_pixel(int x, int y, struct color color)
{
	unsigned short pixel;
	int offset ;
	
	offset = y * myfb->fbvar.xres + x;

	pixel = makepixel(color);

	if(offset > -1 && offset <= myfb->fbfix.smem_len )
		*(myfb->fb+offset) = pixel;
}

void drow_line(int x1, int y1, int x2, int y2, struct color color)
{
	int tmp;
	int dx = x2 - x1;
	int dy = y2 - y1;

	if (abs (dx) < abs (dy)) {
		if (y1 > y2) {
			tmp = x1; x1 = x2; x2 = tmp;
			tmp = y1; y1 = y2; y2 = tmp;
			dx = -dx; dy = -dy;
		}
		x1 <<= 16;
		/* dy is apriori >0 */
		dx = (dx << 16) / dy;
		while (y1 <= y2) {
			drow_pixel(x1>>16, y1, color);
			x1 += dx;
			y1++;
		}
	} else {
		if (x1 > x2) {
			tmp = x1; x1 = x2; x2 = tmp;
			tmp = y1; y1 = y2; y2 = tmp;
			dx = -dx; dy = -dy;
		}
		y1 <<= 16;
		dy = dx ? (dy << 16) / dx : 0;
		while (x1 <= x2) {
			drow_pixel(x1,y1>>16,color);
			y1 += dy;
			x1++;
		}
	}
}


void drow_rect (int x1, int y1, int x2, int y2, struct color color)
{
	drow_line (x1, y1, x2, y1, color);
	drow_line (x2, y1, x2, y2, color);
	drow_line (x2, y2, x1, y2, color);
	drow_line (x1, y2, x1, y1, color);
}


void set_vfb_buf(int n)
{
	
	int i ;

	for(i =0 ; i<n; i++)
	{
		vfb_list[i]= (unsigned short*)malloc(myfb->fbfix.smem_len);

		if(vfb_list[i] == 0)
		{
			perror("fb_list malloc error \n");
			exit(1);
		}
	}
}

void  free_vfb_buf(int n)
{
	int i;
	for (i = 0; i < n; i++) {
		if (vfb_list[i])
			free(vfb_list[i]);
	}
}

/*
 * buf_pixel func uses only master,
 * master saves each vfb data and send data 
 * each vfb data is vfb_list[location]
 *
 */
void buf_pixel(int x, int y, struct color color)
{
	unsigned short pixel;
	int offset ;
	int location  ;

	if ( -1  < x && x <320 && -1< y  && y<240)
	{
		location = 0;
		
	}
	if ( 319 < x && x <640 && -1< y  && y<240)
	{
		location = 1;
		x-=320; 
	}
	
	if ( -1 < x && x <320 && 239< y  && y<480)
	{
		location = 2;
		y-=240;
	}
	
	if ( 319 < x && x <640 && 239< y  && y<480)
	{
		location = 3;
		x-=320;
		y-=240;
		
	}

	offset =y * myfb->fbvar.xres + x;
	pixel = makepixel(color);
		
	if(offset > -1 && offset <= myfb->fbfix.smem_len )
		*(vfb_list[location]+offset) = pixel ;
}

void show_vfb(unsigned short* vfb)
{
	//memset( , 0, myfb->fbfix.smem_len);
	memcpy( (void*)myfb->fb,vfb, myfb->fbfix.smem_len);
	memset( vfb, 0, myfb->fbfix.smem_len);
}
	
struct myfb_info* myfb_open (void)
{

		int ret ;

		myfb =(struct myfb_info*) malloc(sizeof( struct myfb_info));
		if (!myfb) {
			perror("myfb malloc");
			exit(1);
		}

		myfb->fd = open(FBDEVFILE, O_RDWR);

		if(myfb->fd < 0)
		{
			perror("fbdev open");
			exit(1);
		}


		//ret=ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);
		ret=ioctl(myfb->fd, FBIOGET_VSCREENINFO, &myfb->fbvar);

		if(ret < 0)
		{
			perror("fbdev ioctl(FSCREENINFO)");
			exit(1);
		}


		//ret = ioctl(fbfd, FBIOGET_FSCREENINFO, &fbfix);
		ret=ioctl(myfb->fd, FBIOGET_FSCREENINFO, &myfb->fbfix);

		if(ret < 0)
		{
			perror("fbdev ioctl(FSCREENINFO)");
			exit(1);
		}



		myfb->fb=(unsigned short *)mmap(0, myfb->fbvar.xres*myfb->fbvar.yres*16/8,PROT_READ|PROT_WRITE, MAP_SHARED, myfb->fd, 0);

		memset (myfb->fb,0, myfb->fbvar.xres*myfb->fbvar.yres*16/8);

		if (myfb->fb == 0)
		{
			perror("mmap err\n");
			exit(1);
		}

		return myfb;
}

/*
 * Under this line Added by amoolove
 */

void myfb_close(void)
{
	munmap(myfb->fb, myfb->fbvar.xres*myfb->fbvar.yres*16/8);
	close(myfb->fd);
	free(myfb);
}

void buf_bmp(bmphandle_t bh, int x, int y)
{
	int i, j;
	struct color pixel;
	for (i = 0; i < bmp_height(bh); i++)
		for (j = 0; j < bmp_width(bh); j++) {
			pixel = bmp_getpixel(bh, j, i);
			buf_pixel(j+x,i+y,pixel);
		}
}

void monitor_bmp(bmphandle_t bh, int x, int y, struct oo_fb_data *data)
{
	int i, j, k = 0;
	struct color pixel;
	for (i=0; i<bmp_height(bh); i++)
			for (j=0; j<bmp_width(bh); j++) {
				pixel = bmp_getpixel(bh, j, i);
				data4monitor(data+k, x+j, y+i, makepixel(pixel));
				k++;
			}
}

void clear_screen(void)
{
	memset(myfb->fb, 0, myfb->fbvar.xres*myfb->fbvar.yres*16/8);
}

