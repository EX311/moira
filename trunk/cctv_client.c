/*
 *  cctv_client app
 *  code by amon
 *
 *  using lib tslib, pthread  ->  -lts -lpthread
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <pthread.h>
#include <signal.h>

#include "tslib.h"

#include "oo.h"


#define FB_NAME    "/dev/fb0" 
#define TTY_NAME   "/dev/ttyS0" 

#define DEBUG 0



extern struct myfb_info *myfb;
unsigned int cam_id ;
unsigned int event = 0; 
struct tsdev *ts;
char *tsdevice = "/dev/ts0";
struct ts_sample samp;

struct myfb_info* myfb ;
//char *taget_ip ;
char *ipaddr[VFB_MAX] = {"192.168.77.20","192.168.77.30","192.168.77.77","192.168.88.55"};

//static FILE * tty_fp;

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

/*
 *  ts_click func is thread fanc 
 *  
 *  read input touch screen 
 *
 */
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
				put_string_center(140, 100, " exit cctv app ", white);
				event = 9 ;
				sleep(1);
				pthread_exit(NULL);
				}

				else if( (t = myicon_handle(&icon[i], &samp)))
				{
				cam_id = i ;
				event = 1;
#if DEBUG
				printf(" cam id is %d event is %d \n",cam_id, event);
#endif
				}
			
			
			}
		}
	}
	pthread_exit(NULL);
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

int main(void)
{

	pthread_t ts_thread;
	void* thread_result;
	int thread_res;

	int count = 0 ;

	int i = 0;
	int ret;
	int sock;

	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);

	myfb = myfb_open();
	set_vfb_buf(VFB_MAX);

	register_icon(&icon[0], 40, 198,60,40, "cam 1", "" );
	register_icon(&icon[1], 130, 198,60,40, "cam 2", "");
	register_icon(&icon[2], 220, 198,60,40, "cam 3", "");
	register_icon(&icon[3], 298, 5,20,20, "x", "");
	
	thread_res = pthread_create(&ts_thread, NULL, ts_click,NULL);

	if (thread_res !=0)
	{
		perror (" thread create failed\n");
		exit(1);
	}


	for (i=0; i<myicon_count; i++)
		draw_icon(&icon[i]);
	
	while( event ==0)
	{
		sleep(1);
	//	printf("inout ready\n");	
	}

	while(1)
	{

//		printf(" main loop \n");

		if (event == 9)
		{
			sleep(1);
			clear_screen();
			exit(0);
		}

		sock = tcp_client_connect(ipaddr[cam_id], ip2port(ipaddr[cam_id],8002));

		if (sock < 0)
		{
			fprintf(stderr, "%s connect error\n", ipaddr[cam_id]);
			cam_id = (cam_id+1)%4;
			continue;
		}


		while(1)
		{
			ret = read(sock,(unsigned char*)vfb_list[cam_id] + count , myfb->fbfix.smem_len);
			//printf(" cam data read   event = %d \n",event);
			count += ret;
			
#if DEBUG
			printf(" 	debug :: ret is %d \n",count);
#endif
			if( count == myfb->fbfix.smem_len)
			{
#if DEBUG
				printf("		 debug: call vfb_show \n ");
#endif
				show_vfb(vfb_list[cam_id]);

				for (i=0; i<myicon_count; i++)
					draw_icon(&icon[i]);
				count = 0;


				if (event == 9)
				{
				put_string_center(140, 100, " exit cctv app ", white);
				sleep(2);
				clear_screen();
				exit(0);
				}

				else if( event != 0 )
				{	
					break;
				}
			}

		}

		close(sock);
	}

	thread_res= pthread_join(ts_thread, &thread_result);
	myfb_close();
	free_vfb_buf(VFB_MAX);
	return 0;

}

