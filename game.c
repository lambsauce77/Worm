#include "game.h"
#include "entity.h"
#include "gfx.h"
#include "input.h"
#include "wall.h"

#define G_SCALE 9.18f
#define ROPE_SENS 2500.0f
#define ROPE_MIN_LEN 30.0f

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

static vec_t rope_update(rope_t* r, vec_t dir, f32 dt) {
	if (!r) {
		return VEC_ZERO;
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

static void rope_fire(vec_t dest) {
	vec_t delta = VEC_DELTA(dest, ENTITY_CENTER(player));
	f32 tan_vel = player.velocity.x * cos(rope_test.angle) - player.velocity.y * sin(rope_test.angle);
	rope_test.len = VEC_MAG(delta);
	rope_test.pivot = dest;
	rope_test.angle = 0.0f;
	rope_test.angular_vel = tan_vel;
	player_state = PLAYER_STATE_ROPE;
}

void game_start(void) {
	vec_t center = VEC2(WINDOW_W / 2, WINDOW_H / 2);
	ENTITY_MOVE(&player, center);
	rope_fire(center);
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
			rope_fire(VEC2(new_pos.x, new_pos.y - 150.0f));
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