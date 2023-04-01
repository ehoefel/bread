
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <wayland-client.h>
#include <wayland-util.h>
#include <xkbcommon/xkbcommon.h>
#include "keyboard.h"
#include "config.h"
#include "log.h"
#include "symbol.h"
#include "sysutils.h"

void keyboard_keymap(
  struct keyboard *keyboard,
  int32_t fd,
  uint32_t size)
{
  log_enter_context("keyboard_keymap");
  char *map_shm = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  assert(map_shm != MAP_FAILED);
  struct xkb_keymap *xkb_keymap = xkb_keymap_new_from_string(
    keyboard->context,
    map_shm,
    XKB_KEYMAP_FORMAT_TEXT_V1,
    XKB_KEYMAP_COMPILE_NO_FLAGS);
  munmap(map_shm, size);
  close(fd);

  struct xkb_state *xkb_state = xkb_state_new(xkb_keymap);
  xkb_keymap_unref(keyboard->keymap);
  xkb_state_unref(keyboard->state);
  keyboard->keymap = xkb_keymap;
  keyboard->state = xkb_state;
  //munmap(map_shm, size);
  //close(fd);
  log_leave_context();
}

void keyboard_key_held(struct keyboard *keyboard, uint32_t key)
{
  log_enter_context("keyboard_key_held");
  struct symbol symbol = symbol_create(keyboard->state, key);

  if (symbol.keycode == keyboard->repeat.keycode) {
    keyboard->repeat.active = false;
  } else {
    keyboard->repeat.next = gettime_ms() + keyboard->repeat.delay;
  }
  log_leave_context();
}

void keyboard_key_pressed(struct keyboard *keyboard, uint32_t key)
{
  log_enter_context("keyboard_key_pressed");
  struct input input = {
    .symbol = symbol_create(keyboard->state, key),
    .mod_ctrl = xkb_state_mod_name_is_active(
      keyboard->state,
      XKB_MOD_NAME_CTRL,
      XKB_STATE_MODS_EFFECTIVE),
  };

  if (xkb_keymap_key_repeats(keyboard->keymap, input.symbol.keycode)
      && keyboard->repeat.rate != 0) {
    keyboard->repeat.active = true;
    keyboard->repeat.keycode = input.symbol.keycode;
    keyboard->repeat.next = gettime_ms() + keyboard->repeat.delay;
  }

  if (keyboard->state == NULL) {
    log_leave_context();
    return;
  }

  input_on_keypress(&keyboard->input_handler, &input);
  log_leave_context();
}

void keyboard_modifiers(
  struct keyboard *keyboard,
  uint32_t serial,
  uint32_t mods_depressed,
  uint32_t mods_latched,
  uint32_t mods_locked,
  uint32_t group)
{
  log_enter_context("keyboard_modifiers");
  if (keyboard->state == NULL) {
    log_debug("keyboard->state is NULL");
    log_leave_context();
    return;
  }
  xkb_state_update_mask(
    keyboard->state,
    mods_depressed,
    mods_latched,
    mods_locked,
    0,
    0,
    group);
  log_leave_context();
}

void keyboard_repeat_info(
  struct keyboard *keyboard,
  int32_t rate,
  int32_t delay)
{
  log_enter_context("keyboard_repeat_info");
  keyboard->repeat.rate = rate;
  keyboard->repeat.delay = delay;
  log_leave_context();
}

struct keyboard keyboard_create(struct config *conf)
{
  log_enter_context("keyboard_create");
  struct keyboard keyboard;
  keyboard.context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  if (keyboard.context == NULL) {
    log_enter_context("keyboard.context is NULL");
    exit(EXIT_FAILURE);
  }
  log_leave_context();
  return keyboard;
}
