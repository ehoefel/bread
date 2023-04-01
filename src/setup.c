#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include <wayland-util.h>

#include "setup.h"
#include "bread.h"
#include "keyboard.h"
#include "log.h"
#include "mathutils.h"
#include "scale.h"
#include "surface.h"
#include "viewporter.h"
#include "wayland.h"
#include "window.h"
#include "xmalloc.h"

static void zwlr_layer_surface_configure(
  void *data,
  struct zwlr_layer_surface_v1 *zwlr_layer_surface,
  uint32_t serial,
  uint32_t width,
  uint32_t height)
{
  log_enter_context("zwlr_layer_surface_configure");
  if (width == 0 || height == 0) {
    /* Compositor is deferring to us, so don't do anything. */
    log_debug("Layer surface configure with no width or height.\n");
    return;
  }
  log_debug("Layer surface configure, %u x %u.\n", width, height);
  struct bread *bread = data;
  struct window *window = bread->window;

  /*
   * Resize the main window.
   * We want actual pixel width / height, so we have to scale the
   * values provided by Wayland.
   */
  if (window->fractional_scale != 0) {
    window->surface.width = scale_apply(width, window->fractional_scale);
    window->surface.height = scale_apply(height, window->fractional_scale);
  } else {
    window->surface.width  = width  * window->scale;
    window->surface.height = height * window->scale;
  }

  zwlr_layer_surface_v1_ack_configure(
    window->zwlr_layer_surface,
    serial);
  log_leave_context();
}

static void zwlr_layer_surface_close(
  void *data,
  struct zwlr_layer_surface_v1 *zwlr_layer_surface)
{
  log_enter_context("zwlr_layer_surface_close");
  struct bread *bread = data;
  //tofi->closed = true;
  log_debug("Layer surface close.\n");
  log_leave_context();
}

static const struct zwlr_layer_surface_v1_listener zwlr_layer_surface_listener = {
  .configure = zwlr_layer_surface_configure,
  .closed = zwlr_layer_surface_close
};

uint32_t fixup_percentage(uint32_t value, uint32_t base, bool is_percent);

uint32_t fixup_percentage(uint32_t value, uint32_t base, bool is_percent)
{
  if (is_percent) {
    return value * base / 100;
  }
  return value;
}

static void wl_keyboard_keymap(
  void *data,
  struct wl_keyboard *keyboard,
  uint32_t format,
  int32_t fd,
  uint32_t size)
{
  log_enter_context("wl_keyboard_keymap");
  assert(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);

  struct bread *bread = data;
  keyboard_keymap(&bread->keyboard, fd, size);
  log_leave_context();
}

static void wl_keyboard_enter(
  void *data,
  struct wl_keyboard *keyboard,
  uint32_t serial,
  struct wl_surface *surface,
  struct wl_array *keys)
{
  log_enter_context("wl_keyboard_enter");
  /* Deliberately left blank */
  log_leave_context();
}

static void wl_keyboard_leave(
  void *data,
  struct wl_keyboard *keyboard,
  uint32_t serial,
  struct wl_surface *surface)
{
  log_enter_context("wl_keyboard_leave");
  /* Deliberately left blank */
  log_leave_context();
}

static void wl_keyboard_key(
  void *data,
  struct wl_keyboard *keyboard,
  uint32_t serial,
  uint32_t time,
  uint32_t key,
  uint32_t state)
{
  log_enter_context("wl_keyboard_key");
  struct bread *bread = data;

  /*
   * If this wasn't a keypress (i.e. was a key release), just update key
   * repeat info and return.
   */
  if (state != WL_KEYBOARD_KEY_STATE_PRESSED) {
    keyboard_key_held(&bread->keyboard, key);
    log_leave_context();
    return;
  }

  keyboard_key_pressed(&bread->keyboard, key);
  log_leave_context();
}

static void wl_keyboard_modifiers(
  void *data,
  struct wl_keyboard *keyboard,
  uint32_t serial,
  uint32_t mods_depressed,
  uint32_t mods_latched,
  uint32_t mods_locked,
  uint32_t group)
{
  log_enter_context("wl_keyboard_modifiers");
  struct bread *bread = data;
  keyboard_modifiers(
    &bread->keyboard,
    serial,
    mods_depressed,
    mods_latched,
    mods_locked,
    group);
  log_leave_context();
}

