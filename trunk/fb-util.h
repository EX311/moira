

#include <linux/fb.h> /* for fb_var_screeninfo, FBIOGET_VSCREENINFO */


//struct fb_var_screeninfo* fbvar;
//struct fb_fix_screeninfo* fbfix;
//struct color color;

#define FBDEVFILE "/dev/fb0"

#define VFB_MAX 4

struct color
{
	unsigned char r ;
	unsigned char g ;
	unsigned char b ;
};

struct myfb_info
{
	int fd;
	unsigned short *fb ;
	struct fb_var_screeninfo fbvar;
	struct fb_fix_screeninfo fbfix;
};

//char* vfb_list[VFB_MAX] ; 
unsigned short* vfb_list[VFB_MAX] ; 

extern unsigned short makepixel(struct color);
extern void drow_pixel(int x, int y, struct color);
extern void drow_line(int x1, int y1, int x2, int y2, struct color color);
extern void drow_rect (int x1, int y1, int x2, int y2, struct color color);

//extern char** set_vfb_buf(int n);
extern void set_vfb_buf(int n);
extern void free_vfb_buf(int n);
extern void show_vfb(unsigned short* vfb);
extern void buf_pixel(unsigned short* buf ,int x, int y, struct color color);

extern struct myfb_info* myfb_open (void);
extern void myfb_close(void);
// add functions for BMP
struct bmphandle_s;

typedef struct bmphandle_s *bmphandle_t;

bmphandle_t bmp_open(const char *filename);
void bmp_close(bmphandle_t bh);

int bmp_height(bmphandle_t bh);
int bmp_width(bmphandle_t bh);

struct color bmp_getpixel(bmphandle_t bh, int x, int y);
