#include "gfx.h"

SDL_Renderer* renderer = NULL;

b8 gfx_init(SDL_Renderer* r) {
	if (!r) {
		return FALSE;
	}

	renderer = r;
	return TRUE;
}

void gfx_draw_entity(entity_t* e) {
	if (!renderer || !e) {
		return;
	}

	SDL_SetRenderDrawColor(renderer, e->clr.r, e->clr.g, e->clr.b, e->clr.a);
	SDL_RenderFillRectF(renderer, &e->rect);
}

void gfx_draw_wall(wall_t* w) {
	if (!renderer || !w) {
		return;
	}

	SDL_SetRenderDrawColor(renderer, w->clr.r, w->clr.g, w->clr.b, w->clr.a);
	SDL_RenderFillRectF(renderer, &w->rect);
}

void gfx_draw_line(vec_t v0, vec_t v1) {
	if (!renderer) {
		return;
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawLineF(renderer, v0.x, v0.y, v1.x, v1.y);
}