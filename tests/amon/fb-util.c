/*
 * fb info 
 */




#include <stdio.h>
#include <stdlib.h> /* for exit */
#include <unistd.h> /* for open/close .. */
#include <fcntl.h> /* for O_RDWR */
#include <sys/ioctl.h> /* for ioctl */
#include <linux/fb.h> /* for fb_var_screeninfo, FBIOGET_VSCREENINFO */
#include <sys/mman.h> /* for mmap */

#include "fb-util.h"



#define FBDEVFILE "/dev/fb0"

#define DEBUG 0

/*
 *
#if DEBUG
	printf(" message \n");
#endif

*/

struct myfb_info *myfb;
struct color color;

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
	
	offset =y * myfb->fbvar.xres + x;

	pixel = makepixel(color);
	
	
#if DEBUG
	printf(" pixel is 0x%x  \n", pixel);
	printf(" myfb->fb  x is : %d y is %d offset : %d val is 0x%x \n",x,y, offset ,*myfb->fb);
	
#endif

	*(myfb->fb+offset) = pixel ;
	
}
/*
void drow_line(int x1, int y1, int x2, int y2, struct color color)
{
	int dx, dy;

	dx = x2 -x1 ;
	dy = y2 -y1 ;


}
*/
struct myfb_info* myfb_open (void)
{

	int ret ;
	int fbfd ;

	myfb =(struct myfb_info*) malloc(sizeof( struct myfb_info));

	fbfd = open(FBDEVFILE, O_RDWR);
	
	if(fbfd < 0)
	{
		perror("fbdev open");
		exit(1);
	}
	
	
	//ret=ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);
	ret=ioctl(fbfd, FBIOGET_VSCREENINFO, &myfb->fbvar);
	
	if(ret < 0)
	{
		perror("fbdev ioctl(FSCREENINFO)");
		exit(1);
	}


	//ret = ioctl(fbfd, FBIOGET_FSCREENINFO, &fbfix);
	ret=ioctl(fbfd, FBIOGET_VSCREENINFO, &myfb->fbfix);
	
	if(ret < 0)
	{
		perror("fbdev ioctl(FSCREENINFO)");
		exit(1);
	}
	


	myfb->fb=(unsigned short *)mmap(0, myfb->fbvar.xres*myfb->fbvar.yres*16/8,PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

	memset (myfb->fb,0, myfb->fbvar.xres*myfb->fbvar.yres*16/8);

	if (myfb->fb == 0)
	{
		perror("mmap err\n");
		exit(1);
	}
	
	return myfb;
}

















/*
int main(void)
{

	struct fb_var_screeninfo* fbvar;
	struct fb_fix_screeninfo* fbfix;
	
	get_fbinfo(fbvar,fbfix);

	printf("x-resolution : %d\n", fbvar->xres);
	printf("y-resolution : %d\n", fbvar->yres);
	printf("x-resolution(virtual) : %d\n", fbvar->xres_virtual);
	printf("y-resolution(virtual) : %d\n", fbvar->yres_virtual);
	printf("bpp : %d\n", fbvar->bits_per_pixel);

	exit(0);

}
*/
