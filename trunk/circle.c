unsigned short * fb_mmap_temp;	// 더블 버퍼링 (myfb->fb의 임시 temp 변수)

void  dot( int x, int y, struct color *color)	// write color pixel
{
	unsigned short *ptr;

	if ( 0 <= x && 0 <= y && x < myfb->fbvar.xres && y < myfb->fbvar.yres )
	{
		ptr   = fb_mmap_temp + myfb->fbvar.xres * y + x;	// offset
		*ptr  = makeplixel(*color);	// 임시 mmap변수 fb_mmap_temp에 픽셀을 write
	}
}

static void ploat_circle( int x_center, int y_center, int x_coor, int y_coor, struct color *color)	// 빈원
{																											// 해당 좌표에 color pixel을 write
	dot( x_center +x_coor, y_center +y_coor, color);
	dot( x_center -x_coor, y_center +y_coor, color);
	dot( x_center +x_coor, y_center -y_coor, color);
	dot( x_center -x_coor, y_center -y_coor, color);
	dot( x_center +y_coor, y_center +x_coor, color);
	dot( x_center -y_coor, y_center +x_coor, color);
	dot( x_center +y_coor, y_center -x_coor, color);
	dot( x_center -y_coor, y_center -x_coor, color);
}

static void inner_circle(int x_center, int y_center, int x_coor, int y_coor, struct color *color)	// 꽉찬원
{
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

void  drow_ploat_circle( int x_center, int y_center, int radius, struct color color)	// 빈원 draw (x, y, radius, color)
{
	int      x_coor;
	int      y_coor;
	int      p_value;

	fb_mmap_temp = (unsigned short *)malloc(myfb->fbvar.xres*myfb->fbvar.yres*2);	// 더블버퍼링 임시변수에 screen size 할당
	memset(fb_mmap_temp, 0, myfb->fbvar.xres*myfb->fbvar.yres*2);	// 더블버퍼 클리어

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

	memcpy(myfb->fb, fb_mmap_temp, fbvar.xres*fbvar.yres*2);	// mmap에 memory copy (더블 버퍼링)
}

void  drow_inner_circle( int x_center, int y_center, int radius, struct color color)	// 꽉찬원 draw (x, y, radius, color)
{
	int      x_coor;
	int      y_coor;
	int      p_value;

	fb_mmap_temp = (unsigned short *)malloc(myfb->fbvar.xres*myfb->fbvar.yres*2);	// 더블버퍼링 임시변수에 screen size 할당
	memset(fb_mmap_temp, 0, myfb->fbvar.xres*myfb->fbvar.yres*2);	// 더블버퍼 클리어

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

	memcpy(myfb->fb, fb_mmap_temp, fbvar.xres*fbvar.yres*2);	// mmap에 memory copy (더블 버퍼링)
}
