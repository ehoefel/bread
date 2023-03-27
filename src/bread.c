#include <stdio.h>
#include <stdlib.h>

#include "bread.h"
#include "config.h"

void bread_apply_config(struct bread *bread, struct config *conf)
{
  return;
}

struct bread bread_create(struct config *conf)
{
  struct bread bread = {
    .wayland = wayland_create(conf)
  };
  bread_apply_config(&bread, conf);
  return bread;
}
