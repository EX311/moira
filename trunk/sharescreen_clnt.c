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

int serv_sock[3];
int my_sock,clnt_sock;
int numofdevice,isconnect,server_share;

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
	memset(&clnt_addr,0,clnt_addr_size);
	ret = accept(serv_sock, (struct sockaddr *)&clnt_addr, (socklen_t *)&clnt_addr_size);
	if (ret == -1)
	{
		perror("accept()");
		return ret;
	}

	return ret;
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


void *read_and_draw(void *data)
{
	int x,y;
	int i,ret,serv_socket;
	serv_socket =*(int *)data;
#ifdef DEBUG
	printf("*data = %d serv_socket = %d\n",*(int *)data,serv_socket);
#endif
	x=y=0;
	while(1)
	{
		struct ts_sample samp;
		ret = read(serv_socket,&samp,sizeof(samp));
		if(ret<0)
		{
			perror("read");
			exit(1);
		}
		if(ret<sizeof(samp))
			continue;
		if(x==0 && y==0)
		{
			x=samp.x;
			y=samp.y;

		}
		line(x,y,samp.x,samp.y,2);
		x = samp.x;
		y = samp.y;
	}
}

void *serverinit_and_write(void *data)
{
	char *myaddr,buff[10];
	int ret;
	myaddr = data;
	my_sock = tcp_server_listen(ip2port(myaddr,7777),2);
	while(1)
	{
		clnt_sock = tcp_server_accept(my_sock);
		isconnect = 1;
		while(1)
		{
			/* client has come */
			/* read client's request */
			/* check mode first */
			/* because we need to know if client click share or alone or exit button */ 
			ret = read(clnt_sock,buff,10);	
#ifdef DEBUG
			printf("buff = %s\n",buff);
#endif
			if(!strcmp(buff,"Share"))
			{
				//share mode
				server_share =1;
				buff[0] = '\0';
			}
			else if(!strcmp(buff,"Alone"))
			{
				//non-share mode
				server_share = 0;
				buff[0] = '\0';
			}
			else if(!strcmp(buff,"X"))
			{
				server_share = 0;
				buff[0] = '\0';
				break;
			}
			buff[0] = '\0';
		}
	}

}

int main(int argc,char *argv[])
{
	struct tsdev *ts;
	int count=0;
#ifdef DEBUG
	//FILE *fp;
#endif
	int read_flag = 0;
	unsigned int i,j;
	unsigned int share;
	unsigned int mode = 0;
	unsigned char *buff,*old;
	//unsigned short temp;
	pthread_t read_thread;
	pthread_t serv_thread;

	char *serv_addr[3];
	char *myip;
	//this has to be changed like this
	//char *serv_addr[3];//without initialization

	char *tsdevice="/dev/ts0";

#ifdef DEBUG
	//fp = fopen("result.txt","r+w");
#endif

	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);

	if(argc<2)
	{
		printf("Usage : %s your ip\n",argv[0]); 
		exit(1);
	}
#ifdef DEBUG
	printf("your ip : %s\n",argv[1]);
#endif	
	myip = (char *)malloc(strlen(argv[1])+1);
	strcpy(myip,argv[1]);


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
	buttons[1].text = "Share";
	buttons[2].text = "Alone";
	buttons[3].w = 20;
	buttons[3].h = 20;
	buttons[3].x = 280; buttons[3].y = 20;
	buttons[3].text = "X";

	refresh_screen();
	numofdevice = 1;
	//what we need to do is like this
	//
	//numofdevice = getnumberofdevice();
	//getipaddr(serv_addr,numofdevice);
	//
	// we don't need  position of devices right now
	// just need to get the other devices ip addresses in array "serv_addr[3]"
	//
	//then we need to connect to server
	//like this
	//
	serv_addr[0] = "192.168.77.77";
	for(i=0;i<numofdevice;i++)
	{
		serv_sock[i] = tcp_client_connect(serv_addr[i],ip2port(serv_addr[i],7777));
	}

	//this is test code before integration.
	//so I assume there's only one connection.
	//serv_sock = tcp_client_connect(serv_addr,ip2port(serv_addr,7777));
	/* think i don't need to do this too.
	 * 
	 */
	//buff = (unsigned char *)malloc(fix.smem_len);
//	old = (unsigned char *)malloc(fix.smem_len);
//	memset(buff,0,fix.smem_len);
//	memset(old,0,fix.smem_len);

	/*
	 * after making connections with servers.
	 * we need our server working for other clients.
	 */


	pthread_create(&serv_thread,NULL,serverinit_and_write,(void *)myip);
	while(1)
	{
		struct ts_sample samp;
		int ret;
		int x,y;

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
					case 1://share button clicked
						mode = 1;
						//refresh_screen();
						/* Share mode  */
						/* Whar we need to do here is just set the mode and let other device */
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
						//thread starts and set flag 
						//if flag is set don't start threads again.
						//thread func must read from server samp and draw on fb.
						if(!share)
						{
							pthread_create(&read_thread,NULL,read_and_draw,serv_sock);
							share = 1;
						}
						//it's gonna be changed like this
						//for(i=0;i<numofdevice;i++)
						//	pthread_create(&read_thread[i],NULL,read_adn_draw,serv_sock[i]);
						break;
					case 2:
						//alone button clicked
						/* non-share mode */
						mode = 2;
						//kill threads.
						pthread_kill(read_thread,SIGKILL);
						//this will be changed like this
						//for(i=0;i<numofdevice;i++)
						//{
						//	pthread_kill(&read_thread[i],SIGKILL);
						//}
						share = 0;
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
			if(server_share)
			{
				write(clnt_sock,samp,sizeof(samp));
			}
			if(mode == 0x80000000 ||mode == 0x80000002)
				line(x,y,samp.x,samp.y,2);
			else if(mode == 0x80000001)
			{
				/* mode 0x80000001 means it's sharing mode
				 * we draw on our screen and send it to others as well.
				 *
				 */ 
				//we could do like this
				for(i=0;i<numofdevice;i++)
				{
					write(serv_sock[i],samp,sizeof(samp));
				}
			//	write(serv_sock,samp,sizeof(samp));
				line(x,y,samp.x,samp.y,2);
			}
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
	pthread_join(read_thread,NULL);
	pthread_join(serv_thread,NULL);

	return 0;
}
