

#include <linux/fb.h> /* for fb_var_screeninfo, FBIOGET_VSCREENINFO */


//struct fb_var_screeninfo* fbvar;
//struct fb_fix_screeninfo* fbfix;
//struct color color;



struct color
{
	unsigned char r ;
	unsigned char g ;
	unsigned char b ;
};

struct myfb_info
{
	unsigned short *fb ;
	struct fb_var_screeninfo fbvar;
	struct fb_fix_screeninfo fbfix;
};

extern void drow_pixel(int x, int y, struct color);
extern struct myfb_info* myfb_open (void);
