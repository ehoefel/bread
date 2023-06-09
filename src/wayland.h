#ifndef WAYLAND_H
#define WAYLAND_H

#include <wayland-client.h>
#include <wayland-util.h>
#include "config.h"
//#include "listener.h"

struct output_list_element {
  struct wl_list link;
  struct wl_output *output;
  char *name;
  uint32_t width;
  uint32_t height;
  int32_t scale;
  int32_t transform;
};


struct wl_globals {
  struct wl_display *display;
  struct wl_registry *registry;
  struct wl_compositor *compositor;
  struct wl_seat *seat;
  struct wl_shm *shm;
  struct wl_data_device_manager *data_device_manager;
  struct wl_data_device *data_device;
  struct wp_viewporter *viewporter;
  struct wp_fractional_scale_manager_v1 *fractional_scale_manager;
  struct zwlr_layer_shell_v1 *zwlr_layer_shell;
  struct wl_list output_list;
  struct output_list_element *default_output;
};

struct wayland {
  struct wl_globals global;
  struct wl_keyboard *keyboard;
  struct wl_pointer *pointer;
};

//void wayland_set_listener(struct wayland *wayland, struct listener *listener);
struct wayland wayland_create(struct config *conf);
void wayland_init(struct wayland *wayland);
struct wl_surface *wayland_create_surface(struct wayland *wayland);


#endif /* WAYLAND_H */
