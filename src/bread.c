#include <stdio.h>
#include <stdlib.h>

#include "bread.h"
#include "config.h"
#include "log.h"
#include "setup.h"

void bread_apply_config(struct bread *bread, struct config *conf)
{
  return;
}

struct bread bread_create(struct config *conf)
{
  log_enter_context("bread_create");

  struct bread bread = {
    .name = "bread",
    .keyboard = keyboard_create(conf),
    .wayland = wayland_create(conf),
    .window = window_create(conf)
  };

  bread_apply_config(&bread, conf);

  setup_bread(&bread);

  log_leave_context();
  return bread;
}
