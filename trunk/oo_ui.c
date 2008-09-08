#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "tslib.h"
#include "oo.h"

#define	MAX_CMD		6
#define	ICONSIZE	48

struct icon {
	int x, y, w, h;
	char *name;
	struct color color;
	struct icon *next;
};

struct cmd_list {
	char *path;
	char *args;
	struct cmd_list *next;
};

extern struct myfb_info *myfb;
extern unsigned short *vfb_list[VFB_MAX];

int icon_count, chat_count = 0;
char *file_ui = "oo_ui.lua";
char bg_image[25];
struct color bgcolor;
struct icon *head = NULL;
struct cmd_list *cmdlist = NULL;
struct color white = {0xff, 0xff, 0xff};

void insert_icon(int x, int y, char *name, struct color color)
{
	struct icon *temp;
	struct icon *p_temp = head;
	if (p_temp == NULL) {
		temp = (struct icon *)malloc(sizeof(struct icon));
		temp->x = x;
		temp->y = y;
		temp->w = ICONSIZE;
		temp->h = ICONSIZE;
		temp->color = color;
		temp->name = (char *)malloc(strlen(name)+1);
		memcpy(temp->name, name, strlen(name));
		temp->name[strlen(name)] = '\0';
		temp->next = NULL;
		head = temp;
	} else {
		while (p_temp->next != NULL)
			p_temp = p_temp->next;

		temp = (struct icon *)malloc(sizeof(struct icon));
		temp->x = x;
		temp->y = y;
		temp->w = ICONSIZE;
		temp->h = ICONSIZE;
		temp->color = color;
		temp->name = (char *)malloc(strlen(name)+1);
		memcpy(temp->name, name, strlen(name));
		temp->name[strlen(name)] = '\0';
		temp->next = NULL;
		p_temp->next = temp;
	}
}

void free_icon(void)
{
	struct icon *temp;
	struct icon *p_temp = head;
	while (p_temp->next) {
		temp = p_temp->next;
		if (p_temp->name)
			free(p_temp->name);
		free(p_temp);
		p_temp = temp;
	}

	if (p_temp->name)
		free(p_temp->name);
	free(p_temp);
	head = NULL;
}

int lua_set_icon_count(lua_State *L)
{
	lua_gettop(L);
	icon_count = (int)lua_tonumber(L,1);
#ifdef DEBUG
	fprintf(stderr, "icon count #%d\n", icon_count);
#endif
	return 0;
}

int lua_set_icon_info(lua_State *L)
{
	lua_gettop(L);
	insert_icon((int)lua_tonumber(L,1), (int)lua_tonumber(L,2), (char *)lua_tostring(L,3), reveres_pixel((unsigned short)lua_tonumber(L,4)));
	return 0;
}

void insert_cmd(char *path, char *args)
{
	struct cmd_list *temp;
	struct cmd_list *p_temp = cmdlist;
	if (p_temp == NULL) {
		temp = (struct cmd_list *)malloc(sizeof(struct cmd_list));
		temp->path = (char *)malloc(strlen(path)+1);
		memcpy(temp->path, path, strlen(path));
		temp->path[strlen(path)] = '\0';
		temp->args = (char *)malloc(strlen(args)+1);
		memcpy(temp->args, args, strlen(args));
		temp->args[strlen(args)] = '\0';
		temp->next = NULL;
		cmdlist = temp;
	} else {
		while (p_temp->next != NULL)
			p_temp = p_temp->next;

		temp = (struct cmd_list *)malloc(sizeof(struct cmd_list));
		temp->path = (char *)malloc(strlen(path)+1);
		memcpy(temp->path, path, strlen(path));
		temp->path[strlen(path)] = '\0';
		temp->args = (char *)malloc(strlen(args)+1);
		memcpy(temp->args, args, strlen(args));
		temp->args[strlen(args)] = '\0';
		temp->next = NULL;
		p_temp->next = temp;
	}
}

void free_cmd(void)
{
	struct cmd_list *temp;
	struct cmd_list *p_temp = cmdlist;
	while (p_temp->next) {
		temp = p_temp->next;
		if (p_temp->path)
			free(p_temp->path);
		if (p_temp->args)
			free(p_temp->args);
		free(p_temp);
		p_temp = temp;
	}

	if (p_temp->path)
		free(p_temp->path);
	if (p_temp->args)
		free(p_temp->args);
	free(p_temp);
	cmdlist = NULL;
}