static void wl_keyboard_repeat_info(
  void *data,
  struct wl_keyboard *keyboard,
  int32_t rate,
  int32_t delay)
{
  log_enter_context("wl_keyboard_repeat_info");
  struct bread *bread = data;
  keyboard_repeat_info(&bread->keyboard, rate, delay);
  log_leave_context();
}

static void output_geometry(
  void *data,
  struct wl_output *output,
  int32_t x,
  int32_t y,
  int32_t physical_width,
  int32_t physical_height,
  int32_t subpixel,
  const char *make,
  const char *model,
  int32_t transform)
{
  log_enter_context("output_geometry");
  struct bread *bread = data;
  struct wayland *wayland = &bread->wayland;
  struct output_list_element *el;
  wl_list_for_each(el, &wayland->global.output_list, link) {
    if (el->output == output) {
      el->transform = transform;
    }
  }
  log_leave_context();
}

static void output_mode(
  void *data,
  struct wl_output *output,
  uint32_t flags,
  int32_t width,
  int32_t height,
  int32_t refresh)
{
  log_enter_context("output_mode");
  struct bread *bread = data;
  struct wayland *wayland = &bread->wayland;
  struct output_list_element *el;
  wl_list_for_each(el, &wayland->global.output_list, link) {
    if (el->output == output) {
      if (flags & WL_OUTPUT_MODE_CURRENT) {
        el->width = width;
        el->height = height;
      }
    }
  }
  log_leave_context();
}

static void output_scale(
  void *data,
  struct wl_output *output,
  int32_t factor)
{
  log_enter_context("output_scale");
  struct bread *bread = data;
  struct wayland *wayland = &bread->wayland;
  struct output_list_element *el;
  wl_list_for_each(el, &wayland->global.output_list, link) {
    if (el->output == output) {
      el->scale = factor;
    }
  }
  log_leave_context();
}

static void output_name(
    void *data,
    struct wl_output *output,
    const char *name)
{
  log_enter_context("output_name");
  struct bread *bread = data;
  struct wayland *wayland = &bread->wayland;
  struct output_list_element *el;
  wl_list_for_each(el, &wayland->global.output_list, link) {
    if (el->output == output) {
      el->name = xstrdup(name);
    }
  }
  log_leave_context();
}

static void output_description(
    void *data,
    struct wl_output *wl_output,
    const char *description)
{
  log_enter_context("output_description");
  /* Deliberately left blank */
  log_leave_context();
}

static void output_done(void *data, struct wl_output *output)
{
  log_enter_context("output_done");
  log_leave_context();
}

static const struct wl_output_listener output_listener = {
  .geometry = output_geometry,
  .mode = output_mode,
  .done = output_done,
  .scale = output_scale,
#ifndef NO_WL_OUTPUT_NAME
  .name = output_name,
  .description = output_description,
#endif
};


static const struct wl_keyboard_listener keyboard_listener = {
  .keymap = wl_keyboard_keymap,
  .enter = wl_keyboard_enter,
  .leave = wl_keyboard_leave,
  .key = wl_keyboard_key,
  .modifiers = wl_keyboard_modifiers,
  .repeat_info = wl_keyboard_repeat_info,
};

static void pointer_enter(
  void *data,
  struct wl_pointer *pointer,
  uint32_t serial,
  struct wl_surface *surface,
  wl_fixed_t surface_x,
  wl_fixed_t surface_y)
{
  log_enter_context("pointer_enter");
  /*
  struct tofi *tofi = data;
  if (tofi->hide_cursor) {
    // Hide the cursor by setting its surface to NULL.
    wl_pointer_set_cursor(tofi->wl_pointer, serial, NULL, 0, 0);
  }
  */
  log_leave_context();
}

static void pointer_leave(
  void *data,
  struct wl_pointer *pointer,
  uint32_t serial,
  struct wl_surface *surface)
{
  log_enter_context("pointer_leave");
  /* Deliberately left blank */
  log_leave_context();
}

static void pointer_motion(
  void *data,
  struct wl_pointer *pointer,
  uint32_t time,
  wl_fixed_t surface_x,
  wl_fixed_t surface_y)
{
  log_enter_context("pointer_motion");
  /* Deliberately left blank */
  log_leave_context();
}

static void pointer_button(
  void *data,
  struct wl_pointer *pointer,
  uint32_t serial,
  uint32_t time,
  uint32_t button,
  enum wl_pointer_button_state state)
{
  log_enter_context("pointer_button");
  /* Deliberately left blank */
  log_leave_context();
}

