#include "game.h"
#include "entity.h"
#include "gfx.h"
#include "input.h"
#include "wall.h"

#define G_SCALE 9.18f
#define ROPE_SENS 2500.0f
#define ROPE_MIN_LEN 30.0f
#define ROPE_MAX_PIVOTS 16

typedef enum {
	PLAYER_STATE_IDLE,
	PLAYER_STATE_ROPE,
	PLAYER_STATE_WALK,
	PLAYER_STATE_FLY,
	PLAYER_STATE_COUNT
}PlayerState;
PlayerState player_state;

typedef struct {
	f32 angle;
	f32 angular_vel;
	f32 len;
	vec_t pivot;
}rope_t;

entity_t player = {
	.clr = { 255, 255, 255, 255 },
	.rect = { 0, 0, 50, 50}
};

rope_t rope_test = {
	.angle = 0.5f,
	.angular_vel = 0.0f,
	.len = 100.0f,
	.pivot = {WINDOW_W/2, WINDOW_H/2}
};

u32 wall_count = 2;
wall_t* walls = (wall_t[]){
	{ .clr = {0, 0, 255, 255}, .rect = {200, WINDOW_H / 2, 50, 200} },
	{.clr = {0, 0, 255, 255}, .rect = {WINDOW_W - 200, WINDOW_H / 2, 50, 200} }
};

static b8 aabb(entity_t* e, vec_t pos) {
	if (!e) {
		return FALSE;
	}

	vec_t epos = VEC2(pos.x - e->rect.w * 0.5f, pos.y - e->rect.h * 0.5f);

	for (u32 i = 0; i < wall_count; i++) {
		SDL_FRect* wrect = &walls[i].rect;

		if (epos.x < wrect->x + wrect->w &&
			epos.x + e->rect.w > wrect->x &&
			epos.y < wrect->y + wrect->h &&
			epos.y + e->rect.h > wrect->y) {
			return TRUE;
		}
	}

	return FALSE;
}

static b8 line_in_seg(vec_t l0, vec_t l1, vec_t s0, vec_t s1, vec_t* out) {
	f32 x0 = l0.x;
	f32 y0 = l0.y;
	f32 x1 = l1.x;
	f32 y1 = l1.y;

	f32 x2 = s0.x;
	f32 y2 = s0.y;
	f32 x3 = s1.x;
	f32 y3 = s1.y;

	f32 d = (x0 - x1) * (y2 - y3) - (y0 - y1) * (x2 - x3);
	if (fabsf(d) < EPS) {
		return FALSE;
	}

	f32 px = ((x0 * y1 - y0 * x1) * (x2 - x3) - (x0 - x1) * (x2 * y3 - y2 * x3)) / d;
	f32 py = ((x0 * y1 - y0 * x1) * (y2 - y3) - (y0 - y1) * (x2 * y3 - y2 * x3)) / d;

	if ((px < fmin(x0, x1) || px > fmax(x0, x1)) ||
		(px < fmin(x2, x3) || px > fmax(x2, x3)) ||
		(py < fmin(y0, y1) || py > fmax(y0, y1)) ||
		(py < fmin(y2, y3) || py > fmax(y2, y3))) {
		return FALSE;
	}

	if (out) {
		out->x = px;
		out->y = py;
	}

	return TRUE;
}

