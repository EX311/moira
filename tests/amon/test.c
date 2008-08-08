#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "fb-util.h"

int main(void)
{

	struct myfb_info* myfb ;
	struct color color;

	int i, j;


	myfb = myfb_open();

	color.r = 0 ;
	color.g = 0;
	color.b = 255;

	for ( i = 0 ; i <100 ;i++)
	{
		for( j=0; j <100 ; j++)
			drow_pixel(j,i,color);
	}

	drow_line(150,150, 300,200,color);
	
	printf("x-resolution : %d\n", myfb->fbvar.xres);
	printf("y-resolution : %d\n", myfb->fbvar.yres);
	printf("x-resolution(virtual) : %d\n", myfb->fbvar.xres_virtual);
	printf("y-resolution(virtual) : %d\n", myfb->fbvar.yres_virtual);
	printf("bpp : %d\n", myfb->fbvar.bits_per_pixel);
	return 0; 
}
	
	
	/*
	struct fb_var_screeninfo* fbvar;
	struct fb_fix_screeninfo* fbfix;
	
	get_fbinfo(fbvar,fbfix);

	printf("x-resolution : %d\n", fbvar->xres);
	printf("y-resolution : %d\n", fbvar->yres);
	printf("x-resolution(virtual) : %d\n", fbvar->xres_virtual);
	printf("y-resolution(virtual) : %d\n", fbvar->yres_virtual);
	printf("bpp : %d\n", fbvar->bits_per_pixel);

	exit(0);
*/


