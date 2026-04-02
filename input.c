#include "input.h"

b8 cur_keys[SDL_NUM_SCANCODES];
b8 prev_keys[SDL_NUM_SCANCODES];

b8 input_key_press(SDL_Scancode sc){
	return cur_keys[sc] && !prev_keys[sc];
}

b8 input_key_down(SDL_Scancode sc){
	return cur_keys[sc];
}

void input_update(void){
	memcpy(prev_keys, cur_keys, sizeof(cur_keys));
}

void input_poll(SDL_Event* e){
	switch (e->type) {

		case SDL_KEYDOWN:
			cur_keys[e->key.keysym.scancode] = TRUE;
			break;

		case SDL_KEYUP:
			cur_keys[e->key.keysym.scancode] = FALSE;
			break;
	}
}