static b8 rope_intersects(rope_t* r, vec_t pos, vec_t* out) {
	for (u32 i = 0; i < wall_count; i++) {
		wall_t* w = &walls[i];

		vec_t p0 = VEC2(w->rect.x, w->rect.y);
		vec_t p1 = VEC2(w->rect.x + w->rect.w, w->rect.y);
		vec_t p2 = VEC2(w->rect.x, w->rect.y + w->rect.h);
		vec_t p3 = VEC2(w->rect.x + w->rect.w, w->rect.y + w->rect.h);

		vec_t sides[4][2] = { {p0, p1}, {p1, p3}, {p3, p2}, {p2, p0} };

		for (u32 j = 0; j < 4; j++) {
			vec_t point;

			if (line_in_seg(pos, r->pivot, sides[j][0], sides[j][1], &point)) {
				if (out) {
					*out = point;
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

static vec_t rope_update(rope_t* r, vec_t dir, f32 dt) {
	if (!r) {
		return VEC_NULL;
	}

	const f32 damping = 2.0f;
	const f32 gravity = G_SCALE * 300.0f;	
	const f32 len_mult = 0.3f;
	const f32 bounce_str = 2.0f;

	f32 len = r->len;
	f32 vel = r->angular_vel;
	f32 ang = r->angle;

	len += dir.y * len_mult * dt;

	if (len <= 0.0f) {
		len = ROPE_MIN_LEN;
	}

	f32 alpha = -(gravity / len) * sin(ang) + (dir.x / len) - (damping * vel);

	vel += alpha * dt;
	ang += vel * dt;

	f32 x = r->pivot.x + len * sin(ang);
	f32 y = r->pivot.y + len * cos(ang);

	vec_t pos = VEC2(x, y);
	vec_t hit;

	const f32 min_dist = 10.0f;

	if (rope_intersects(r, pos, &hit)) {
		f32 dist = VEC_DIST(hit, r->pivot);
		if (dist > min_dist) {
			vec_t tip = pos;

			r->pivot = hit;

			f32 dx = tip.x - r->pivot.x;
			f32 dy = tip.y - r->pivot.y;

			len = sqrtf(dx * dx + dy * dy);
			ang = atan2f(dx, dy);
		}
	}

	if (aabb(&player, pos)) {
		vel = r->angular_vel * -bounce_str;
		f32 sign = (vel > 0) - (vel < 0);
		len = r->len + sign;
		ang = r->angle;
	}

	r->len = len;
	r->angular_vel = vel;
	r->angle = ang;

	return pos;
}

static void rope_fire(rope_t* r, vec_t dest) {
	if (!r) {
		return;
	}

	vec_t delta = VEC_DELTA(dest, ENTITY_CENTER(player));
	f32 tan_vel = player.velocity.x * cos(r->angle) - player.velocity.y * sin(r->angle);

	r->len = VEC_MAG(delta);
	r->pivot = dest;
	r->angle = 0.0f;
	r->angular_vel = tan_vel;
	player_state = PLAYER_STATE_ROPE;
}

void game_start(void) {
	vec_t center = VEC2(WINDOW_W / 2, WINDOW_H / 2);
	ENTITY_MOVE(&player, center);
	rope_fire(&rope_test, center);
}

void game_update(f32 dt) {
	vec_t delta = VEC_ZERO;

	vec_t old_pos = ENTITY_CENTER(player);
	vec_t new_pos = old_pos;

	switch (player_state) {

		case PLAYER_STATE_ROPE:
			if (input_key_down(SDL_SCANCODE_UP)) {
				delta.y = -1.0f;
			}
			if (input_key_down(SDL_SCANCODE_DOWN)) {
				delta.y = 1.0f;
			}
			if (input_key_down(SDL_SCANCODE_RIGHT)) {
				delta.x = 1.0f;
			}
			if (input_key_down(SDL_SCANCODE_LEFT)) {
				delta.x = -1.0f;
			}

			VEC_SCALE(&delta, ROPE_SENS);
			new_pos = rope_update(&rope_test, delta, dt);
			break;

		case PLAYER_STATE_FLY:
			if (aabb(&player, new_pos)) {
				VEC_SCALE(&player.velocity, -0.9f);
			}
			player.velocity.y += G_SCALE * dt;
			VEC_ADD(&new_pos, player.velocity);
			break;
	}

	player.velocity = VEC_DELTA(new_pos, old_pos);
	ENTITY_MOVE(&player, new_pos);

	if (input_key_press(SDL_SCANCODE_SPACE)) {
		if (player_state == PLAYER_STATE_ROPE) {
			player_state = PLAYER_STATE_FLY;
		}
		else {
			rope_fire(&rope_test, VEC2(new_pos.x, new_pos.y - 150.0f));
		}
	}
}

void game_render(void) {
	for (u32 i = 0; i < wall_count; i++) {
		gfx_draw_wall(&walls[i]);
	}

	if (player_state == PLAYER_STATE_ROPE) {
		gfx_draw_line(ENTITY_CENTER(player), rope_test.pivot);
	}

	gfx_draw_entity(&player);
}