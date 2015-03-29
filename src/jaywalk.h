//
// Copyright (c) 2015 Jeffrey M. Squyres
// $HEADER$
//

#ifndef _JAYWALK_H_
#define _JAYWALK_H_

#include <pebble.h>

typedef enum {
    JAYWALK_DISPLAY_MODE_NORMAL,
    JAYWALK_DISPLAY_MODE_SHAKE
} jaywalk_display_mode_t;

void jaywalk_display(void);

void jaywalk_time_window_load(Window *window);
void jaywalk_time_window_unload(Window *window);

void jaywalk_time_init(void);
void jaywalk_time_display(void);
void jaywalk_time_finalize(void);

void jaywalk_battery_window_load(Window *window);
void jaywalk_battery_window_unload(Window *window);

void jaywalk_battery_init(void);
void jaywalk_battery_display(void);
void jaywalk_battery_finalize(void);

void jaywalk_shake_init(void);
jaywalk_display_mode_t jaywalk_shake_get_display_mode(void);
void jaywalk_shake_finalize(void);


#endif /* _JAYWALK_H_ */
