/* 
 * theMeal Server Program
 * code by amoolove
 */

#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <unistd.h>
//#include <fcntl.h>
#include <signal.h>
//#include <sys/ioctl.h>
//#include <sys/mman.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <linux/fb.h>

#include "oo.h"

extern struct myfb_info *myfb;

static void sig(int sig)
{
	fflush(stderr);
	printf("Quit - Sig #%d caught\n", sig);
	fflush(stdout);
	exit(1);
}

int main(int argc, char *argv[])
{
	int ret, count, offset;
	int serv_sock, clnt_sock;
//	unsigned short *buff;
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
	fprintf(stderr, " *** server for theMeal ***\n\n");
#endif
	printf("theMeal: oo_server started!\n");
	
/* initialize */

	myfb = myfb_open();

	serv_sock = tcp_server_listen(ip2port(myip, 8000), 2);
	if (serv_sock < 0)
		exit(1);
		
//	buff = (unsigned short *)malloc(myfb->fbfix.smem_len);
/* main process */

	while (1) {
		clnt_sock = tcp_server_accept(serv_sock);
		if (clnt_sock < 0)
			exit(1);
		
		count = 0;
		offset = 0;

//		while ( (ret = read(clnt_sock, buff+offset, BUFSIZ)) != 0) {
		while ( (ret = read(clnt_sock, myfb->fb, myfb->fbfix.smem_len)) != 0) {
//			count += ret;
//			offset += ret/sizeof(unsigned short);
#ifdef DEBUG
			fprintf(stderr, "READ: #%d Total: #%d\n", ret, count);
#endif
//			if (count == myfb->fbfix.smem_len) {
//				show_vfb(buff);
//				count = 0;
//				offset = 0;
//			}
		}
		clear_screen();
		close(clnt_sock);
	}
	
	close(serv_sock);
	myfb_close();
	
#ifdef DEBUG
	fprintf(stderr, "\n *** All done completely. ***\n");
#endif
	exit(0);
	return 0;
}
