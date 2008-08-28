#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "tslib.h"
#include "fbutils.h"
//#include "oo.h"

//#define SERVERADDR "192.168.77.55"

#define DEBUG 1

static int palette [] =
{
	0x000000, 0xffe080, 0xffffff, 0xe0c0a0, 0x304050, 0x80b8c0
};
#define NR_COLORS (sizeof (palette) / sizeof (palette [0]))

struct ts_button{
	int x,y,w,h;
	char *text;
	int flags;
#define BUTTON_ACTIVE 0x00000001
};

static int button_palette[6]=
{
	1,4,2,
	1,5,0
};

#define NR_BUTTONS 4
static struct ts_button buttons[NR_BUTTONS];

static void sig(int sig)
{
	close_framebuffer();
	fflush(stderr);
	printf("signal %d caught\n", sig);
	fflush(stdout);
	exit(1);
}

static void button_draw (struct ts_button *button)
{
	//if button is active s==3 else s==0.
	int s = (button->flags & BUTTON_ACTIVE) ? 3 : 0;
	//draw rectangle's outline with palette[3] color when it's active or palette[0] when it's not.
	rect (button->x, button->y, button->x + button->w - 1,
			button->y + button->h - 1, button_palette [s]);
	//fill in the rectagle with palette[4] or [1].
	fillrect (button->x + 1, button->y + 1,
			button->x + button->w - 2,
			button->y + button->h - 2, button_palette [s + 1]);
	//and draw fonts inside the button with palette[5] or [2].
	if(strcmp(button->text,"X")==0)
	{//when it's exit button.
		//let's draw decent exit button.
		//put_string(button->x + 2,button->y + 2,button->text, button_palette[s + 2]);
		line(button->x, button->y, button->x+button->w-1, button->y+button->h-1,button_palette[s+2]);
		line(button->x, button->y+button->h -1, button->x+button->w-1, button->y,button_palette[s+2]);
	}
	else{		
		put_string_center (button->x + button->w / 2,
				button->y + button->h / 2,
				button->text, button_palette [s + 2]);
	}
}



static int button_handle (struct ts_button *button, struct ts_sample *samp)
{
	//if inside ==1 that means user clicked area inside of button.
	int inside = (samp->x >= button->x) && (samp->y >= button->y) &&
		(samp->x < button->x + button->w) &&
		(samp->y < button->y + button->h);

	if (samp->pressure > 0) {
		if (inside) {
			if (!(button->flags & BUTTON_ACTIVE)) {
				//if button flag BUTTON_ACTIVE is not set
				button->flags |= BUTTON_ACTIVE;//set the BUTTON_ACTIVE flag bit on.
				button_draw (button);//draw button again.
			}
		} else if (button->flags & BUTTON_ACTIVE) {
			//if button flag BUTTON_ACTIVE is on
			button->flags &= ~BUTTON_ACTIVE;//set it inactive.
			button_draw (button);//draw button again.
		}
	} else if (button->flags & BUTTON_ACTIVE) {
		//if pressure is 0 and BUTTON is Active.
		button->flags &= ~BUTTON_ACTIVE;//inactivate the flag bit.
		button_draw (button);//draw button.
		return 1;
	}

	return 0;
}

int ip2port(char *ip, int startp)
{
	return ( (inet_addr(ip)) >> 24) + startp;
}

int tcp_client_connect(char *server_addr, int port)
{
	int ret;
	int sock;
	struct sockaddr_in serv_addr;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1){
		perror("socket()");
		exit(1);
	}

	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(server_addr);
	serv_addr.sin_port = htons(port);

	ret = connect(sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
	if (ret == -1) {
		close(sock);
		perror("connect()");
		exit(1);
	}

	return sock;
}


static void refresh_screen()
{
	int i;

#ifdef DEBUG 
	printf("before refresh_screen!!\n");
#endif
	fillrect (0, 0, xres - 1, yres - 1, 0);
#ifdef DEBUG
	printf("after refresh_screen!!\n");
#endif

	for (i = 0; i < NR_BUTTONS; i++)
		button_draw (&buttons [i]);
}



