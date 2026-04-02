#ifndef ENTITY_H
#define ENTITY_H

#include "defs.h"

#define ENTITY_CENTER(e) (VEC2( ((e).rect.x + ((e).rect.w * 0.5f)), ((e).rect.y + ((e).rect.h * 0.5f)) ))
#define ENTITY_MOVE(e, pos) \
do{ \
	(e)->rect.x = (pos).x - ((e)->rect.w * 0.5f); \
	(e)->rect.y = (pos).y - ((e)->rect.h * 0.5f); \
}while(0)

typedef struct {
	SDL_Color clr;
	SDL_FRect rect;
	vec_t velocity;
}entity_t;

#endif