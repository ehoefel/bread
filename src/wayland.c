
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include <wayland-util.h>
#include <stdbool.h>
#include <stdint.h>

#include "wayland.h"
#include "config.h"
#include "keyboard.h"
#include "log.h"
#include "mathutils.h"
#include "surface.h"
#include "viewporter.h"
#include "xmalloc.h"

struct wayland wayland_create(struct config *conf)
{
  log_enter_context("wayland_create");
  struct wayland wayland;

  log_leave_context();
  return wayland;
}