static void pointer_axis(
  void *data,
  struct wl_pointer *pointer,
  uint32_t time,
  enum wl_pointer_axis axis,
  wl_fixed_t value)
{
  log_enter_context("pointer_axis");
  /* Deliberately left blank */
  log_leave_context();
}

static void pointer_frame(void *data, struct wl_pointer *pointer)
{
  log_enter_context("pointer_frame");
  /* Deliberately left blank */
  log_leave_context();
}

static void pointer_axis_source(
  void *data,
  struct wl_pointer *pointer,
  enum wl_pointer_axis_source axis_source)
{
  log_enter_context("pointer_axis_source");
  /* Deliberately left blank */
  log_leave_context();
}

static void pointer_axis_stop(
  void *data,
  struct wl_pointer *pointer,
  uint32_t time,
  enum wl_pointer_axis axis)
{
  log_enter_context("pointer_axis_stop");
  /* Deliberately left blank */
  log_leave_context();
}

static void pointer_axis_discrete(
  void *data,
  struct wl_pointer *pointer,
  enum wl_pointer_axis axis,
  int32_t discrete)
{
  log_enter_context("pointer_axis_discrete");
  /* Deliberately left blank */
  log_leave_context();
}

static const struct wl_pointer_listener pointer_listener = {
  .enter = pointer_enter,
  .leave = pointer_leave,
  .motion = pointer_motion,
  .button = pointer_button,
  .axis = pointer_axis,
  .frame = pointer_frame,
  .axis_source = pointer_axis_source,
  .axis_stop = pointer_axis_stop,
  .axis_discrete = pointer_axis_discrete
};

static void seat_capabilities(
  void *data,
  struct wl_seat *seat,
  uint32_t capabilities)
{
  log_enter_context("seat_capabilities");
  struct bread *bread = data;
  struct wayland *wayland = &bread->wayland;

  bool have_keyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;
  bool have_pointer = capabilities & WL_SEAT_CAPABILITY_POINTER;

  if (have_keyboard && wayland->keyboard == NULL) {
    wayland->keyboard = wl_seat_get_keyboard(wayland->global.seat);
    wl_keyboard_add_listener(
      wayland->keyboard,
      &keyboard_listener,
      bread);
  } else if (!have_keyboard && wayland->keyboard != NULL) {
    wl_keyboard_release(wayland->keyboard);
    wayland->keyboard = NULL;
  }

  if (have_pointer && wayland->pointer == NULL) {
    wayland->pointer = wl_seat_get_pointer(wayland->global.seat);
    wl_pointer_add_listener(
      wayland->pointer,
      &pointer_listener,
      bread);
  } else if (!have_pointer && wayland->pointer != NULL) {
    wl_pointer_release(wayland->pointer);
    wayland->pointer = NULL;
  }
  log_leave_context();
}

static void seat_name(void *data, struct wl_seat *seat, const char *name)
{
  log_enter_context("seat_name");
  /* Deliberately left blank */
  log_leave_context();
}

static const struct wl_seat_listener seat_listener = {
  .capabilities = seat_capabilities,
  .name = seat_name,
};

static void dummy_fractional_scale_preferred_scale(
    void *data,
    struct wp_fractional_scale_v1 *fractional_scale,
    uint32_t scale)
{
  struct bread *bread = data;
  bread->window->fractional_scale = scale;
}

static const struct wp_fractional_scale_v1_listener dummy_fractional_scale_listener = {
  .preferred_scale = dummy_fractional_scale_preferred_scale
};

