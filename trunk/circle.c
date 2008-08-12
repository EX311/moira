void  dot( int x, int y)
{
	unsigned short *ptr;

	if ( 0 <= x && 0 <= y && x < screen_width && y < screen_height)
	{
		ptr   = fb_mapped + screen_width * y + x;
		*ptr  = 0xffff;
	}
}

static void ploat_circle( int x_center, int y_center, int x_coor, int y_coor)
{
	dot( x_center +x_coor, y_center +y_coor);
	dot( x_center -x_coor, y_center +y_coor);
	dot( x_center +x_coor, y_center -y_coor);
	dot( x_center -x_coor, y_center -y_coor);
	dot( x_center +y_coor, y_center +x_coor);
	dot( x_center -y_coor, y_center +x_coor);
	dot( x_center +y_coor, y_center -x_coor);
	dot( x_center -y_coor, y_center -x_coor);
}

void  circle( int x_center, int y_center, int radius)	// x, y, radius
{
	int      x_coor;
	int      y_coor;
	int      p_value;

	x_coor   = 0;
	y_coor   = radius;
	p_value   = 3 - 2 * radius;
	while   ( x_coor < y_coor)
	{
		ploat_circle( x_center, y_center, x_coor, y_coor);
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
		ploat_circle( x_center, y_center, x_coor, y_coor);
}
