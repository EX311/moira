/*
 * fbbmp.c : bmp drawing on frame buffer example
 *
 * Copyright(C) 2002 holelee
 * modified by amoolove
 */

#include <stdio.h>
#include <stdlib.h> /* for exit */
#include <string.h>
#include <dirent.h>
#include <unistd.h> /* for open/close .. */
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <linux/fb.h>  /* for fb_var_screeninfo, FBIOGET_VSCREENINFO */

#include "oo.h"
#include "read_proc.h"

#define	DEBUG	1
struct bmplist {
	int x, y, w, h;
	char name[30];
};

int quit = 1;
int x = 0;
int y = 0;
int vfb_count = 0;
int ts_sock[VFB_MAX] = {-1,-1,-1,-1};
struct color black = {0,0,0};
struct color white = {0xff,0xff,0xff};
struct bmplist list[7];

//char *ipaddr[VFB_MAX] = {"192.168.123.167", "192.168.123.182", "192.168.123.172", "192.168.123.157"};
int mylocation;
char ipaddr[VFB_MAX][16];
char *file_bmp = NULL;

extern struct myfb_info *myfb;
extern unsigned short *vfb_list[VFB_MAX];

void *ts_net_read(void *arg);
void *ts_local_read(void *arg);
void *tilt_read(void *arg);

static void sig(int sig)
{
	fflush(stderr);
	printf("Quit - Sig #%d caught\n", sig);
	fflush(stdout);
	exit(1);
}

struct tsdev *ts_init(void)
{
	struct tsdev *t = ts_open("/dev/ts0", 0);
	if (!t) {
		perror("ts_open");
		exit(1);
	}
	if (ts_config(t)) {
		perror("ts_config");
		exit(1);
	}

	return t;
}

void insert_list(struct bmplist *list, int y, char *name)
{
	list->x = 48;
	list->y = y;
	list->w = 224;
	list->h = 30;
	memcpy(list->name, name, strlen(name));
}

int list_handle(struct bmplist *list, struct ts_sample *samp)
{
	int inside = (samp->x >= list->x) && (samp->y >= list->y) && (samp->x < list->x + list->w) && (samp->y < list->y + list->h);

	if (samp->pressure > 0) {
		if (inside) 
			return 1;
		else
			return 0;
	}

	return 0;
}

void find_bmp(char *dirname)
{
	int i, ret, count, chat, quit;
	DIR *dp;
	struct dirent *dirp;
	struct ts_sample samp;
	struct tsdev *ts = ts_init();
	char temp[30];

	if ( (dp = opendir(dirname)) == NULL) {
		perror("opendir");
		exit(1);
	}

	i = 30;
	quit = 1;
	count = 0;
	while ( (dirp = readdir(dp)) != NULL) {
		if (strstr(dirp->d_name, ".bmp")) {
			strcpy(temp, dirname);
			strcat(temp, dirp->d_name);
			insert_list(&list[count], i, temp);
			drow_rect(list[count].x, list[count].y-15, list[count].x + list[count].w, list[count].y + list[count].h-15, white);
			put_string(96, i, dirp->d_name, white);
			i += 30;
			count++;
			if (count >= 7)
				break;
		}
	}
	closedir(dp);

	chat = 0;
	while (quit > 0) {
		ret = ts_read(ts, &samp, 1);
		if (ret < 0) {
			perror("ts_read");
			continue;
		}
		if (ret != 1)
			continue;

		for (i=0; i<count; i++) {
			if (list_handle(&list[i], &samp) > 0) {
				if (chat < 30) {
					chat++;
					continue;
				}

				file_bmp = list[i].name;
				quit = 0;
			}
		}
	}
#ifdef DEBUG
	fprintf(stderr, "BMP: %s\n", file_bmp);
#endif

	ts_close(ts);
	return;
}

