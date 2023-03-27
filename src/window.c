#include "window.h"
#include "config.h"
#include "wayland.h"

void window_apply_config(struct window *window, struct config *conf)
{
  return;
}

struct window window_create(struct config *conf)
{
  struct window window = {
  };

  window_apply_config(&window, conf);

  return window;
}
