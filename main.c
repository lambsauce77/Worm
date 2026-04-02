#include "defs.h"
#include "game.h"
#include "gfx.h"
#include "input.h"

#define EXIT(code, renderer, window) \
do{ \
	if(renderer){SDL_DestroyRenderer(renderer);} \
	if(window){SDL_DestroyWindow(window);} \
	SDL_Quit(); \
	return(code); \
}while(0)

i32 exit_code = 0;
b8 running = FALSE;

void quit(i32 code) {
	exit_code = code;
	running = FALSE;
}

int main(void) {

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		EXIT(-1, NULL, NULL);
	}

	SDL_Window* window = SDL_CreateWindow(
		"Worm",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_W,
		WINDOW_H,
		0);

	if (!window) {
		EXIT(-1, NULL, NULL);
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, SDL_RENDERER_ACCELERATED, 0);

	if (!renderer) {
		EXIT(-1, NULL, window);
	}

	if (!gfx_init(renderer)) {
		EXIT(-1, renderer, window);
	}

	u64 last_time = SDL_GetTicks64();
	SDL_Event event;
	running = TRUE;

	game_start();

	while (running) {
		input_update();

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit(0);
			}
			input_poll(&event);
		}

		u64 cur_time = SDL_GetTicks64();
		f32 dt = (cur_time - last_time) / 1000.0f;
		last_time = cur_time;

		game_update(dt);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		game_render();

		SDL_RenderPresent(renderer);
	}


	EXIT(exit_code, renderer, window);
}