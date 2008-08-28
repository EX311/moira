#include "oo.h"

extern struct oo_button 

void draw_button(struct oo_button *button, char *string)
{
	drow_rect(button->x, button->y, button->w, button->h, button->color);
	put_string_center(button->x + button->w/2, button->y + button->h/2, string, button->color);
}

int check_button(struct oo_button *button, struct ts_sample *samp)
{
	return (samp->x >= button->x) && (samp->y >= button->y) && (samp->x < button->x + button->w) && (samp->y < button->y + button->h);
}

/* HOW TO CHATTERING
 *
 * first, Declaration chat variable;
 * second, in handle function, chat++ will be placed properly;
 * last, in main function, after handle function call it can be written like next code,
 *
 * while (1) {
 * 	if ( (t = button_handle())) {
 * 		if (chat < 15)
 * 			continue;
 * 		chat = 0;
 *
 * 		...
 * 	}
 */
