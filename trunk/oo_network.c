/*
 * Network Lib for theMeal Project
 * code by amoolove
 *
 * U need tslib
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "oo.h"

extern struct myfb_info *myfb;

int error_handling(char *message)
{
	fputs(message, stderr);
	fputs(" failed\n", stderr);
	return -1;
}

/*
 * from string of ip to integer port NO.
 */
int ip2port(char *ip)
{
	return ( (inet_addr(ip)) >> 24) + 8000;
}
/*
 * TCP server functions
 *

 * all process socket(), bind(), and listen()
 * with string of port NO. and count of listen socket
 * returns socket fd
 */
int tcp_server_listen(int port, int num)
{
	int ret;
	int sock;
	struct sockaddr_in serv_addr;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		return error_handling("socket()");

	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	ret = bind(sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
	if (ret == -1)
		return error_handling("bind()");

	ret = listen(sock, num);
	if (ret == -1)
		return error_handling("listen()");

	return sock;
}
/* 
 * accept() process
 * with server socket fd
 * returns client socket fd
 */
int tcp_server_accept(int serv_sock)
{
	int ret;
	struct sockaddr_in clnt_addr;
	int clnt_addr_size = sizeof(clnt_addr);
	ret = accept(serv_sock, (struct sockaddr *)&clnt_addr, (socklen_t *)&clnt_addr_size);
	if (ret == -1)
		return error_handling("accept()");

	return ret;
}
/*
 * TCP client function
 * with string of server address and port NO.
 * returns socket fd
 */
int tcp_client_connect(char *server_addr, int port)
{
	int ret;
	int sock;
	struct sockaddr_in serv_addr;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		return error_handling("socket()");

	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(server_addr);
	serv_addr.sin_port = htons(port);

	ret = connect(sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
	if (ret == -1) {
		close(sock);
		return error_handling("connect()");
	}

	return sock;
}
/*
 * input of framebuffer data I/O functions
 *

 * with socket fd, input data structure, send data size
 * returns writing data size
 */
int input_send(int sock, struct oo_i_data *data, int size)
{
	int ret;
	ret = write(sock, data, size);
	return ret;
}
/* with socket fd, buffer
 * returns read data structure pointer
 */
struct oo_i_data *input_recv(int sock, char *buff)
{
	int ret;

	ret = read(sock, buff, I_BUFSIZE);
	return (struct oo_i_data *)buff;
}
/* with socket fd, fb data structure, send data size
 * returns writing data size
 */
int fb_send(int sock, unsigned short *data, int size)
{
	int ret;
	ret = write(sock, data, size);
//	ret = send(sock, data, size, MSG_DONTWAIT);
	return ret;
}
/* with socket fd, buffer
 * returns read data structure pointer
 */
struct oo_fb_data *fb_recv(int sock, char *buff, int *size)
{
	int ret;

	ret = read(sock, buff, I_BUFSIZE);
	*size = ret;
	return (struct oo_fb_data *)buff;
}	

int data4monitor(struct oo_fb_data *fb_data, int x, int y, unsigned short data)
{
	if (fb_data) {
		fb_data->offset = y * M_XRES + x;
		fb_data->pix_data = data;
		return 0;
	}
	return -1;
}

struct oo_fb_data *alloc_net_buf(int size)
{
	struct oo_fb_data *buf = (struct oo_fb_data *)malloc(size);
	return buf;
}

int free_net_buf(struct oo_fb_data *buf)
{
	if (buf) {
		free(buf);
		return 0;
	}
	return -1;
}
