#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdint.h>
#include <xkbcommon/xkbcommon.h>

struct symbol {
  uint32_t unicode_char;
  uint32_t key;
  xkb_keycode_t keycode;
  xkb_keysym_t xkb_sym;
};

struct symbol symbol_create(struct xkb_state *state, uint32_t key);

#endif /* SYMBOL_H */
