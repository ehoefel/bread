#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <stdint.h>
#include <xkbcommon/xkbcommon.h>
#include "fractional-scale-v1.h"
#include "wlr-layer-shell-unstable-v1.h"

#include "bread.h"
#include "config.h"
#include "surface.h"

#define MAX_OUTPUT_NAME_LEN 256

#define ANCHOR_TOP_LEFT (\
    ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT \
    )
#define ANCHOR_TOP (\
    ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT \
    )
#define ANCHOR_TOP_RIGHT (\
    ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT \
    )
#define ANCHOR_RIGHT (\
    ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM \
    )
#define ANCHOR_BOTTOM_RIGHT (\
    ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT \
    )
#define ANCHOR_BOTTOM (\
    ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT \
    )
#define ANCHOR_BOTTOM_LEFT (\
    ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT \
    )
#define ANCHOR_LEFT (\
    ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM \
    )
#define ANCHOR_CENTER (\
    ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT \
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT \
    )


struct window {
  char target_output_name[MAX_OUTPUT_NAME_LEN];
  struct surface surface;
  struct wp_viewport *viewport;
  struct zwlr_layer_surface_v1 *zwlr_layer_surface;
  int32_t output_width;
  int32_t output_height;
  uint32_t width;
  uint32_t height;
  uint32_t scale;
  uint32_t fractional_scale;
  int32_t transform;
  int32_t exclusive_zone;
  int32_t margin_top;
  int32_t margin_bottom;
  int32_t margin_left;
  int32_t margin_right;
  int32_t padding_top;
  int32_t padding_bottom;
  int32_t padding_left;
  int32_t padding_right;
  bool width_is_percent;
  bool height_is_percent;
  bool exclusive_zone_is_percent;
  bool margin_top_is_percent;
  bool margin_bottom_is_percent;
  bool margin_left_is_percent;
  bool margin_right_is_percent;
  bool padding_top_is_percent;
  bool padding_bottom_is_percent;
  bool padding_left_is_percent;
  bool padding_right_is_percent;
  uint32_t anchor;
  bool use_scale;
};

struct window *window_create(struct config *conf);

#endif /* WINDOW_H */
