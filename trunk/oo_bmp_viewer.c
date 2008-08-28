/*
 * fbbmp.c : bmp drawing on frame buffer example
 *
 * Copyright(C) 2002 holelee
 * modified by amoolove
 */

#include <stdio.h>
#include <stdlib.h> /* for exit */
#include <string.h>
#include <unistd.h> /* for open/close .. */
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <linux/fb.h>  /* for fb_var_screeninfo, FBIOGET_VSCREENINFO */
#include "oo.h"

int quit = 1;
int x = 0;
int y = 0;
int ts_sock[VFB_MAX];
struct color black = {0,0,0};
//char *ipaddr[VFB_MAX] = {"192.168.1.10", "192.168.77.30", "192.168.77.55", "192.168.77.77"};
char *ipaddr[VFB_MAX] = {"192.168.1.10", "192.168.77.20", "192.168.77.55", "192.168.77.77"};

extern struct myfb_info *myfb;

void *ts_net_read(void *arg);
void *ts_local_read(void *arg);

static void sig(int sig)
{
	fflush(stderr);
	printf("Quit - Sig #%d caught\n", sig);
	fflush(stdout);
	exit(1);
}

int main(int argc, char *argv[])
{
	int i, ret;
	int fb_sock[VFB_MAX];
	struct oo_fb_data *buf_monitor;
	bmphandle_t bh;
	pthread_t ts_thread[VFB_MAX];
	void *thread_ret[VFB_MAX];

	if (argc < 2) {
		printf("USAGE %s bmpfile\n", argv[0]);
		exit(1);
	}

	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);

	/* initialize */
	myfb = myfb_open();
/*
	fb_sock[0] = tcp_client_connect(ipaddr[0], 8192);
	if (fb_sock[0] < 0) {
		fprintf(stderr, "[MONITOR] %s connect error\n", ipaddr[0]);
	}
*/
//	for (i=1; i<VFB_MAX; i++) {
	for (i=1; i<2; i++) {
		fb_sock[i] = tcp_client_connect(ipaddr[i], ip2port(ipaddr[i], 8000));
		if (fb_sock[i] < 0) 
			fprintf(stderr, "[FB] %s connect error\n", ipaddr[i]);
		else
			fprintf(stderr, "[FB] %s connect success!\n", ipaddr[i]);

		ts_sock[i] = tcp_client_connect(ipaddr[i], ip2port(ipaddr[i], 7000));
		if (ts_sock[i] < 0) 
			fprintf(stderr, "[TS] %s connect error\n", ipaddr[i]);
		else
			fprintf(stderr, "[TS] %s connect success!\n", ipaddr[i]);
	}

	bh = bmp_open(argv[1]);
	if (bh == NULL) {
		perror("bmp_open");
		exit(1);
	}

	set_vfb_buf(VFB_MAX);
	buf_monitor = alloc_net_buf(sizeof(struct oo_fb_data)*bmp_width(bh)*bmp_height(bh)*16/8);

	ret = pthread_create(&ts_thread[0], NULL, ts_local_read, (void *)"local");
	if (ret != 0) {
		perror("thread create");
		exit(1);
	}
#ifdef DEBUG
	else
		fprintf(stderr, "%d created\n", (int)ts_thread[0]);
#endif
	for (i=1; i<VFB_MAX; i++) {
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
		buf_bmp(bh, x, y);
		for (i=1; i<VFB_MAX; i++) {
			if (fb_sock[i] > 0) {
//				fb_send(fb_sock[i], vfb_list[i], myfb->fbfix.smem_len);
			}
		}
		show_vfb(vfb_list[0]);
/*
		if (fb_sock[0] > 0) {
			monitor_bmp(bh, 0, 0, buf_monitor);
			ret = write(fb_sock[0], buf_monitor, sizeof(struct oo_fb_data)*bmp_width(bh)*bmp_height(bh)*16/8);
		}
*/
	}

	for (i=0; i<VFB_MAX; i++) {
		if (ts_sock[i] > 0) {
			ret = pthread_cancel(ts_thread[i]);
			if (ret != 0)
				perror("thread cancel failed\n");
		}
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

	/* clean up */
	free_net_buf(buf_monitor);
	free_vfb_buf(VFB_MAX);
	bmp_close(bh);

	for (i=0; i<VFB_MAX; i++) {
		if (fb_sock[i] > 0) {
			shutdown(fb_sock[i], SHUT_WR);
			close(fb_sock[i]);
		}

		if (ts_sock[i] > 0) {
			shutdown(ts_sock[i], SHUT_RD);
			close(ts_sock[i]);
		}
	}

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

		x = 320;
		write(sock, "1", 1);
	}

	write(sock, "0", 1);

	shutdown(sock, SHUT_RDWR);
	close(sock);
	quit = 0;
	pthread_exit(0);
}

void *ts_local_read(void *arg)
{
	int ret;
	struct tsdev *ts;
	struct ts_sample samp;
	char *tsdevice = "/dev/ts0";
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

	ts = ts_open(tsdevice, 0);
	if (!ts) {
		perror("ts_open");
		exit(1);
	}
	if (ts_config(ts)) {
		perror("ts_config");
		exit(1);
	}

	while (quit != 0) {
		ret = ts_read(ts, &samp, 1);
		if (ret < 0) {
			perror("ts_read");
			continue;
		}
		if (ret != 1)
			continue;
#ifdef DEBUG
		fprintf(stderr, "%s %ld.%06ld: %6d %6d %6d\n", (char *)arg, samp.tv.tv_sec, samp.tv.tv_usec, samp.x, samp.y, samp.pressure);
#endif

		x = 0;
		if (samp.x > 300)
			break;
	}

	ts_close(ts);
	quit = 0;
	pthread_exit(0);
}

