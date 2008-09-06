/*
 *  Using VF cam app
 * compile :arm-linux-gcc fb-util.c oo_network.c bmplib.c font_8x8.c cam_view.c -o OUT_FILE_NAME -lpthread -lts
 * run : /.OUT_FILE_NAME
 * code my amon
 *
 * func 1 is  320*240 image show my device
 * func 2 is  320*240 image show my device and send same image to other device
 * func 3 is  640*480 image show splited image  320*240 image * 4
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "oo.h"

#include "/home/amon/work/source/rebis-2.6/drivers/media/video/userapp.h" // linux source tree for rebis

#define CODEC_NAME  "/dev/preview"


#define DEBUG 0

#define FB_NAME    "/dev/fb0" 
#define TTY_NAME   "/dev/ttyS0" 

static camif_param_t camif_cfg;

struct myfb_info* myfb ;

//                      { master ip[ screen 1], [screen 2] ,     [screen 3]   ,    [screen 4]    }
//char *ipaddr[VFB_MAX] = {"192.168.123.167", "192.168.123.182", "192.168.123.172", "192.168.123.157"};
char *ipaddr[VFB_MAX] = {"192.168.123.172", "192.168.123.167", "192.168.123.182", "192.168.123.157"};

int sock[VFB_MAX];
int event = 0; 
int func = 0; // 1 is only master , 2 is master image send conneted slaves 3 is full screen mode 640*480
unsigned char* cam_data;
int cam_fd = 0 ;


int image_size;

struct tsdev *ts;
char *tsdevice = "/dev/ts0";
struct ts_sample samp;



struct icon {
	int x, y, w, h;
	char *name;
	char *action;
};

#define  myicon_count  4  
static struct icon icon[myicon_count]; 

struct color white = {0xff, 0xff, 0xff};


static void sig(int sig)
{
	fflush(stderr);
	printf("signal %d caught\n", sig);
	fflush(stdout);
	exit(1);
}

void register_icon(struct icon *icon, int x, int y, int w , int h, char *name, char *action)
{
	char *t_temp = (char *)malloc(strlen(name)+1);
	char *t_act = (char *)malloc(strlen(action)+1);
	memcpy(t_temp, name, strlen(name));
	t_temp[strlen(name)] = '\0';
	memcpy(t_act, action, strlen(action));
	t_act[strlen(action)] = '\0';

	icon->x = x;
	icon->y = y;
	icon->w = w;
	icon->h = h;
	icon->name = t_temp;
	icon->action = t_act;
}


void draw_icon(struct icon *icon)
{
	drow_rect(icon->x, icon->y, icon->x+icon->w, icon->y+icon->h, white);
	put_string_center(icon->x+icon->w/2, icon->y+icon->h/2, icon->name, white);
}

int myicon_handle(struct icon *icon, struct ts_sample *samp)
{
	int inside = (samp->x >= icon->x) && (samp->y >= icon->y) && (samp->x < icon->x + icon->w) && (samp->y < icon->y + icon->h);
	if (samp->pressure > 0) {
		if (inside)
			return inside;
		else
			return 0;
	}
	return 0;
}



void *  ts_click(void* arg)
{
	int i , ret ,t;
	int chat = 0 ;

	ts = ts_open(tsdevice, 0);
	if (!ts) 
	{
		perror("ts_open");
		exit(1);
	}


	if (ts_config(ts)) 
	{
		perror("ts_config");
		exit(1);
	}						        
#if DEBUG
	printf(" thread func start \n");
#endif
	while(1)
	{
		ret = ts_read(ts, &samp, 1);
		chat ++;

		if (ret < 0) {
			perror("ts_read");
			exit(1);
		}

		if (chat < 15)
			continue;
		else
		{
			chat = 0;
			for (i=0; i < myicon_count; i++)
			{
				if ( (t = myicon_handle(&icon[myicon_count -1], &samp)))
				{
					put_string_center(140, 100, " exit cam app ", white);
					event = 9 ;
					sleep(1);
					pthread_exit(NULL);
				}

				else if( (t = myicon_handle(&icon[i], &samp)))
				{
					func = i+1;	
					event =1;
#if DEBUG
					printf(" event is %d \n", event);
#endif
				}


			}
		}
	}
	pthread_exit(NULL);
}









static inline int cam_init(void)
{
	int dev_fp = -1;
	dev_fp = open(CODEC_NAME, O_RDWR);

	if (dev_fp < 0) {
		perror(CODEC_NAME);
		return -1;
	}


	return dev_fp;
}


void cam_set(int func)
{
	if(cam_data)
		free(cam_data);

	if (func == 1 || func == 2)
	{
#if DEBUG
		printf(" func is 1 or 2  320*240\n");
#endif
		camif_cfg.dst_x = 320 ;
		camif_cfg.dst_y = 240;

		camif_cfg.bpp = 16;
		camif_cfg.flip = 0 ;

		image_size = 320*240*2;

		cam_data = (unsigned char*)malloc(image_size);
	}

	else if(func == 3)
	{
#if DEBIG
		printf(" mode is 2 640*480\n");
#endif
		camif_cfg.dst_x = 640 ;
		camif_cfg.dst_y = 480;

		camif_cfg.bpp = 16;
		camif_cfg.flip = 0 ;

		image_size = 640*480*2 ;	

		cam_data = (unsigned char*)malloc(640*480*2);
	}

	if(cam_data <0)
	{
		perror(" cam_data malloc err\n");
		exit(1);
	}


	if ( func > 0 && func <4)
	{

		cam_fd = cam_init();
#if DEBUG
		printf(" cam init() cam_fd is %d func is %d\n", cam_fd, func);
#endif
		if (ioctl(cam_fd, CMD_CAMERA_INIT, &camif_cfg)) {
			perror("ioctl");
			exit(1);	
		}

		write(cam_fd,"O",2); // cam start code 
	}
}




void send_data(func)
{
	int i ;
	for (i=1; i<VFB_MAX; i++) 
	{
		/*   change ->  start i = 0
		 *   if ( get_MasterLocation() == i )
		 *    continue;
		 *
		 */


		if(func ==2)
			fb_send(sock[i], vfb_list[0], myfb->fbfix.smem_len);
		else if (func ==3)
		{
			fb_send(sock[i], vfb_list[i], myfb->fbfix.smem_len);
		}
	}
}

