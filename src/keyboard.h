#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include <wayland-client.h>
#include <wayland-util.h>
#include <xkbcommon/xkbcommon.h>

#include "config.h"
#include "input.h"

struct keyboard {
  /* Keyboard objects */
  char *keymap_string;
  struct xkb_state *state;
  struct xkb_context *context;
  struct xkb_keymap *keymap;
  struct {
    uint32_t rate;
    uint32_t delay;
    uint32_t keycode;
    uint32_t next;
    bool active;
  } repeat;
  struct input_handler input_handler;
};

void keyboard_keymap(
  struct keyboard *keyboard,
  int32_t fd,
  uint32_t size);
void keyboard_key_held(
  struct keyboard *keyboard,
  uint32_t key);
void keyboard_key_pressed(
  struct keyboard *keyboard,
  uint32_t key);
void keyboard_modifiers(
  struct keyboard *keyboard,
  uint32_t serial,
  uint32_t mods_depressed,
  uint32_t mods_latched,
  uint32_t mods_locked,
  uint32_t group)
;
void keyboard_repeat_info(
  struct keyboard *keyboard,
  int32_t rate,
  int32_t delay)
;

struct keyboard keyboard_create(struct config *conf);

#endif /* KEYBOARD_H */
