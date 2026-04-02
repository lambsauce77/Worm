#ifndef GFX_H
#define GFX_H

#include "defs.h"
#include "entity.h"
#include "wall.h"

b8 gfx_init(SDL_Renderer* r);
void gfx_draw_entity(entity_t* e);
void gfx_draw_wall(wall_t* w);
void gfx_draw_line(vec_t v0, vec_t v1);

#endif