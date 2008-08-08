#include "tslib.h"

struct ts_sample phy2vir_pos(struct ts_sample p, unsigned int my_location)
{
	struct ts_sample v;
	switch (my_location) {
		case 0:
			v = p;
			break;
		case 1:
			v = p;
			v.x = p.x + 320;
			break;
		case 2:
			v = p;
			v.y = p.y + 240;
			break;
		case 3:
			v = p;
			v.x = p.x + 320;
			v.y = p.y + 240;
			break;
		default:
			break;
	}

	return v;
}