void clear_all_screen(void)
{
	int i;
	memset( vfb_list[0], 0, myfb->fbfix.smem_len);

	for (i=1; i<VFB_MAX; i++) 
	{
		fb_send(sock[i], vfb_list[0], myfb->fbfix.smem_len);
	}


}


int main(void)
{

	pthread_t ts_thread;
	void* thread_result;
	int thread_res;

	int count ;
	int offset =0;
	pid_t pid;

	unsigned short buf_pix ;
	struct color tmp_color;

	int i,j, ret;


	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);

	myfb = myfb_open();

	set_vfb_buf(VFB_MAX);
#if DEBUG
	printf(" set_ vfb buf () \n");
#endif

	register_icon(&icon[0], 40, 198,60,40, "Single", "" );
	register_icon(&icon[1], 130, 198,60,40, "Mult", "");
	register_icon(&icon[2], 220, 198,60,40, "Full Screen", "");
	register_icon(&icon[3], 298, 5,20,20, "x", "");

	thread_res = pthread_create(&ts_thread, NULL, ts_click,NULL);

	if (thread_res !=0)
	{
		perror (" thread create failed\n");
		exit(1);
	}

	clear_screen();  

	for (i=0; i<myicon_count; i++)
		draw_icon(&icon[i]);

	while( event ==0)
	{
		sleep(1);  // input delay
	}

	while(1)
	{

		if (event == 9)
		{
			put_string_center(140, 100, " exit cam app ", white);
			sleep(2);
			goto end;

		}
		cam_set(func);

		if(func == 2 || func == 3)
		{
			for(i =0 ; i <VFB_MAX ; i++)
			{
				sock[i] = tcp_client_connect(ipaddr[i], ip2port(ipaddr[i],8000));
				if (sock[i] < 0)
				{
					fprintf(stderr, "%s connect error\n", ipaddr[i]);
				}
			}
		}
		event = 0;
		while(1)
		{
#if DEBUG
			printf(" main loop start func is %d \n",func);	
#endif
			count = read(cam_fd, (unsigned char*)cam_data, image_size);
			if( count <0)
			{
				perror(" cam_data read err \n");
				exit(1);
			}

			offset = 0;
			for (i =0 ; i <camif_cfg.dst_y ; i++)
			{
				for(j = 0 ; j < camif_cfg.dst_x; j++)
				{
					memcpy(&buf_pix,cam_data+offset, sizeof(unsigned short));
					offset+= 2;
					tmp_color = reveres_pixel(buf_pix);
					buf_pixel(j,i,tmp_color);
				}
			}
			if(func == 1)
			{
				show_vfb(vfb_list[0]);//  demo ver need change 0 to  mylocation	
			}
			else if (func != 1 &&  sock[1] > 0 )
			{
				send_data(func);
				show_vfb(vfb_list[0]);//  demo ver need change 0 to  mylocation	
			}
			for (i=0; i<myicon_count; i++)
				draw_icon(&icon[i]);

			if (event == 9)
			{
				put_string_center(140, 100, " exit cam app ", white);
				sleep(2);
				goto end;

			}
			else if( event != 0 )
			{	
				break;
			}

		}// end while "main lool"


		write(cam_fd,"X",2);	/* Stop Camera */
		if (cam_fd) close(cam_fd);
	
		
		for(i =0 ; i< VFB_MAX; i++)
		{
			if(sock[i])
				close(sock[i]);
		}
		
		
		//clear_all_screen();
	}


end:

	clear_screen();
	//	clear_all_screen();

	thread_res= pthread_join(ts_thread, &thread_result);
		for(i =0 ; i< VFB_MAX; i++)
		{
			if(sock[i])
			close(sock[i]);
		}

	if (cam_fd > 0) 
	{
		write(cam_fd,"X",2);
		close(cam_fd);
	}

	if (cam_data >0)
		free(cam_data);

	myfb_close();
	free_vfb_buf(VFB_MAX);
	return 0;

}

