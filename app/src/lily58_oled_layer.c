#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static const struct device *left_display = DEVICE_DT_GET(DT_CHOSEN(zmk_display_left));

static char layer_name[16];

ZMK_EVENT_IMPL(layer_state_changed);

static void update_left_oled(void) {
    uint8_t layer = zmk_keymap_layer_active();
    switch (layer) {
        case 0:
            snprintf(layer_name, sizeof(layer_name), "Default");
            break;
        case 1:
            snprintf(layer_name, sizeof(layer_name), "Raise");
            break;
        case 2:
            snprintf(layer_name, sizeof(layer_name), "Lower");
            break;
        default:
            snprintf(layer_name, sizeof(layer_name), "Layer %d", layer);
            break;
    }
    if (left_display) {
        struct display_buffer_descriptor desc = {
            .buf_size = 128 * 64 / 8,
            .width = 128,
            .height = 64,
            .pitch = 128
        };
        display_blanking_on(left_display);
        struct display_capabilities caps;
        display_get_capabilities(left_display, &caps);
        display_set_pixel_format(left_display, PIXEL_FORMAT_MONO10);
        display_write(left_display, 0, 0, &desc, layer_name);
        display_blanking_off(left_display);
    }
}

ZMK_LISTENER(layer_display, layer_state_changed);
ZMK_SUBSCRIPTION(layer_display, zmk_layer_state_changed);

void layer_display_handler(struct zmk_event_header *event) {
    update_left_oled();
}