static void registry_global(
  void *data,
  struct wl_registry *registry,
  uint32_t name,
  const char *interface,
  uint32_t version)
{
  log_enter_context("registry_global");
  struct bread *bread = data;
  struct wayland *wayland = &bread->wayland;
  if (!strcmp(interface, wl_compositor_interface.name)) {
    log_debug("registering compositor interface");
    wayland->global.compositor = wl_registry_bind(
      registry,
      name,
      &wl_compositor_interface,
      4);
  } else if (!strcmp(interface, wl_seat_interface.name)) {
    log_debug("registering seat interface");
    wayland->global.seat = wl_registry_bind(
      registry,
      name,
      &wl_seat_interface,
      7);
    wl_seat_add_listener(
      wayland->global.seat,
      &seat_listener,
      bread);
  } else if (!strcmp(interface, wl_output_interface.name)) {
    log_debug("registering output interface");
    struct output_list_element *el = xmalloc(sizeof(*el));
    if (version < 4) {
      el->name = xstrdup("");
    } else {
      version = 4;
    }
    el->output = wl_registry_bind(
      registry,
      name,
      &wl_output_interface,
      version);
    wl_output_add_listener(
      el->output,
      &output_listener,
      bread);
    wl_list_insert(&wayland->global.output_list, &el->link);
  } else if (!strcmp(interface, wl_shm_interface.name)) {
    log_debug("registering shm interface");
    wayland->global.shm = wl_registry_bind(
      registry,
      name,
      &wl_shm_interface,
      1);
  } else if (!strcmp(interface, wl_data_device_manager_interface.name)) {
    log_debug("registering data device manager interface");
    wayland->global.data_device_manager = wl_registry_bind(
      registry,
      name,
      &wl_data_device_manager_interface,
      3);
  } else if (!strcmp(interface, zwlr_layer_shell_v1_interface.name)) {
    log_debug("registering zwlr layer shell interface");
    if (version < 3) {
    } else {
      version = 3;
    }
    wayland->global.zwlr_layer_shell = wl_registry_bind(
      registry,
      name,
      &zwlr_layer_shell_v1_interface,
      version);
  } else if (!strcmp(interface, wp_viewporter_interface.name)) {
    log_debug("registering viewporter interface");
    wayland->global.viewporter = wl_registry_bind(
      registry,
      name,
      &wp_viewporter_interface,
      1);
  } else
    if (!strcmp(interface, wp_fractional_scale_manager_v1_interface.name)) {
    log_debug("registering fractional scale manager interface");
    wayland->global.fractional_scale_manager = wl_registry_bind(
      registry,
      name,
      &wp_fractional_scale_manager_v1_interface,
      1);
  }
  log_leave_context();
}

static void registry_global_remove(
  void *data,
  struct wl_registry *registry,
  uint32_t name)
{
  log_enter_context("registry_global_remove");
  /* Deliberately left blank */
  log_leave_context();
}

static const struct wl_registry_listener registry_listener = {
  .global = registry_global,
  .global_remove = registry_global_remove,
};


static void dummy_surface_enter(
    void *data,
    struct wl_surface *surface,
    struct wl_output *output)
{
  log_enter_context("dummy_surface_enter");

  struct bread *bread = data;
  struct wayland *wayland = bread>wayland;
  struct output_list_element *el;
  wl_list_for_each(el, &wayland->global.output_list, link) {
    if (el->output == output) {
      wayland->global.default_output = el;
      break;
    }
  }

  log_leave_context();
}

static void dummy_surface_leave(
    void *data,
    struct wl_surface *wl_surface,
    struct wl_output *output)
{
  log_enter_context("dummy_surface_leave");
  /* Deliberately left blank */
  log_leave_context();
}

static const struct wl_surface_listener dummy_surface_listener = {
  .enter = dummy_surface_enter,
  .leave = dummy_surface_leave
};

/*
 * These "dummy_*" functions are callbacks just for the dummy surface used to
 * select the default output if there's more than one.
 */
static void dummy_layer_surface_configure(
  void *data,
  struct zwlr_layer_surface_v1 *zwlr_layer_surface,
  uint32_t serial,
  uint32_t width,
  uint32_t height)
{
  zwlr_layer_surface_v1_ack_configure(
    zwlr_layer_surface,
    serial);
}

static void dummy_layer_surface_close(
  void *data,
  struct zwlr_layer_surface_v1 *zwlr_layer_surface)
{
  /* Deliberately left blank */
}


static void surface_enter(
  void *data,
  struct wl_surface *wl_surface,
  struct wl_output *wl_output)
{
  log_debug("Surface entered output.");
}

static void surface_leave(
  void *data,
  struct wl_surface *wl_surface,
  struct wl_output *wl_output)
{
  /* Deliberately left blank */
}

static const struct wl_surface_listener wl_surface_listener = {
  .enter = surface_enter,
  .leave = surface_leave
};

static const struct zwlr_layer_surface_v1_listener
dummy_layer_surface_listener = {
  .configure = dummy_layer_surface_configure,
  .closed = dummy_layer_surface_close
};

