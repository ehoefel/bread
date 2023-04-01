#ifndef BREAD_H
#define BREAD_H

#include "config.h"
#include "keyboard.h"
#include "wayland.h"
#include "window.h"

struct bread {
  char *name;
  struct wayland wayland;
  struct keyboard keyboard;
  struct window *window;
};

struct bread bread_create(struct config *conf);

#endif /* BREAD_H */

