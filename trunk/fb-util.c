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
//#include "fb-util.h"

struct myfb_info *myfb;
unsigned short *vfb_list[VFB_MAX];
unsigned short * fb_mmap_temp;  // 더블 버퍼링 (myfb->fb의 임시 temp 변수)

unsigned short makepixel(struct color color)
{
	unsigned char r = color.r;
	unsigned char g = color.g;
	unsigned char b = color.b;

	return (unsigned short)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
}

/*
 reveres_pixel func run only 16 bpp
  unsigned short pix_data -> struct color type
*/
struct color reveres_pixel(unsigned short pix)
{
	struct color color; 
	unsigned short r_mask = 0xf800 ;
	unsigned short g_mask = 0x07e0 ;
	unsigned short b_mask = 0x001f ;

	color.r = (r_mask & pix)>>8 ;
	color.g = (g_mask & pix)>>3 ;
	color.b = (b_mask & pix)<<3 ;

	return color;
}


void drow_pixel(int x, int y, struct color color)
{
	unsigned short pixel;
	int offset ;
	
	offset = y * myfb->fbvar.xres + x;

	pixel = makepixel(color);

	if(offset > -1 && offset <= myfb->fbfix.smem_len )
		*(myfb->fb+offset) = pixel ;
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

/* 원 그리기 함수(더블버퍼링 적용) */
void  dot( int x, int y, struct color *color) // write color pixel
{
	//unsigned short *ptr;

	if ( 0 <= x && 0 <= y && x < myfb->fbvar.xres*2 && y < myfb->fbvar.yres*2 )
	{
		//ptr   = fb_mmap_temp + myfb->fbvar.xres * y + x;  // offset
		//*ptr  = makepixel(*color); // 임시 mmap변수 fb_mmap_temp에 픽셀을 write
		buf_pixel(x, y, *color);
	}
}
 
static void ploat_circle( int x_center, int y_center, int x_coor, int y_coor, struct color *color)  // 빈원
{                                                     // 해당 좌표에 color pixel을 하나씩 write
	dot( x_center +x_coor, y_center +y_coor, color);
	dot( x_center -x_coor, y_center +y_coor, color);
	dot( x_center +x_coor, y_center -y_coor, color);
	dot( x_center -x_coor, y_center -y_coor, color);
	dot( x_center +y_coor, y_center +x_coor, color);
	dot( x_center -y_coor, y_center +x_coor, color);
	dot( x_center +y_coor, y_center -x_coor, color);
	dot( x_center -y_coor, y_center -x_coor, color);
}

static void inner_circle(int x_center, int y_center, int x_coor, int y_coor, struct color *color) // 꽉찬원
{																											// 해당 좌표에 x, y 축을 기준으로 color pixel을 write
	int x_dot;
	int y_dot;

	y_dot = y_center + y_coor;
	for(x_dot = x_center - x_coor; x_dot < x_center + x_coor; x_dot++)
		dot(x_dot, y_dot, color);
	
	y_dot = y_center - y_coor;
	for(x_dot = x_center - x_coor; x_dot < x_center + x_coor; x_dot++)
		dot(x_dot, y_dot, color);
	
	y_dot = y_center + x_coor;
	for(x_dot = x_center - y_coor; x_dot < x_center + y_coor; x_dot++)
		dot(x_dot, y_dot, color);
	
	y_dot = y_center - x_coor;
	for(x_dot = x_center - y_coor; x_dot < x_center + y_coor; x_dot++)
		dot(x_dot, y_dot, color);
}

void  drow_ploat_circle( int x_center, int y_center, int radius, struct color color)  // 빈원 draw (x, y, radius, color)
{
	int      x_coor;
	int      y_coor;
	int      p_value;

	//fb_mmap_temp = (unsigned short *)malloc(myfb->fbvar.xres*myfb->fbvar.yres*2); // 더블버퍼링 임시변수에 screen size 할당
  //memset(fb_mmap_temp, 0, myfb->fbvar.xres*myfb->fbvar.yres*2); // 더블버퍼 클리어

	x_coor   = 0;
	y_coor   = radius;
	p_value   = 3 - 2 * radius;
	while   ( x_coor < y_coor)
	{
		ploat_circle( x_center, y_center, x_coor, y_coor, &color);
		if ( p_value < 0)
		{
			p_value   += 4 * x_coor +6;
		}
		else
		{
			p_value   += 4 * ( x_coor -y_coor) +10;
			y_coor--;
		}
		x_coor++;
	}
	if ( x_coor == y_coor)
		ploat_circle( x_center, y_center, x_coor, y_coor, &color);
	
	//memcpy(myfb->fb, fb_mmap_temp, myfb->fbvar.xres*myfb->fbvar.yres*2);  // mmap에 memory copy (더블 버퍼링)
	//free(fb_mmap_temp);
}

void  drow_inner_circle( int x_center, int y_center, int radius, struct color color)  // 꽉찬원 draw (x, y, radius, color)
{
	int      x_coor;
	int      y_coor;
	int      p_value;

	//fb_mmap_temp = (unsigned short *)malloc(myfb->fbvar.xres*myfb->fbvar.yres*2); // 더블버퍼링 임시변수에 screen size 할당
  //memset(fb_mmap_temp, 0, myfb->fbvar.xres*myfb->fbvar.yres*2); // 더블버퍼 클리어

	x_coor   = 0;
	y_coor   = radius;
	p_value   = 3 - 2 * radius;
	while   ( x_coor < y_coor)
	{
		inner_circle( x_center, y_center, x_coor, y_coor, &color);
		if ( p_value < 0)
		{
			p_value   += 4 * x_coor +6;
		}
		else
		{
			p_value   += 4 * ( x_coor -y_coor) +10;
			y_coor--;
		}
		x_coor++;
	}
	if ( x_coor == y_coor)
		inner_circle( x_center, y_center, x_coor, y_coor, &color);
	
	//memcpy(myfb->fb, fb_mmap_temp, myfb->fbvar.xres*myfb->fbvar.yres*2);  // mmap에 memory copy (더블 버퍼링)
	//free(fb_mmap_temp);
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

void clear_vfb_buf(int n)
{
	int i;

	for (i=0; i<n; i++)
		memset(vfb_list[i], 0, myfb->fbfix.smem_len);
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
	else if ( 319 < x && x <640 && -1< y  && y<240)
	{
		location = 1;
		x-=320; 
	}	
	else if ( -1 < x && x <320 && 239< y  && y<480)
	{
		location = 2;
		y-=240;
	}	
	else if ( 319 < x && x <640 && 239< y  && y<480)
	{
		location = 3;
		x-=320;
		y-=240;		
	}

	if (x >= 640 || y >= 480)
		return;

	offset =y * myfb->fbvar.xres + x;
	pixel = makepixel(color);

	if(offset > -1 && offset <= myfb->fbfix.smem_len ) {
		*(vfb_list[location]+offset) = pixel ;
	}
}

void buf_test_rect(int x1, int y1, int x2, int y2, struct color color)
{
	int i, j, x, y;
	unsigned short pixel;
	int offset;
	int location;

	for(i = y1;i <= y2;i++)
	{
		for(j = x1;j <= x2;j++)
		{
			x = j; y = i;
			if ( -1  < x && x <320 && -1< y  && y<240)
			{
				location = 0;		
			}
			else if ( 319 < x && x <640 && -1< y  && y<240)
			{
				location = 1;
				x-=320; 
			}	
			else if ( -1 < x && x <320 && 239< y  && y<480)
			{
				location = 2;
				y-=240;
			}	
			else if ( 319 < x && x <640 && 239< y  && y<480)
			{
				location = 3;
				x-=320;
				y-=240;		
			}

			if (x >= 640 || y >= 480)
				return;

			offset =y * myfb->fbvar.xres + x;
			pixel = makepixel(color);

			if(offset > -1 && offset <= myfb->fbfix.smem_len ) {
				*(vfb_list[location]+offset) = pixel ;
			}
		}
	}
}

void buf_rect(int x1, int y1, int x2, int y2, struct color color)
{
	int i, j;

	for(i = y1;i <= y2;i++)
	{
		for(j = x1;j <= x2;j++)
			buf_pixel(j, i, color);
	}
}


void show_vfb(unsigned short* vfb)
{
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

void put_char(int x, int y, int c, struct color color)
{
	int i,j,bits;
	for (i = 0; i < font_vga_8x8.height; i++) {
		bits = font_vga_8x8.data [font_vga_8x8.height * c + i];
		for (j = 0; j < font_vga_8x8.width; j++, bits <<= 1)
			if (bits & 0x80)
				drow_pixel(x+j, y+i, color);
	}
}

void put_string(int x, int y, char *s, struct color color)
{
	int i;
	for (i = 0; *s; i++, x += font_vga_8x8.width, s++)
		put_char (x, y, *s, color);
}

void put_string_center(int x, int y, char *s, struct color color)
{
	size_t sl = strlen (s);
	put_string (x - (sl / 2) * font_vga_8x8.width, y - font_vga_8x8.height / 2, s, color);
}

void myfb_close(void)
{
//	clear_screen();
	munmap(myfb->fb, myfb->fbvar.xres*myfb->fbvar.yres*16/8);
	close(myfb->fd);
	free(myfb);
}

void buf_bmp(bmphandle_t bh, int x, int y)	// buffer use inner rect
{
	int i, j;
	struct color pixel;
	for (i = 0; i < bmp_height(bh); i++)
		for (j = 0; j < bmp_width(bh); j++) {
			pixel = bmp_getpixel(bh, j, i);
			buf_pixel(j+x,i+y,pixel);
		}
}
/*
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
*/
void clear_screen(void)
{
	memset(myfb->fb, 0, myfb->fbvar.xres*myfb->fbvar.yres*16/8);
}

