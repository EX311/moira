/* 
 * theMeal TouchScreen Server Program
 * code by amoolove
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <linux/fb.h>

#include "tslib.h"
#include "oo.h"

static void sig(int sig)
{
	fflush(stderr);
	printf("Quit - Sig #%d caught\n", sig);
	fflush(stdout);
	exit(1);
}

int main(int argc, char *argv[])
{
	int ret;
	int serv_sock, clnt_sock;
	char *myip;
	char *tsdevice = "/dev/ts0";
	struct tsdev *ts;

	if (argc < 2) {
		printf("USAGE : %s myip\n", argv[0]);
		exit(1);
	}
	myip = argv[1];

	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);

#ifdef DEBUG
	fprintf(stderr, " *** server for theMeal ***\n\n");
#endif
	printf("theMeal: oo_ts_server started!\n");
	
/* initialize */
	ts = ts_open(tsdevice, 0);
	if (!ts) {
		perror("ts_open");
		exit(1);
	}

	if (ts_config(ts)) {
		perror("ts_config");
		exit(1);
	}

	serv_sock = tcp_server_listen(ip2port(myip, 7000), 2);
	if (serv_sock < 0)
		exit(1);

/* main process */
	while (1) {
		int pid, status;
		char ts_ack[1];
		struct ts_sample samp, v_samp;
		clnt_sock = tcp_server_accept(serv_sock);
		if (clnt_sock < 0)
			exit(1);

		pid = fork();
		// Parent Process
		if (pid > 0) {
#ifdef DEBUG
			fprintf(stderr, "clnt_sock #%d pid #%d\n", clnt_sock, pid);
#endif
			close(clnt_sock);
			wait(&status);
			continue;
		} 
		/* Child Process
		 * send ts_sample structure and recv ack character
		 * unless ack is not 1, it will send ts_sample structure
		 */
		else { 		
			do {
				ret = ts_read(ts, &samp, 1);
				if (ret < 0) {
					perror("ts_read");
					continue;
				}

				if (ret != 1)
					continue;

//				v_samp = phy2vir_pos(samp, mylocation);
//				ret = write(clnt_sock, &v_samp, sizeof(struct ts_sample));
				ret = write(clnt_sock, &samp, sizeof(struct ts_sample));
#ifdef DEBUG
				fprintf(stderr, "%ld.%06ld: %6d %6d %6d\n", samp.tv.tv_sec, samp.tv.tv_usec, samp.x, samp.y, samp.pressure);
				fprintf(stderr, "To #%d write: %d\n", clnt_sock, ret);
#endif
				if (samp.x > 300 && samp.y < 20)
					break;
			} while (1);
			shutdown(clnt_sock, SHUT_WR);
			close(clnt_sock);
#ifdef DEBUG
			fprintf(stderr, "\n *** child process is done\n");
#endif
			exit(0);
		} // end child process
	} // end while(1)
	
	close(serv_sock);
#ifdef DEBUG
	fprintf(stderr, "\n *** All done completely. ***\n");
#endif
	exit(0);
	return 0;
}
