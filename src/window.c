#include <stdbool.h>
#include <stdint.h>

#include <wayland-client.h>
#include <wayland-util.h>
#include "window.h"
#include "bread.h"
#include "config.h"
#include "log.h"
#include "mathutils.h"
#include "scale.h"
#include "wayland.h"

void window_apply_config(struct window *window, struct config *conf)
{
  log_enter_context("window_apply_config");
  log_leave_context();
  return;
}

struct window *window_create(struct config *conf)
{
  log_enter_context("window_create");
  struct window *window = (struct window*)malloc(sizeof(struct window));

  window_apply_config(window, conf);

  log_leave_context();
  return window;
}