int main(int argc, char *argv[])
{
	int i, ret, mode;
	int fb_sock[VFB_MAX] = {0,};
	bmphandle_t bh;
	pthread_t ts_thread[VFB_MAX], tilt_thread;
	void *thread_ret[VFB_MAX], *tilt_ret;
	char *invalid_msg = "Sorry, Maximum size 640x480";
	char *bye_msg = "Bye - thank you!";

	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);

	/* initialize */
	myfb = myfb_open();
	find_bmp("/root/bmp/");
	insert_ipaddr();

	bh = bmp_open(file_bmp);
	if (bh == NULL) {
		perror("bmp_open");
		exit(1);
	}

	if ( (bmp_width(bh) > 640) || (bmp_height(bh) > 480)) {
		clear_screen();
		put_string_center(myfb->fbvar.xres/2, myfb->fbvar.yres/2, invalid_msg, white);
		exit(0);
	} else if ( (bmp_width(bh) <= 320) && (bmp_height(bh) <= 240)) {
		mode = 0;
	} else {
		mode = 1;
	} 

	for (i=0; i<VFB_MAX; i++) {
		if (strlen(ipaddr[i]) > 14) {

		if (mylocation == i)
			continue;

		fb_sock[i] = tcp_client_connect(ipaddr[i], ip2port(ipaddr[i], 8000));
		if (fb_sock[i] < 0) 
			fprintf(stderr, "[FB] %s connect error\n", ipaddr[i]);
		else {
			vfb_count++;
			fprintf(stderr, "[FB] %s connect success!\n", ipaddr[i]);
		}

		if (mode == 0) {
			ts_sock[i] = tcp_client_connect(ipaddr[i], ip2port(ipaddr[i], 7000));
			if (ts_sock[i] < 0) 
				fprintf(stderr, "[TS] %s connect error\n", ipaddr[i]);
			else
				fprintf(stderr, "[TS] %s connect success!\n", ipaddr[i]);
		}
		}
	}
	
	set_vfb_buf(VFB_MAX);

	ret = pthread_create(&ts_thread[0], NULL, ts_local_read, (void *)"local");
	if (ret != 0) {
		perror("thread create");
		exit(1);
	}
#ifdef DEBUG
	else
		fprintf(stderr, "%d created\n", (int)ts_thread[0]);
#endif

	if (mode == 0) {
		ret = pthread_create(&tilt_thread, NULL, tilt_read, (void *)"tilt");
		if (ret != 0) {
			perror("thread create");
			exit(1);
		}
#ifdef DEBUG
		else
			fprintf(stderr, "%d created\n", (int)tilt_thread);
#endif
	}

	for (i=0; i<VFB_MAX; i++) {
		if (ts_sock[i] > 0) {
			ret = pthread_create(&ts_thread[i], NULL, ts_net_read, (void *)&(ts_sock[i]));
			if (ret != 0) {
				fprintf(stderr, "thread create: %s\n", ipaddr[i]);
				exit(1);
			}
#ifdef DEBUG
			else
				fprintf(stderr, "%d created\n", (int)ts_thread[i]);
#endif
		}
	}
	/* main loop */

	while (quit != 0) {
		clear_vfb_buf(VFB_MAX);
		sleep(1);
		buf_bmp(bh, x, y);
		for (i=0; i<VFB_MAX; i++) {
			if (fb_sock[i] > 0) {
				ret = fb_send(fb_sock[i], vfb_list[i], myfb->fbfix.smem_len);
#ifdef DEBUG
				fprintf(stderr, "[%d] send: #%d to %s\n", i, ret, ipaddr[i]);
#endif
			}
		}
		show_vfb(vfb_list[mylocation]);
	}

	for (i=0; i<VFB_MAX; i++) {
		if (ts_sock[i] > 0) {
			ret = pthread_cancel(ts_thread[i]);
			if (ret != 0)
				perror("thread cancel failed\n");
		}
	}

	if (mode == 0) {
		ret = pthread_cancel(tilt_thread);
		if (ret != 0)
			perror("thread cancel failed\n");
	}
	
	for (i=0; i<VFB_MAX; i++) {
		if (ts_sock[i] > 0) {
			ret = pthread_join(ts_thread[i], &thread_ret[i]);
			if (ret != 0)
				fprintf(stderr, "pthread_join: %s\n", ipaddr[i]);
#ifdef DEBUG
			else
				fprintf(stderr, "%d join ok!\n", (int)ts_thread[i]);
#endif
		}
	}

	if (mode == 0) {		
		ret = pthread_join(tilt_thread, &tilt_ret);
		if (ret != 0)
			fprintf(stderr, "pthread_join: Tilt\n");
#ifdef DEBUG
		else
			fprintf(stderr, "%d join ok!\n", (int)tilt_thread);
#endif
	}
	
	/* clean up */
	put_string_center(myfb->fbvar.xres/2, myfb->fbvar.yres/2, bye_msg, white);
	clear_vfb_buf(VFB_MAX);
	bmp_close(bh);

	for (i=0; i<VFB_MAX; i++) {
		if (fb_sock[i] > 0) {
			fb_send(fb_sock[i], vfb_list[i], myfb->fbfix.smem_len);
			shutdown(fb_sock[i], SHUT_WR);
			close(fb_sock[i]);
		}

		if (ts_sock[i] > 0) {
			shutdown(ts_sock[i], SHUT_RD);
			close(ts_sock[i]);
		}
	}

	free_vfb_buf(VFB_MAX);
	myfb_close();
	printf("All Done!\n");
	exit(0);
	return 0;
}

