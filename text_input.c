#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include "text-input-unstable-v3-client-protocol.h"

static struct wl_display *display;
static struct wl_registry *registry;
static struct wl_seat *seat;
static struct zwp_text_input_manager_v3 *manager;
static struct zwp_text_input_v3 *text_input;

static void registry_global(
    void *data,
    struct wl_registry *registry,
    uint32_t name,
    const char *interface,
    uint32_t version
) {
    if (strcmp(interface, wl_seat_interface.name) == 0) {
        seat = wl_registry_bind(registry, name, &wl_seat_interface, 1);
    }
    if (strcmp(interface, zwp_text_input_manager_v3_interface.name) == 0) {
        manager = wl_registry_bind(
            registry,
            name,
            &zwp_text_input_manager_v3_interface,
            1
        );
    }
}

static void registry_remove(void *data, struct wl_registry *registry, uint32_t name) {}

static const struct wl_registry_listener registry_listener = {
    .global = registry_global,
    .global_remove = registry_remove,
};

void wl_init() {
    display = wl_display_connect(NULL);
    if (!display) return;

    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);
    wl_display_roundtrip(display);

    if (!seat || !manager) return;

    text_input = zwp_text_input_manager_v3_get_text_input(manager, seat);
    zwp_text_input_v3_enable(text_input);
    zwp_text_input_v3_commit(text_input);
    wl_display_roundtrip(display);
}

void wl_commit_text(const char *text) {
    if (!text_input) return;

    zwp_text_input_v3_set_surrounding_text(text_input, text, 0, 0);
    zwp_text_input_v3_commit(text_input);
    wl_display_flush(display);
}

void wl_cleanup() {
    if (text_input) zwp_text_input_v3_destroy(text_input);
    if (manager) zwp_text_input_manager_v3_destroy(manager);
    if (seat) wl_seat_destroy(seat);
    if (registry) wl_registry_destroy(registry);
    if (display) wl_display_disconnect(display);
}
