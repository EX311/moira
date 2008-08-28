
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "tslib.h"
#include "fbutils.h"

#define DEBUG 1

int serv_sock, clnt_sock;
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

#define NR_BUTTONS 2
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



int tcp_server_listen(int port, int num)
{
	int ret;
	int sock;
	struct sockaddr_in serv_addr;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		perror("socket()");
		return ret; 
	}

	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	ret = bind(sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
	if (ret == -1)
	{
		perror("bind()");
		return ret;
	}

	ret = listen(sock, num);
	if (ret == -1)
	{
		perror("listen()");
		return ret;
	}

	return sock;
}

int tcp_server_accept(int serv_sock)
{
	int ret;
	struct sockaddr_in clnt_addr;
	int clnt_addr_size = sizeof(clnt_addr);
	ret = accept(serv_sock, (struct sockaddr *)&clnt_addr, (socklen_t *)&clnt_addr_size);
	if (ret == -1)
	{
		perror("accept()");
		return ret;
	}

	return ret;
}

void *read_ts(void *data)
{
	int x,y;
	unsigned int i;
	unsigned int mode =0;
	while(1)
	{
		struct ts_sample samp;
		int ret;

		ret = ts_read(data, &samp,1);
		if(ret<0)
		{
			perror("ts_read");
			close_framebuffer();
			exit(1);
		}
		if(ret!=1)
			continue;
		for(i=0;i<NR_BUTTONS;i++)
			if(button_handle(&buttons[i],&samp))
				switch(i)
				{
					case 0://clear button clicked
						mode = 0;
						refresh_screen();
						break;
					case 1://exit button clicked
						mode = 1;
						put_string_center(xres/2,yres/2, "Bye~!!",1);
						sleep(2);
						fillrect(0,0,xres-1,yres-1,0);
						exit(1);
						break;
				}
		if(samp.pressure>0)
		{
			if(mode == 0x80000000)
				line(x,y,samp.x,samp.y,2);
			x = samp.x;
			y = samp.y;
			mode|=0x80000000;
		}
		else
		{
			mode&=~0x80000000;
		}
	}
}



int main(int argc, char *argv[])
{
	struct tsdev *ts;
	unsigned int i;
	int ret;
	int connect = 0;
	char buff[10]="";
	char *myaddr;
	pthread_t ts_thread;
	char *tsdevice = "/dev/ts0";

	if (argc < 2) 
	{
		printf("USAGE : %s myip\n", argv[0]);
		exit(1);
	}


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
	//myfb = myfb_open();
	if(open_framebuffer()){
		close_framebuffer();
		exit(1);
	}
	for(i=0;i<NR_COLORS;i++)
		setcolor(i,palette[i]);

	memset(&buttons,0,sizeof(buttons));
	buttons[0].w  = 70;
	buttons[0].h  = 20;
	buttons[0].x = 40; buttons[0].y = 200;
	buttons[0].text = "Clear";
	buttons[1].w = 20;
	buttons[1].h = 20;
	buttons[1].x = 280; buttons[1].y = 20;
	buttons[1].text = "X";

	refresh_screen();

	myaddr = argv[1];
	serv_sock = tcp_server_listen(ip2port(myaddr,7000),2);
	if(serv_sock < 0)
	{
		perror("tcp_server_listen");
		close_framebuffer();
		exit(1);
	}

	pthread_create(&ts_thread, NULL, read_ts,ts);
	while(1)
	{

		clnt_sock = tcp_server_accept(serv_sock);
		if(clnt_sock <0)
		{
			perror("tcp_server_accept");
			exit(1);
		}
		else{
			connect = 1;
#ifdef DEBUG
			printf("accept successful!!\n");
#endif
		}
		while(1)
		{
			ret = read(clnt_sock,buff,10);
#ifdef DEBUG
			printf("buff = %s\n",buff);
#endif
			if(!strcmp(buff,"Merge"))
			{
				//merge mode
#ifdef DEBUG
				printf("Merge mode on\n");
#endif
				ret = write(clnt_sock,fbuffer,fix.smem_len);

				if(ret<=0)
				{
					perror("write");
					exit(1);
				}
#ifdef DEBUG
				else
				{
					printf("server sent : %d\n",ret);
				}
#endif
			}
			else if(!strcmp(buff,"Split"))
			{
				//split mode
#ifdef DEBUG
				printf("Split mode on\n");
#endif
			}
			else if(!strcmp(buff,"X"))
			{
#ifdef DEBUG
				printf("Exit\n");
#endif
			}

		}
		
	}
	pthread_join(ts_thread, NULL);
	close(clnt_sock);
}
