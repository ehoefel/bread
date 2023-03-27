#ifndef BREAD_H
#define BREAD_H

#include "config.h"
#include "keyboard.h"
#include "window.h"

struct bread {
  struct wayland wayland;
  struct keyboard keyboard;
};

struct bread bread_create(struct config *conf);

#endif /* BREAD_H */

