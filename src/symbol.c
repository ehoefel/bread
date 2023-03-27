
#include <stdint.h>
#include <xkbcommon/xkbcommon.h>
#include "symbol.h"

struct symbol symbol_create(struct xkb_state *state, uint32_t key)
{
  struct symbol symbol = {
    .key = key,
    .keycode = key + 8,
    .xkb_sym = xkb_state_key_get_one_sym(state, symbol.keycode),
    .unicode_char = xkb_state_key_get_utf32(state, symbol.keycode)
  };

  return symbol;

}