struct wl_surface *create_surface(struct bread *bread)
{
  log_enter_context("create_surface");
  struct wayland *wayland = &bread->wayland;
  struct wl_compositor *compositor = wayland->global.compositor;
  struct wl_surface *surface = wl_compositor_create_surface(compositor);
  wl_surface_add_listener(
    surface,
    &wl_surface_listener,
    bread);
  log_leave_context();
  return surface;
}

void setup_wayland_init(struct bread *bread)
{
  log_enter_context("wayland_init");
  struct wayland *wayland = &bread->wayland;

  log_debug("wayland init globals");
  struct wl_globals *global = &wayland->global;
  wl_list_init(&global->output_list);
  global->display = wl_display_connect(NULL);

  if (global->display == NULL) {
    exit(EXIT_FAILURE);
  }

  global->registry = wl_display_get_registry(global->display);

  log_debug("wayland registry add listener");
  wl_registry_add_listener(
    wayland->global.registry,
    &registry_listener,
    bread);

  log_debug("first roundtrip");
  wl_display_roundtrip(wayland->global.display);
  log_debug("second roundtrip");
  wl_display_roundtrip(wayland->global.display);

  log_leave_context();
}

void setup_create_window_surface(struct bread *bread)
{
  struct wayland *wayland = &bread->wayland;
  struct window *window = bread->window;
  window->surface.wl_surface = create_surface(bread);
  if (window->width == 0 || window->height == 0) {
    /*
     * Workaround for compatibility with legacy behaviour.
     *
     * Before the fractional_scale protocol was released, there was
     * no way for a client to know whether a fractional scale
     * factor had been set, meaning percentage-based dimensions
     * were incorrect. As a workaround for full-size windows, we
     * allowed specifying 0 for the width / height, which caused
     * zwlr_layer_shell to tell us the correct size to use.
     *
     * To make fractional scaling work, we have to use
     * wp_viewporter, and no longer need to set the buffer scale.
     * However, viewporter doesn't allow specifying 0 for
     * destination width or height. As a workaround, if 0 size is
     * set, don't use viewporter, warn the user and set the buffer
     * scale here.
     */
    log_warning("Width or height set to 0, disabling fractional scaling support.\n");
    log_warning("If your compositor supports the fractional scale protocol, percentages are preferred.\n");
    window->fractional_scale = 0;
    wl_surface_set_buffer_scale(
        window->surface.wl_surface,
        window->scale);
  } else if (wayland->global.viewporter == NULL) {
    /*
     * We also could be running on a Wayland compositor which
     * doesn't support wp_viewporter, in which case we need to use
     * the old scaling method.
     */
    log_warning("Using an outdated compositor, "
        "fractional scaling will not work properly.\n");
    window->fractional_scale = 0;
    wl_surface_set_buffer_scale(
        window->surface.wl_surface,
        window->scale);
  }

  log_leave_context();
}

void setup_window_init(struct bread *bread)
{
  log_enter_context("setup_window_init");
  struct window *window = bread->window;

  setup_create_window_surface(bread);

  struct wayland *wayland = &bread->wayland;

  /* Grab the first (and only remaining) output from our list. */
  struct wl_output *wl_output;
  {
    struct output_list_element *el;
    el = wl_container_of(wayland->global.output_list.next, el, link);
    wl_output = el->output;
  }

  window->zwlr_layer_surface = zwlr_layer_shell_v1_get_layer_surface(
      wayland->global.zwlr_layer_shell,
      window->surface.wl_surface,
      wl_output,
      ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY,
      "launcher");

  zwlr_layer_surface_v1_set_keyboard_interactivity(
      window->zwlr_layer_surface,
      ZWLR_LAYER_SURFACE_V1_KEYBOARD_INTERACTIVITY_EXCLUSIVE);

  zwlr_layer_surface_v1_add_listener(
      window->zwlr_layer_surface,
      &zwlr_layer_surface_listener,
      bread);

  zwlr_layer_surface_v1_set_anchor(
      window->zwlr_layer_surface,
      window->anchor);

  zwlr_layer_surface_v1_set_exclusive_zone(
      window->zwlr_layer_surface,
      window->exclusive_zone);

  zwlr_layer_surface_v1_set_margin(
      window->zwlr_layer_surface,
      window->margin_top,
      window->margin_right,
      window->margin_bottom,
      window->margin_left);
  /*
   * No matter whether we're scaling via Cairo or not, we're presenting a
   * scaled buffer to Wayland, so scale the window size here if we
   * haven't already done so.
   */
  zwlr_layer_surface_v1_set_size(
      window->zwlr_layer_surface,
      window->width,
      window->height);

  /*
   * Set up a viewport for our surface, necessary for fractional scaling.
   */
  if (wayland->global.viewporter != NULL) {
    window->viewport = wp_viewporter_get_viewport(
        wayland->global.viewporter,
        window->surface.wl_surface);
    if (window->width > 0 && window->height > 0) {
      wp_viewport_set_destination(
          window->viewport,
          window->width,
          window->height);
    }
  }

  /* Commit the surface to finalise setup. */
  wl_surface_commit(window->surface.wl_surface);
  log_leave_context();
}

