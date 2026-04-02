#include "game.h"
#include "entity.h"
#include "gfx.h"
#include "input.h"

#define G_SCALE 9.18f
#define ROPE_SENS 2000.0f
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

static vec_t rope_update(rope_t* r, vec_t dir, f32 dt) {
	if (!r) {
		return VEC_ZERO;
	}

	const f32 damping = 2.0f;
	const f32 gravity = G_SCALE * 100.0f;	
	const f32 len_mult = 0.5f;

	r->len += dir.y * len_mult * dt;

	if (r->len <= 0.0f) {
		r->len = ROPE_MIN_LEN;
	}

	f32 alpha = -(gravity / r->len) * sin(r->angle) + (dir.x / r->len) - (damping * r->angular_vel);

	r->angular_vel += alpha * dt;
	r->angle += r->angular_vel * dt;

	f32 x = r->pivot.x + r->len * sin(r->angle);
	f32 y = r->pivot.y + r->len * cos(r->angle);

	return VEC2(x, y);
}

static void rope_fire(vec_t dest) {
	player_state = PLAYER_STATE_ROPE;
	rope_test.pivot = dest;
	vec_t delta = VEC_DELTA(dest, ENTITY_CENTER(player));
	rope_test.len = VEC_MAG(delta);
	rope_test.angle = 0.0f;
	f32 tan_vel = player.velocity.x * cos(rope_test.angle) - player.velocity.y * sin(rope_test.angle);
	rope_test.angular_vel = tan_vel;
}

void game_start(void) {
	rope_fire(VEC2(WINDOW_W / 2, WINDOW_H / 2));
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
	if (player_state == PLAYER_STATE_ROPE) {
		gfx_draw_line(ENTITY_CENTER(player), rope_test.pivot);
	}

	gfx_draw_entity(&player);
}