//
// Copyright (c) 2015 Jeffrey M. Squyres
// $HEADER$
//

#include <pebble.h>

#include "jaywalk.h"

static TextLayer *s_time_layer;
static GFont s_time_font;
static Layer *s_date_time_line_layer;
static TextLayer *s_date_layer;
static GFont s_date_font;

static int margin_offset = 5;

/*************************************************************************/

void jaywalk_time_display() {
    // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    // Display the date
    static char date_str[128];
    date_str[sizeof(date_str) - 1] = '\0';
    strftime(date_str, sizeof(date_str) - 1, "%A\n%B %e, %Y", tick_time);
    text_layer_set_text(s_date_layer, date_str);

    // Display the time
    static char time_str[] = "00:00:00";
    if (clock_is_24h_style() == true) {
        strftime(time_str, sizeof(time_str), "%k:%M:%S", tick_time);
    } else {
        strftime(time_str, sizeof(time_str), "%l:%M:%S", tick_time);
    }
    text_layer_set_text(s_time_layer, time_str);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    jaywalk_time_display();
}

/*************************************************************************/

static void draw_date_time_line(Layer *layer, GContext *c) {
    GRect window_bounds = layer_get_bounds(layer);

    GPoint left = GPoint(0, 0);
    GPoint right = GPoint(window_bounds.size.w, 0);
    graphics_context_set_stroke_color(c, GColorBlack);
    graphics_draw_line(c, left, right);
}

void jaywalk_time_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_bounds(window_layer);

    // Time.  Start y at the bottom of the window, subtract off 1 line
    // of text (font size), and a little fluff (10);
    int width, height, x, y;
    width = window_bounds.size.w - 2 * margin_offset;
    height = 45;
    x = margin_offset;
    y = window_bounds.size.h - 45 - 10;;
    s_time_layer = text_layer_create(GRect(x, y, width, height));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    s_time_font =
        fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_45));
    text_layer_set_font(s_time_layer, s_time_font);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
    layer_add_child(window_layer,
                    text_layer_get_layer(s_time_layer));

    // Line between time and date.
    width = window_bounds.size.w - 10;
    height = 1;
    x = margin_offset;
    y -= 0;
    s_date_time_line_layer = layer_create(GRect(x, y, width, height));
    layer_add_child(window_layer, s_date_time_line_layer);
    layer_set_update_proc(s_date_time_line_layer, draw_date_time_line);

    // Date.  Start at the y from the date/time line, subtract 2 lines
    // of text (2*font size) and a little fluff (5).
    width = window_bounds.size.w - 2 * margin_offset;
    height = 40;
    x = margin_offset;
    y -= (20 + 20 + 5);
    s_date_layer = text_layer_create(GRect(x, y, width, height));
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorBlack);
    s_date_font =
        fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATE_20));
    text_layer_set_font(s_date_layer, s_date_font);
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
    layer_add_child(window_layer,
                    text_layer_get_layer(s_date_layer));
}

void jaywalk_time_window_unload(Window *window) {
    text_layer_destroy(s_time_layer);
    fonts_unload_custom_font(s_time_font);
    text_layer_destroy(s_date_layer);
    fonts_unload_custom_font(s_date_font);
}

/*************************************************************************/

void jaywalk_time_init(void) {
    // Register with TickTimerService
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

    jaywalk_time_display();
}

void jaywalk_time_finalize(void) {
    tick_timer_service_unsubscribe();
}
