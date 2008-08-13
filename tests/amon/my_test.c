/*
 * test app  make by amon
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//#include "fb-util.h"
#include "oo.h"

#define BUFSIZE 4096

#define DEBUG	1 


#if DEBUG

#endif

bmphandle_t bh;

struct myfb_info* myfb ;

int server_sock_listen, server_sock_accept, client_sock;

char* cp_port= "8000";
int port ;

char* ip_list[4] ;


void do_server(void)
{
	/*
	int Mylocation = get_Mylocation();
	char* myip = get_IpInfo(MyLocation); 
	*/

	int count =0 ;
	int ret;
	int offset = 0;
	
	unsigned short* buf;
	buf = (unsigned short*) malloc(myfb->fbfix.smem_len);
	
	//port = ip2port(ip);  
	port = 8000;
	server_sock_listen = tcp_server_listen(port, 5);  //arg port, listen num
	server_sock_accept = tcp_server_accept(server_sock_listen);

	
	#if DEBUG
	printf(" server[slave] ip : %s , port is %d \n", ip_list[0], port);
	#endif
	
	//recv data
	while(ret = read(server_sock_accept, buf+offset, BUFSIZE))
	{	
		count += ret;
		offset += (ret/2);


		#if DEBUG
		printf(" recv data size is %d \n",ret);
		#endif
		
		if(count == myfb->fbfix.smem_len)
		{
			show_vfb(buf);
			count = 0 ;
		}
	}


}

/*
void do_client(void)
{

	client_sock = tcp_client_connect("192.168.1.50", "8000") ;
	printf("test cli_sock is %d \n",client_sock);
	
}

*/

int send_data(unsigned short* vfb)
{
	int ret;
	int slave_loc = 1;
	
	ret = write(client_sock,vfb_list[slave_loc] , myfb->fbfix.smem_len);
	return ret;
}
	

int main(int argc , char**argv)
{


	struct color color;

	int i, j;
	//char* fb_list[ VFB_MAX];

	int x,y;

	int master;

	ip_list[0] = "192.168.77.30";
	ip_list[1] = "192.168.77.55";

	int debug_send_data_size =0;
#if DEBUG
	int test_count =0 ;
#endif


	master = atoi(argv[1]);
	myfb = myfb_open();
	

	// is master ? yse -> client , no ->server
	
	//if (master == mylocation)
	if (master == 1)
	{
			
		if(argc !=5)
		{
			perror("useing filename (int)arg [master is 1, slaver 2");
			exit(1);
		}	
	
		set_vfb_buf(4);
		bh = bmp_open(argv[2]);
		buf_bmp(bh,atoi(argv[3]),atoi(argv[4]));

		

		client_sock = tcp_client_connect( ip_list[1], "8000") ;
/*		
	// show my location data [pixel]
		x= 150;
		y= 30;
	
	//	for ( x = 150; x < 200; x++)
		{
			printf(" test for loop 1 x= %d\n",x);
			for( i =0 ; i<50; i++)
			{

				for(j=0; j <400 ; j++)
					buf_pixel(vfb_list[0],j+x,i+y,color);
			}

			//show_vfb(vfb_list[mylocation]);
*/
			show_vfb(vfb_list[0]);  // my fb data show
			debug_send_data_size = send_data(vfb_list[1]);
			#if DEBUG
				printf("send_data() call %d , send data size is %d\n",test_count,debug_send_data_size);
				test_count++;
			#endif

	}

	

	else
	{
			do_server();
	}
	
	
	
	
/*
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
*/
	return 0; 
}
