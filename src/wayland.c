
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include <wayland-util.h>

#include "wayland.h"
#include "bread.h"
#include "config.h"
#include "keyboard.h"
#include "viewporter.h"
#include "xmalloc.h"

static void wl_keyboard_keymap(
  void *data,
  struct wl_keyboard *keyboard,
  uint32_t format,
  int32_t fd,
  uint32_t size)
{
  assert(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);

  struct bread *bread = data;
  keyboard_keymap(&bread->keyboard, fd, size);
}

static void wl_keyboard_enter(
  void *data,
  struct wl_keyboard *keyboard,
  uint32_t serial,
  struct wl_surface *surface,
  struct wl_array *keys)
{
  /* Deliberately left blank */
}

static void wl_keyboard_leave(
  void *data,
  struct wl_keyboard *keyboard,
  uint32_t serial,
  struct wl_surface *surface)
{
  /* Deliberately left blank */
}

static void wl_keyboard_key(
  void *data,
  struct wl_keyboard *keyboard,
  uint32_t serial,
  uint32_t time,
  uint32_t key,
  uint32_t state)
{
  struct bread *bread = data;

  /*
   * If this wasn't a keypress (i.e. was a key release), just update key
   * repeat info and return.
   */
  if (state != WL_KEYBOARD_KEY_STATE_PRESSED) {
    keyboard_key_held(&bread->keyboard, key);
    return;
  }

  keyboard_key_pressed(&bread->keyboard, key);
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
  struct bread *bread = data;
  keyboard_modifiers(
    &bread->keyboard,
    serial,
    mods_depressed,
    mods_latched,
    mods_locked,
    group);
}

static void wl_keyboard_repeat_info(
  void *data,
  struct wl_keyboard *keyboard,
  int32_t rate,
  int32_t delay)
{
  struct bread *bread = data;
  keyboard_repeat_info(&bread->keyboard, rate, delay);
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
  struct bread *bread = data;
  struct wayland *wayland = &bread->wayland;
  struct output_list_element *el;
  wl_list_for_each(el, &wayland->global.output_list, link) {
    if (el->output == output) {
      el->transform = transform;
    }
  }
}

static void output_mode(
  void *data,
  struct wl_output *output,
  uint32_t flags,
  int32_t width,
  int32_t height,
  int32_t refresh)
{
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
}

static void output_scale(
  void *data,
  struct wl_output *output,
  int32_t factor)
{
  struct bread *bread = data;
  struct wayland *wayland = &bread->wayland;
  struct output_list_element *el;
  wl_list_for_each(el, &wayland->global.output_list, link) {
    if (el->output == output) {
      el->scale = factor;
    }
  }
}

static void output_name(
    void *data,
    struct wl_output *output,
    const char *name)
{
  struct bread *bread = data;
  struct wayland *wayland = &bread->wayland;
  struct output_list_element *el;
  wl_list_for_each(el, &wayland->global.output_list, link) {
    if (el->output == output) {
      el->name = xstrdup(name);
    }
  }
}

static void output_description(
    void *data,
    struct wl_output *wl_output,
    const char *description)
{
  /* Deliberately left blank */
}

static void output_done(void *data, struct wl_output *output)
{
//log_debug("Output configuration done.\n");
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
  /*
  struct tofi *tofi = data;
  if (tofi->hide_cursor) {
    // Hide the cursor by setting its surface to NULL.
    wl_pointer_set_cursor(tofi->wl_pointer, serial, NULL, 0, 0);
  }
  */
}

static void pointer_leave(
  void *data,
  struct wl_pointer *pointer,
  uint32_t serial,
  struct wl_surface *surface)
{
  /* Deliberately left blank */
}

static void pointer_motion(
  void *data,
  struct wl_pointer *pointer,
  uint32_t time,
  wl_fixed_t surface_x,
  wl_fixed_t surface_y)
{
  /* Deliberately left blank */
}

static void pointer_button(
  void *data,
  struct wl_pointer *pointer,
  uint32_t serial,
  uint32_t time,
  uint32_t button,
  enum wl_pointer_button_state state)
{
  /* Deliberately left blank */
}

static void pointer_axis(
  void *data,
  struct wl_pointer *pointer,
  uint32_t time,
  enum wl_pointer_axis axis,
  wl_fixed_t value)
{
  /* Deliberately left blank */
}

static void pointer_frame(void *data, struct wl_pointer *pointer)
{
  /* Deliberately left blank */
}

static void pointer_axis_source(
  void *data,
  struct wl_pointer *pointer,
  enum wl_pointer_axis_source axis_source)
{
  /* Deliberately left blank */
}

static void pointer_axis_stop(
  void *data,
  struct wl_pointer *pointer,
  uint32_t time,
  enum wl_pointer_axis axis)
{
  /* Deliberately left blank */
}

static void pointer_axis_discrete(
  void *data,
  struct wl_pointer *pointer,
  enum wl_pointer_axis axis,
  int32_t discrete)
{
  /* Deliberately left blank */
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
}

static void seat_name(void *data, struct wl_seat *seat, const char *name)
{
  /* Deliberately left blank */
}

static const struct wl_seat_listener seat_listener = {
  .capabilities = seat_capabilities,
  .name = seat_name,
};

static void registry_global(
  void *data,
  struct wl_registry *registry,
  uint32_t name,
  const char *interface,
  uint32_t version)
{
  struct bread *bread = data;
  struct wayland *wayland = &bread->wayland;
  if (!strcmp(interface, wl_compositor_interface.name)) {
    wayland->global.compositor = wl_registry_bind(
      registry,
      name,
      &wl_compositor_interface,
      4);
  } else if (!strcmp(interface, wl_seat_interface.name)) {
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
    wayland->global.shm = wl_registry_bind(
      registry,
      name,
      &wl_shm_interface,
      1);
  } else if (!strcmp(interface, wl_data_device_manager_interface.name)) {
    wayland->global.data_device_manager = wl_registry_bind(
      registry,
      name,
      &wl_data_device_manager_interface,
      3);
  } else if (!strcmp(interface, zwlr_layer_shell_v1_interface.name)) {
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
    wayland->global.viewporter = wl_registry_bind(
      registry,
      name,
      &wp_viewporter_interface,
      1);
  } else
    if (!strcmp(interface, wp_fractional_scale_manager_v1_interface.name)) {
    wayland->global.fractional_scale_manager = wl_registry_bind(
      registry,
      name,
      &wp_fractional_scale_manager_v1_interface,
      1);
  }
}

static void registry_global_remove(
  void *data,
  struct wl_registry *registry,
  uint32_t name)
{
	/* Deliberately left blank */
}

static const struct wl_registry_listener registry_listener = {
  .global = registry_global,
  .global_remove = registry_global_remove,
};

void wayland_init_globals(struct wl_globals *global)
{
  global->display = wl_display_connect(NULL);

  if (global->display == NULL) {
    exit(EXIT_FAILURE);
  }

  wl_list_init(&global->output_list);
  global->registry = wl_display_get_registry(global->display);
}

struct wayland wayland_create(struct config *conf)
{
  struct wayland wayland;

  wayland_init_globals(&wayland.global);
  wl_registry_add_listener(
    wayland.global.registry,
    &registry_listener,
    &wayland);


  return wayland;
}
