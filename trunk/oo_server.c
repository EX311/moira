/* 
 * theMeal Server Program
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
#include <linux/fb.h>

#include "oo.h"

extern struct myfb_info *myfb;

static void sig(int sig)
{
	fflush(stderr);
	printf("Quit - Sig #%d caught\n", sig);
	exit(2);
}

int main(int argc, char *argv[])
{
	int ret, count, offset;
	int serv_sock, clnt_sock;
	unsigned short *buff;
	char *myip;

	if (argc < 2) {
		printf("USAGE : %s myip\n", argv[0]);
		exit(1);
	}
	myip = argv[1];

	signal(SIGSEGV, sig);
	signal(SIGINT, sig);
	signal(SIGTERM, sig);

#ifdef DEBUG
	printf(" *** server for theMeal ***\n\n");
#endif
	
/* initialize */
	myfb = myfb_open();

	serv_sock = tcp_server_listen(ip2port(myip), 2);
	if (serv_sock < 0)
		exit(1);

	buff = (unsigned short *)malloc(myfb->fbfix.smem_len);
	clear_screen();
	
/* main process */
	while (1) {
		clnt_sock = tcp_server_accept(serv_sock);
		if (clnt_sock < 0)
			exit(1);

		count = 0;
		offset = 0;

		while ( (ret = read(clnt_sock, buff+offset, BUFSIZ)) != 0) {
			count += ret;
			offset += ret/2;
#ifdef DEBUG
			printf("READ: #%d Total: #%d\n", ret, count);
#endif
			if (count == myfb->fbfix.smem_len) {
				show_vfb(buff);
				count = 0;
			}
		}
		close(clnt_sock);
	}
	
	close(serv_sock);
	myfb_close();
	
#ifdef DEBUG
	printf("\n *** All done completely. ***\n");
#endif
	exit(0);
	return 0;
}