void *ts_net_read(void *arg)
{
	int ret, sock;
	char tsbuff[sizeof(struct ts_sample)];
	struct ts_sample samp;
#ifdef DEBUG
	fprintf(stderr, "%d started... \n", *(int *)arg);
#endif

	memset(tsbuff, 0, sizeof(struct ts_sample));

	ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if (ret != 0) {
		perror("pthread setcancelstate");
		exit(1);
	}
	ret = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	if (ret != 0) {
		perror("pthread setcanceltype");
		exit(1);
	}

	sock = *(int *)arg;
	while ( (quit != 0) && (ret = read(sock, tsbuff, sizeof(struct ts_sample)))) {
		memcpy(&samp, tsbuff, sizeof(struct ts_sample));
#ifdef DEBUG
		fprintf(stderr, "%d -> %ld.%06ld: %6d %6d %6d\n", sock, samp.tv.tv_sec, samp.tv.tv_usec, samp.x, samp.y, samp.pressure);
#endif

		if (sock == ts_sock[0]) {
			x = 0; y = 0;
		} else if (sock == ts_sock[1]) {
			x = 320; y = 0;
		} else if (sock == ts_sock[2]) {
			x = 0; y = 240;
		} else if (sock == ts_sock[3]) {
			x = 320; y = 240;
		}
	}

	shutdown(sock, SHUT_RDWR);
	close(sock);
	quit = 0;
	pthread_exit(0);
}

void *ts_local_read(void *arg)
{
	int ret;
	struct tsdev *ts;
	struct ts_sample l_samp;
#ifdef DEBUG
	fprintf(stderr, "%s started...\n", (char *)arg);
#endif

	ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if (ret != 0) {
		perror("pthread setcancelstate");
		exit(1);
	}
	ret = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	if (ret != 0) {
		perror("pthread setcanceltype");
		exit(1);
	}

	ts = ts_init();
	while (quit != 0) {
		ret = ts_read(ts, &l_samp, 1);
		if (ret < 0) {
			perror("ts_read");
			continue;
		}
		if (ret != 1)
			continue;
#ifdef DEBUG
		fprintf(stderr, "%s %ld.%06ld: %6d %6d %6d\n", (char *)arg, l_samp.tv.tv_sec, l_samp.tv.tv_usec, l_samp.x, l_samp.y, l_samp.pressure);
#endif

		switch (mylocation) {
			case 0:
				x = 0; y = 0;
				break;
			case 1:
				x = 320; y = 0;
				break;
			case 2:
				x = 0; y = 240;
				break;
			case 3:
				x = 320; y = 240;
				break;
		}
		if (l_samp.x > 300) {
			break;
		}
	}

	ts_close(ts);
	quit = 0;
	pthread_exit(0);
}

void *tilt_read(void *arg)
{
	int ret, value, pos = 0;
#ifdef DEBUG
	fprintf(stderr, "%s started... \n", (char *)arg);
#endif

	ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if (ret != 0) {
		perror("pthread setcancelstate");
		exit(1);
	}
	ret = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	if (ret != 0) {
		perror("pthread setcanceltype");
		exit(1);
	}

	while (1) {
		value = get_Senser();
		if (value < 100) {			// Left or Top
			pos--;
			if (pos < 0)
				pos = vfb_count;
		} else if (value > 120) {	// Right or Bottom
			pos++;
			if (pos > vfb_count)
				pos = 0;
		}

		switch (pos) {
			case 0:
				x = 0; y = 0;
				break;
			case 1:
				x = 320; y = 0;
				break;
			case 2:
				x = 0; y = 240;
				break;
			case 3:
				x = 320; y = 240;
				break;
		}
	}

	quit = 0;
	pthread_exit(0);
}
