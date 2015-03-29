//
// Copyright (c) 2015 Jeffrey M. Squyres
// $HEADER$
//

#include "jaywalk.h"

static Window *s_main_window;

/*************************************************************************/

void jaywalk_display(void) {
    jaywalk_time_display();
    jaywalk_battery_display();
}

/*************************************************************************/

static void main_window_load(Window *window) {
    jaywalk_time_window_load(window);
    jaywalk_battery_window_load(window);
}

static void main_window_unload(Window *window) {
    jaywalk_time_window_unload(window);
    jaywalk_battery_window_unload(window);
}

/*************************************************************************/

static void init(void) {
    // Create main Window element and assign to pointer
    s_main_window = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
            .load = main_window_load,
            .unload = main_window_unload
    });

    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);

    // Setup our windows
    jaywalk_time_init();
    jaywalk_battery_init();
    jaywalk_shake_init();
}

static void finalize(void) {
    jaywalk_shake_finalize();
    jaywalk_battery_finalize();
    jaywalk_time_finalize();

    window_destroy(s_main_window);
}

/*************************************************************************/

int main(void) {
    // Setup to party!
    init();

    // Party!
    app_event_loop();

    // Post-party cleanup sadness
    finalize();
}
