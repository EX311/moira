/*
 *
 * change-offset.c
 *
 *  change-offset( x,y)  --> return  (위치, x,y) 정보 리턴
 *
 */

int v-xres;// 연결 된후 가상 프레임 버퍼의 가로 길이 
int v-yres;// 연결 된후 가장 프레임퍼버의 세로 길이
int fix-xres; // LCD의 고정된 해상도
int fix-yres;

struct pixsel_info 
{
	int id;
	int x;
	int y;
};

int change-offset( int x , int y)
{
	struct pixel_info pix ;

	if ( x >=  0 && x < 320 && y >=0 && y<240 )
	{
		pix.id = 0;
		pix.x = x ;
		pix.y = y ;
			
	}


	if ( x >= 320 && x< 640 && y >=0 && y<240 )
	{
		pix.id = 1;
		pix.x = x-320;
		pix.y = y ;


	}


	if ( x >= 0 && x< 320 && y >=240 && y<480 )
	{
		pix.id = 2
	}

	if ( x >= 320 && x< 640 && y >=240 && y<480 )
	{
		pix.id = 3
	}
}
