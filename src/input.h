#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <xkbcommon/xkbcommon.h>
#include "state.h"
#include "symbol.h"

struct input_handler {
  struct state *state;
  struct keyboard *keyboard;
};

struct input {
  bool mod_ctrl;
  bool mod_alt;
  bool mod_super;
  struct symbol symbol;
};

void input_on_keypress(struct input_handler *handler, struct input *input);
//void input_refresh_results(struct tofi *tofi);

#endif /* INPUT_H */
