/*
 * Header file for theMeal Project
 * code by amoolove
 * For Rev 100
 */

#ifndef _OO_H
#define _OO_H

#include "tslib.h"
#include "fb-util.h"

#define	M_XRES		1280
#define	I_BUFSIZE	1024
#define	FB_BUFSIZE	10240

struct oo_i_data {
	struct ts_sample samp;
};

struct oo_fb_data {
	int offset;
	unsigned short pix_data;
}; /* Data for monitor */

extern int ip2port(char *);
extern int tcp_server_listen(int, int);
extern int tcp_server_accept(int);
extern int tcp_client_connect(char *, int);
extern int input_send(int, struct oo_i_data *,int);
extern struct oo_i_data *input_recv(int, char *);
extern int fb_send(int, unsigned short *, int);
extern struct oo_fb_data *fb_recv(int, char *, int *);
extern int data4monitor(struct oo_fb_data *, int , int, unsigned short);

extern struct oo_fb_data *alloc_net_buf(int);
extern int free_net_buf(struct oo_fb_data *);

extern void buf_bmp(bmphandle_t, int, int);
extern void monitor_bmp(bmphandle_t, int, int, struct oo_fb_data *);
extern void clear_screen(void);
#endif /* _OO_H */
