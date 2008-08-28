#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tslib.h"
#include "oo.h"

#define	ICONSIZE	48
extern struct myfb_info *myfb;

struct icon {
	int x, y, w, h;
	char *name;
	char *action;
	char *arg;
};

int chat_count = 0;
int icon_count = 5;
char *program_list = "BMP";
struct color white = {0xff, 0xff, 0xff};

static void sig(int sig)
{
	fflush(stderr);
	printf("Quit - Sig #%d caught\n", sig);
	fflush(stdout);
	exit(1);
}

void register_icon(struct icon *icon, int x, int y, char *name, char *action, char *arg)
{
	char *t_temp = (char *)malloc(strlen(name)+1);
	char *t_act = (char *)malloc(strlen(action)+1);
	char *t_arg = (char *)malloc(strlen(arg)+1);
	memcpy(t_temp, name, strlen(name));
	t_temp[strlen(name)] = '\0';
	memcpy(t_act, action, strlen(action));
	t_act[strlen(action)] = '\0';
	memcpy(t_arg, arg, strlen(arg));
	t_arg[strlen(arg)] = '\0';

	icon->x = x;
	icon->y = y;
	icon->w = icon->h = ICONSIZE;
	icon->name = t_temp;
	icon->action = t_act;
	icon->arg = t_arg;
}

void free_icon(struct icon *icon)
{
	if (icon->name)
		free(icon->name);
	if (icon->action)
		free(icon->action);
	if (icon->arg)
		free(icon->arg);
}

void draw_icon(struct icon *icon)
{
	drow_rect(icon->x, icon->y, icon->x+icon->w, icon->y+icon->h, white);
	put_string_center(icon->x+icon->w/2, icon->y+icon->h/2, icon->name, white);
}

char *icon_handle(struct icon *icon, struct ts_sample *samp)
{
	int inside = (samp->x >= icon->x) && (samp->y >= icon->y) && (samp->x < icon->x + icon->w) && (samp->y < icon->y + icon->h);

	if (samp->pressure > 0) {
		if (inside) {
			chat_count++;
			return icon->action;
		} else
			return NULL;
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	int ret, x, y, i;
	struct tsdev *ts;
	struct icon icon[icon_count];
	char *tsdevice = "/dev/ts0";
	char *bmp_list[icon_count];

	bmp_list[0] = "rebis.bmp";
	bmp_list[1] = "sooae.bmp";
	bmp_list[2] = "zero1.bmp";
	bmp_list[3] = "yuna.bmp";
	bmp_list[4] = "alba.bmp";

	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);

	printf("theMeal: oo_ui started!\n");

	myfb = myfb_open();

	ts = ts_open(tsdevice, 0);
	if (!ts) {
		perror("ts_open");
		exit(1);
	}

	if (ts_config(ts)) {
		perror("ts_config");
		exit(1);
	}

	x = y = ICONSIZE;
	for (i=0; i<icon_count; i++) {
		if ( (i != 0) && (i%3 == 0)) {
			x = ICONSIZE;
			y += (2*ICONSIZE);
		}
		register_icon(&icon[i], x, y, "BMP", "/root/bmp", bmp_list[i]);
		x += (2*ICONSIZE);
	}

	while (1) {
		int pid, status;
		struct ts_sample samp;
		char *t;

		clear_screen();
		for (i=0; i<icon_count; i++)
			draw_icon(&icon[i]);

		ret = ts_read(ts, &samp, 1);

		if (ret < 0) {
			perror("ts_read");
			exit(1);
		}

		if (ret != 1)
			continue;

		if (samp.x > 300 && samp.y > 220)
			break;

		for (i=0; i<=icon_count; i++) {
			if ( (t = icon_handle(&icon[i], &samp))) {
				if (chat_count < 20)
					continue;
				chat_count = 0;
				
				printf("TEST :%s\n", t);
			
				pid = fork();
				if (pid == 0) {
#ifdef DEBUG
					printf("Child PID #%d\n", getpid());
#endif
					ret = execl(icon[i].action, icon[i].name, icon[i].arg, 0);
//					ret = execl("/bin/ls", "ls", "-l", 0);
					if (ret < 0) {
						perror("execl");
						exit(1);
					}
				} else {
#ifdef DEBUG
					printf("Exec PID #%d\n", pid);
#endif
					sleep(1);
					wait(&status);
#ifdef DEBUG
					printf("Child terminated by #%d\n", status);
#endif

				}
			}
		}

#ifdef DEBUG
//		printf("%ld.%06ld: %6d %6d %6d\n", samp.tv.tv_sec, samp.tv.tv_usec, samp.x, samp.y, samp.pressure);
#endif
	}

	ts_close(ts);
	myfb_close();
#ifdef DEBUG
	printf("\n *** All done completely. ***\n");
#endif
	exit(0);
	return 0;
}