int main()
{
	struct tsdev *ts;
	int x,y,count=0;
	int read_flag = 0;
	unsigned int i;
	unsigned int merge;
	unsigned int mode = 0;
	unsigned int numofdevice = 0;
	unsigned short *buff,*old;
	int serv_sock;
	char serv_addr[] = "192.168.77.77";

	char *tsdevice="/dev/ts0";


	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);

	ts = ts_open(tsdevice,0);

	if(!ts)
	{
		perror(tsdevice);
		exit(1);
	}

	if(ts_config(ts))
	{
		perror("ts_config");
		exit(1);
	}
	if(open_framebuffer()){
		close_framebuffer();
		exit(1);
	}

	for(i=0;i<NR_COLORS;i++)
		setcolor(i,palette[i]);

	memset(&buttons,0,sizeof(buttons));
	buttons[0].w = buttons[1].w = buttons[2].w = 70;
	buttons[0].h = buttons[1].h = buttons[2].h = 20;
	buttons[0].x = 40; buttons[0].y = 200;
	buttons[1].x = 125; buttons[1].y = 200;
	buttons[2].x = 210; buttons[2].y = 200;
	buttons[0].text = "Clear";
	buttons[1].text = "Merge";
	buttons[2].text = "Split";
	buttons[3].w = 20;
	buttons[3].h = 20;
	buttons[3].x = 280; buttons[3].y = 20;
	buttons[3].text = "X";

	refresh_screen();
	numofdevice = 1;
	//need to connect to server
	//what we need here is function that perform setting network connection
	//between adjacent devices.

	serv_sock = tcp_client_connect(serv_addr,ip2port(serv_addr,7000));
	buff = (unsigned short *)malloc(fix.smem_len);
	old = (unsigned short *)malloc(fix.smem_len);
	memset(buff,0,fix.smem_len);
	memset(old,0,fix.smem_len);

	while(1)
	{
		struct ts_sample samp;
		int ret;

		ret = ts_read(ts, &samp,1);
		//read touchscreen input into samp
		if(ret<0)
		{
			perror("ts_read");
			close_framebuffer();
			exit(1);
		}
		if(ret!=1)//if there isn't any input. goto the top of the loop.
			continue;
		for(i=0; i<NR_BUTTONS; i++)//check if buttons clicked.
			if(button_handle(&buttons[i],&samp))
				switch(i){
					case 0://clear button clicked
						mode = 0;
						refresh_screen();
						break;
					case 1://merge button clicked
						mode = 1;
						//refresh_screen();
						/* we need a code for Merge here */
						/* request other devices to send their fb data */
						ret = write(serv_sock,buttons[mode].text,strlen(buttons[mode].text));
						if(ret<=0)
						{
							perror("write");
							exit(1);
						}
#ifdef DEBUG
						else
						{
							printf("send : %d\n",ret);
						}
#endif
						//now we need to read datas from others.
						count=0;
						read_flag=0;
						while(!read_flag){
							ret = read(serv_sock,buff,fix.smem_len);
							if(ret<=0)
							{
								perror("read");
								exit(1);
							}
							count+=ret;
#ifdef DEBUG
							printf(" read : %d\n",count);
#endif
							if(count==fix.smem_len)
							{
								read_flag=1;
							}
							
						}
#ifdef DEBUG
						printf("read finished!!\n");
#endif
						/* save the recent data in old and mix buff and fb together!!
						*/
#ifdef DEBUG
						printf("before saving old fb\n");
#endif
						memcpy(old,fbuffer,fix.smem_len);
#ifdef DEBUG
						printf("old fbuffer saved!!\n");
#endif
						for(i=0;i<fix.smem_len;i++)
						{
							fbuffer[i]|=buff[i];
						}
						merge = 1;
						break;
					case 2://split button clicked
						mode = 2;
						//refresh_screen();
						/* code for Split */
						ret = write(serv_sock,buttons[mode].text,strlen(buttons[mode].text));
						if(ret<=0)
						{
							perror("write");
							exit(1);
						}
#ifdef DEBUG
						else
						{
							printf("send : %d\n",ret);
						}
#endif
						/* do sth .. what should we do 
						 * 'split' means we need to get back previous status of fb.
						 */
						if(merge)
						{
							memcpy(fbuffer,old,fix.smem_len);
							merge =0;
						}

						break;
					case 3://exit button clicked
						mode = 3;
						ret = write(serv_sock,buttons[mode].text,strlen(buttons[mode].text));
						if(ret<=0)
						{
							perror("write");
							exit(1);
						}
						put_string_center (xres/2, yres/2,   "Bye~!!", 1);
						sleep(2);
						fillrect (0, 0, xres - 1, yres - 1, 0);
						exit(1);
						break;
				}
		if(samp.pressure > 0)
		{
			if(mode == 0x80000000)
				line(x,y,samp.x,samp.y,2);
			x = samp.x;
			y = samp.y;
			mode |=0x80000000;
		}
		else
		{
			mode &= ~0x80000000;
		}

	}
	close_framebuffer();

	return 0;
}
