#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <stdint.h>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>
#include "fractional-scale-v1.h"
#include "wlr-layer-shell-unstable-v1.h"

#include "config.h"
#include "wayland.h"

struct window {

};

struct window window_create(struct config *conf);

#endif /* WINDOW_H */
