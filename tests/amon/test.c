#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "fb-util.h"

int main(void)
{

	struct myfb_info* myfb ;
	struct color color;

	int i, j;
	//char* fb_list[ VFB_MAX];

	int x,y;

	myfb = myfb_open();

	color.r = 0 ;
	color.g = 0;
	color.b = 255;
/*
	for ( i = 0 ; i <100 ;i++)
	{
		for( j=0; j <100 ; j++)
			drow_pixel(j,i,color);
	}
*/
	drow_rect(10,10,150,60,color);
	drow_line(150,150, 300,200,color);

	color.r = 255 ;
	color.g = 0;
	color.b = 0 ;

	//fb_list = set_vfb_buf(2);
	set_vfb_buf(2);

	x= 10;
	y= 30;
	
	for (; x<200; x++)
	{

		for( i =0 ; i<50; i++)
		{
			for(j=0; j <200 ; j++)
				buf_pixel(vfb_list[0],j+x,i+y,color);
		}

		show_vfb(vfb_list[0]);
	}


	
	printf("x-resolution : %d\n", myfb->fbvar.xres);
	printf("y-resolution : %d\n", myfb->fbvar.yres);
	printf("x-resolution(virtual) : %d\n", myfb->fbvar.xres_virtual);
	printf("y-resolution(virtual) : %d\n", myfb->fbvar.yres_virtual);
	printf("bpp : %d\n", myfb->fbvar.bits_per_pixel);
	printf("smem_len: %d\n", myfb->fbfix.smem_len);

	return 0; 
}
