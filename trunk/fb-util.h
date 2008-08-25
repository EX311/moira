/*
 * fb-util.h
 *
 *
 */


#include <linux/fb.h> /* for fb_var_screeninfo, FBIOGET_VSCREENINFO */
#include "font.h"

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

extern unsigned short* vfb_list[VFB_MAX] ;// VFB data point's list 

extern unsigned short makepixel(struct color);


extern struct color reveres_pixel(unsigned short pix);
/*
 * daow 접두어 함수 : 프레임 버퍼 영역에 직접 pixel 단위로 표현하는 함수
 */

extern void drow_pixel(int x, int y, struct color);
extern void drow_line(int x1, int y1, int x2, int y2, struct color color);
extern void drow_rect (int x1, int y1, int x2, int y2, struct color color); //(x1,y1),(x2,y2) 두 점으로 하는 사각형
extern void  drow_ploat_circle( int x_center, int y_center, int radius, struct color color);  // ��� draw (x, y, radius, color)
extern void  drow_inner_circle( int x_center, int y_center, int radius, struct color color);  // ������ draw (x, y, radius, color)

//extern char** set_vfb_buf(int n);

extern void set_vfb_buf(int n); // n개의 vfb와 같은 크기의 메모리 공간을 동적 할당 각 메모리 포인터는  vfb_list[n]에 저장
extern void free_vfb_buf(int n);
extern void show_vfb(unsigned short* vfb); // vfb의 data를 FB에 출력
extern void buf_pixel(int x, int y, struct color color); // color의 생상을 가진 점을 각 사분면의 맞게 set_vfb_buf() 에서 받은 공간에 저장

extern void buf_show_rect(int x1, int y1, int x2, int y2, struct color color);
extern void buf_rect(int x1, int y1, int x2, int y2, struct color color);

extern struct myfb_info* myfb_open (void);
extern void myfb_close(void);

void put_char(int, int, int, struct color);
void put_string(int, int, char *, struct color);
void put_string_center(int, int, char *, struct color);

// add functions for BMP
struct bmphandle_s;

typedef struct bmphandle_s *bmphandle_t;

bmphandle_t bmp_open(const char *filename);
void bmp_close(bmphandle_t bh);

int bmp_height(bmphandle_t bh);
int bmp_width(bmphandle_t bh);

struct color bmp_getpixel(bmphandle_t bh, int x, int y);