void setup_fixup_values(struct bread *bread)
{
  log_enter_context("window_fixup_values");
  struct window *window = bread->window;
  uint32_t base_width = window->output_width;
  uint32_t base_height = window->output_height;
  uint32_t scale;

  if (window->fractional_scale != 0) {
    scale = window->fractional_scale;
  } else {
    scale = window->scale * 120;
  }

  /*
   * If we're going to be scaling these values in Cairo,
   * we need to apply the inverse scale here.
   */
  if (window->use_scale) {
    base_width = scale_apply_inverse(base_width, scale);
    base_height = scale_apply_inverse(base_height, scale);
  }

  window->margin_top = fixup_percentage(
      window->margin_top,
      base_height,
      window->margin_top_is_percent);
  window->margin_bottom = fixup_percentage(
      window->margin_bottom,
      base_height,
      window->margin_bottom_is_percent);
  window->margin_left = fixup_percentage(
      window->margin_left,
      base_width,
      window->margin_left_is_percent);
  window->margin_right = fixup_percentage(
      window->margin_right,
      base_width,
      window->margin_right_is_percent);
  window->padding_top = fixup_percentage(
      window->padding_top,
      base_height,
      window->padding_top_is_percent);
  window->padding_bottom = fixup_percentage(
      window->padding_bottom,
      base_height,
      window->padding_bottom_is_percent);
  window->padding_left = fixup_percentage(
      window->padding_left,
      base_width,
      window->padding_left_is_percent);
  window->padding_right = fixup_percentage(
      window->padding_right,
      base_width,
      window->padding_right_is_percent);

  /*
   * Window width and height are a little special. We're only going to be
   * using them to specify sizes to Wayland, which always wants scaled
   * pixels, so always scale them here (unless we've directly specified a
   * scaled size).
   */
  window->width = fixup_percentage(
      window->width,
      window->output_width,
      window->width_is_percent);
  window->height = fixup_percentage(
      window->height,
      window->output_height,
      window->height_is_percent);
  if (window->width_is_percent || !window->use_scale) {
    window->width = scale_apply_inverse(window->width, scale);
  }
  if (window->height_is_percent || !window->use_scale) {
    window->height = scale_apply_inverse(window->height, scale);
  }

  /* Don't attempt percentage handling if exclusive_zone is set to -1. */
  if (window->exclusive_zone > 0) {
    /* Exclusive zone base depends on anchor. */
    switch (window->anchor) {
      case ANCHOR_TOP:
      case ANCHOR_BOTTOM:
        window->exclusive_zone = fixup_percentage(
            window->exclusive_zone,
            base_height,
            window->exclusive_zone_is_percent);
        break;
      case ANCHOR_LEFT:
      case ANCHOR_RIGHT:
        window->exclusive_zone = fixup_percentage(
            window->exclusive_zone,
            base_width,
            window->exclusive_zone_is_percent);
        break;
      default:
        /*
         * Exclusive zone >0 is meaningless for other
         * anchor positions.
         */
        window->exclusive_zone =
          MIN(window->exclusive_zone, 0);
        break;
    }
  }
  log_leave_context();
}

