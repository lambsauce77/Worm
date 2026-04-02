#ifndef INPUT_H
#define INPUT_H

#include "defs.h"

b8 input_key_press(SDL_Scancode sc);
b8 input_key_down(SDL_Scancode sc);

void input_update(void);
void input_poll(SDL_Event* e);

#endif