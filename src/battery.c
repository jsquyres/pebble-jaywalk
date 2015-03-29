//
// Copyright (c) 2015 Jeffrey M. Squyres
// $HEADER$
//

#include "jaywalk.h"

static TextLayer *s_battery_charge_layer;
static GFont s_battery_charge_font;
static GBitmap *s_battery_icon;
static BitmapLayer *s_battery_icon_layer;

static char battery_percentage_str[8];
static int min_battery_charge = 30;


static void jaywalk_battery_display_work(BatteryChargeState charge) {
    // Only display the battery value if it is <=
    // <min_battery_charge>%, if it is plugged in, or if we're in
    // "shake mode"
    if (charge.charge_percent <= min_battery_charge ||
        charge.is_plugged ||
        jaywalk_shake_get_display_mode() == JAYWALK_DISPLAY_MODE_SHAKE) {
        snprintf(battery_percentage_str,
                 sizeof(battery_percentage_str) - 1, "%2d%%",
                 charge.charge_percent);
    } else {
        battery_percentage_str[0] = '\0';
    }

    text_layer_set_text(s_battery_charge_layer, battery_percentage_str);

    // If we're plugged in, un-hide the battery charge icon.
    // Otherwise, hide it.
    if (charge.is_plugged) {
        layer_set_hidden(bitmap_layer_get_layer(s_battery_icon_layer),
                         false);
    } else {
        layer_set_hidden(bitmap_layer_get_layer(s_battery_icon_layer),
                         true);
    }
}

static void jaywalk_battery_handler(BatteryChargeState charge) {
    jaywalk_battery_display_work(charge);
}

/*************************************************************************/

void jaywalk_battery_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // Battery charge percentage: top right corner
    int width = 30;
    int height = 14;
    int x = bounds.size.w - width;
    int y = 0;
    s_battery_charge_layer = text_layer_create((GRect) {
            .origin = {
                .x = x,
                .y = y
            },
            .size = {
                .w = width,
                .h = height
            }
        });
    s_battery_charge_font =
        fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BATTERY_14));
    text_layer_set_font(s_battery_charge_layer, s_battery_charge_font);
    text_layer_set_text_alignment(s_battery_charge_layer,
                                  GTextAlignmentLeft);
    text_layer_set_background_color(s_battery_charge_layer, GColorClear);
    text_layer_set_text_color(s_battery_charge_layer, GColorBlack);
    layer_add_child(window_layer,
                    text_layer_get_layer(s_battery_charge_layer));

    // Charging icon: top right corner
    s_battery_icon =
        gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHARGING);
#ifdef PBL_PLATFORM_BASALT
    width = gbitmap_get_bounds(s_battery_icon).size.w;
    height = gbitmap_get_bounds(s_battery_icon).size.h;
#else
    width = s_battery_icon->bounds.size.w;
    height = s_battery_icon->bounds.size.h;
#endif
    x -= width;
    y = 0;
    s_battery_icon_layer = bitmap_layer_create((GRect) {
            .origin = {
                .x = x,
                .y = y
            },
            .size = {
                .w = width,
                .h = height
            }
        });
    bitmap_layer_set_alignment(s_battery_icon_layer, GAlignLeft);
    bitmap_layer_set_background_color(s_battery_icon_layer, GColorClear);
    bitmap_layer_set_bitmap(s_battery_icon_layer, s_battery_icon);
    layer_set_hidden(bitmap_layer_get_layer(s_battery_icon_layer), true);

    // Add the text layer
    layer_add_child(window_layer,
                    bitmap_layer_get_layer(s_battery_icon_layer));
}

void jaywalk_battery_window_unload(Window *window) {
    text_layer_destroy(s_battery_charge_layer);
    fonts_unload_custom_font(s_battery_charge_font);
    bitmap_layer_destroy(s_battery_icon_layer);
    gbitmap_destroy(s_battery_icon);
}

/*************************************************************************/

void jaywalk_battery_init(void) {
    jaywalk_battery_display_work(battery_state_service_peek());
    battery_state_service_subscribe(jaywalk_battery_handler);
}

void jaywalk_battery_display() {
    jaywalk_battery_display_work(battery_state_service_peek());
}

void jaywalk_battery_finalize(void) {
    battery_state_service_unsubscribe();
}