void setup_determine_output(struct bread *bread)
{
  log_enter_context("wayland_determine_output");
  struct wayland *wayland = &bread->wayland;
  struct window *window = bread->window;

  struct surface surface = {
    .width = 1,
    .height = 1,
    .wl_surface = create_surface(bread)
  };

  struct wp_fractional_scale_v1 *fractional_scale = NULL;
  if (wayland->global.fractional_scale_manager != NULL) {
    fractional_scale = wp_fractional_scale_manager_v1_get_fractional_scale(
      wayland->global.fractional_scale_manager,
      surface.wl_surface);
    wp_fractional_scale_v1_add_listener(
      fractional_scale,
      &dummy_fractional_scale_listener,
      bread);
  }

  /*
   * If we have a desired output, make sure we appear on it so we
   * can determine the correct fractional scale.
   */
  struct wl_output *output = NULL;
  if (window->target_output_name[0] != '\0') {
    struct output_list_element *el;
    wl_list_for_each(el, &wayland->global.output_list, link) {
      if (!strcmp(window->target_output_name, el->name)) {
        output = el->output;
        break;
      }
    }
  }

  struct zwlr_layer_surface_v1 *zwlr_layer_surface =
    zwlr_layer_shell_v1_get_layer_surface(
      wayland->global.zwlr_layer_shell,
      surface.wl_surface,
      output,
      ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND,
      "dummy");

  /*
   * Workaround for Hyprland, where if this is not set the dummy
   * surface never enters an output for some reason.
   */
  zwlr_layer_surface_v1_set_keyboard_interactivity(
    zwlr_layer_surface,
    ZWLR_LAYER_SURFACE_V1_KEYBOARD_INTERACTIVITY_EXCLUSIVE
    );

  zwlr_layer_surface_v1_add_listener(
    zwlr_layer_surface,
    &dummy_layer_surface_listener,
    bread);

  zwlr_layer_surface_v1_set_size(
    zwlr_layer_surface,
    1,
    1);

  wl_surface_commit(surface.wl_surface);
  log_debug("First dummy roundtrip start");
  wl_display_roundtrip(wayland->global.display);
  log_debug("First dummy roundtrip done");
  log_debug("Initialising dummy surface");
  surface_init(&surface, wayland->global.shm);
  surface_draw(&surface);
  log_debug("Dummy surface initialised");
  log_debug("Second dummy roundtrip start");
  wl_display_roundtrip(wayland->global.display);
  log_debug("Second dummy roundtrip done.\n");
  surface_destroy(&surface);
  zwlr_layer_surface_v1_destroy(zwlr_layer_surface);
  if (fractional_scale != NULL) {
    wp_fractional_scale_v1_destroy(fractional_scale);
  }
  wl_surface_destroy(surface.wl_surface);

  /*
   * Walk through our output list and select the one we want if
   * the user's asked for a specific one, otherwise just get the
   * default one.
   */
  bool found_target = false;
  struct output_list_element *head;
  head = wl_container_of(wayland->global.output_list.next, head, link);

  struct output_list_element *el;
  struct output_list_element *tmp;
  if (window->target_output_name[0] != 0) {
  } else if (wayland->global.default_output != NULL) {
    snprintf(
        window->target_output_name,
        N_ELEM(window->target_output_name),
        "%s",
        wayland->global.default_output->name);
    /* We don't need this anymore. */
    wayland->global.default_output = NULL;
  }
  wl_list_for_each_reverse_safe(el, tmp, &wayland->global.output_list, link) {
    if (!strcmp(window->target_output_name, el->name)) {
      found_target = true;
      continue;
    }
    /*
     * If we've already found the output we're looking for
     * or this isn't the first output in the list, remove
     * it.
     */
    if (found_target || el != head) {
      wl_list_remove(&el->link);
      wl_output_release(el->output);
      free(el->name);
      free(el);
    }
  }

  /*
   * The only output left should either be the one we want, or
   * the first that was advertised.
   */
  el = wl_container_of(wayland->global.output_list.next, el, link);

  /*
   * If we're rotated 90 degrees, we need to swap width and
   * height to calculate percentages.
   */
  switch (el->transform) {
    case WL_OUTPUT_TRANSFORM_90:
    case WL_OUTPUT_TRANSFORM_270:
    case WL_OUTPUT_TRANSFORM_FLIPPED_90:
    case WL_OUTPUT_TRANSFORM_FLIPPED_270:
      window->output_width = el->height;
      window->output_height = el->width;
      break;
    default:
      window->output_width = el->width;
      window->output_height = el->height;
  }
  window->scale = el->scale;
  window->transform = el->transform;
  log_debug("Selected output %s.\n", el->name);
  log_leave_context();
}


void setup_bread(struct bread *bread)
{
  setup_wayland_init(bread);
  setup_determine_output(bread);
  setup_fixup_values(bread);
  setup_window_init(bread);
}