int lua_set_cmd_list(lua_State *L)
{
	lua_gettop(L);
	insert_cmd((char *)lua_tostring(L,1), (char *)lua_tostring(L,2));
	return 0;
}

void set_bgcolor(void)
{
	memset(myfb->fb, makepixel(bgcolor), myfb->fbvar.xres*15);
}

int lua_set_bgcolor(lua_State *L)
{
	lua_gettop(L);
	bgcolor.r = (unsigned short)lua_tonumber(L,1);
	bgcolor.g = (unsigned short)lua_tonumber(L,2);
	bgcolor.b = (unsigned short)lua_tonumber(L,3);
#ifdef DEBUG
	fprintf(stderr, "BG COLOR : #%x%x%x\n", bgcolor.r, bgcolor.g, bgcolor.b);
#endif
	return 0;
}

void set_bgimage(void)
{
	if (chat_count > 0)
		return;
	bmphandle_t bh;

	set_vfb_buf(1);
	bh = bmp_open(bg_image);
	if (bh == NULL) {
		perror("bmp_open");
		return;
	}
	buf_bmp(bh, 0, 0);
	show_vfb(vfb_list[0]);
	bmp_close(bh);
	free_vfb_buf(1);
}

int lua_set_bgimage(lua_State *L)
{
	size_t len;
	lua_gettop(L);
	memcpy(bg_image, (char *)lua_tolstring(L, 1, &len), len);
#ifdef DEBUG
	fprintf(stderr, "BG image : %s\n", bg_image);
#endif
	return 0;
}

void draw_icon(struct icon *icon)
{
	drow_rect(icon->x, icon->y, icon->x + icon->w, icon->y + icon->h, icon->color);
	put_string_center(icon->x + icon->w/2, icon->y + icon->h/2, icon->name, icon->color);
}

int icon_handle(struct icon *icon, struct ts_sample *samp)
{
	int inside = (samp->x >= icon->x) && (samp->y >= icon->y) && (samp->x < icon->x + icon->w) && (samp->y < icon->y + icon->h);
	if (samp->pressure > 0) {
		if (inside) {
			chat_count++;
			return 1;
		} else {
			return 0;
		}
	}

	return 0;
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

int main(int argc, char *argv[])
{
	int ret;
	struct tsdev *ts;
	lua_State *L;

	myfb = myfb_open();
	ts = ts_init();

	L = luaL_newstate();
	luaopen_base(L);

	lua_register(L, "set_count", lua_set_icon_count);
	lua_register(L, "set_icon", lua_set_icon_info);
	lua_register(L, "set_cmd", lua_set_cmd_list);
	lua_register(L, "set_bgcolor", lua_set_bgcolor);
	lua_register(L, "set_bgimage", lua_set_bgimage);

	luaL_dofile(L, file_ui);
	lua_close(L);

	while (1) {
		int pid, state = 0;
		struct ts_sample samp;
		struct icon *i;
		struct cmd_list *j;

		set_bgimage();
		set_bgcolor();
		if (head == NULL) {
			put_string_center(myfb->fbvar.xres/2, myfb->fbvar.yres/2, "Sorry, No Apps. registered!", white);
#ifdef DEBUG
			fprintf(stderr, "No Apps!\n");
#endif
			break;
		}

		for (i=head; i != NULL; i=i->next)
			draw_icon(i);

		ret = ts_read(ts, &samp, 1);
		if (ret < 0) {
			perror("ts_read");
			continue;
		}
		if (ret != 1)
			continue;

		if (samp.x > 310 && samp.y >230)
			break;

		for (i=head, j=cmdlist; i != NULL; i=i->next, j=j->next) {
			if (icon_handle(i, &samp) > 0) {
				if (chat_count < 20)
					continue;
				chat_count = 0;

				pid = fork();
				if (pid == 0) {
#ifdef DEBUG
					fprintf(stderr, " *** This is CHILD! ***\n");
#endif
					if (j)
						ret = execl(j->path, j->args, 0);
					if (ret < 0) {
						perror("execl");
						exit(1);
					}
				} else {
					sleep(1);
					wait(&state);
#ifdef DEBUG
					fprintf(stderr, " *** End of CHILD! ***\n");
#endif
				}
			}
		}
	}

	clear_screen();
	put_string_center(myfb->fbvar.xres/2, myfb->fbvar.yres/2, "Thanks for Use!", white);

	free_icon();
	free_cmd();
	ts_close(ts);
	myfb_close();
	return 0;
}
