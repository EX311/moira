/*
 * Header file for theMeal Project
 * code by amoolove
 *
 */

#ifndef _OO_H
#define _OO_H

#include "tslib.h"

#define	I_BUFSIZE	1024
#define	FB_BUFSIZE	10240

struct oo_i_data {
	struct ts_sample samp;
};

struct oo_fb_data {
	int offset;
	unsigned short pix_data;
};

extern int ip2port(char *);
extern int tcp_server_listen(int, int);
extern int tcp_server_accept(int);
extern int tcp_client_connect(char *, char *);
extern int input_send(int, struct oo_i_data *,int);
extern struct oo_i_data *input_recv(int, char *);
extern int fb_send(int, struct oo_fb_data *, int);
extern struct oo_fb_data *fb_recv(int, char *, int *);
extern int data4monitor(struct oo_fb_data *, int , int);
#endif /* _OO_H */
