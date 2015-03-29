//
// Copyright (c) 2015 Jeffrey M. Squyres
// $HEADER$
//

#include "jaywalk.h"

// Seconds
static time_t shake_timeout = 5;

// When a shake is detected, this is set to be
// time(NULL)+shake_timeout
static time_t display_shake_until = 0;

// What display mode are we in?  Start in jaywalk mode.
static jaywalk_display_mode_t mode = JAYWALK_DISPLAY_MODE_NORMAL;


//
// Called at the end of the shake timeout to reset the display
//
static void jaywalk_wakeup_restore_normal() {
    mode = JAYWALK_DISPLAY_MODE_NORMAL;
    display_shake_until = 0;

    jaywalk_display();
}

//
// Handler for the tap (shake) service
//
static void jaywalk_shake_handler(AccelAxisType axis, int32_t direction) {
    // We don't care what tap/shake happened -- just that it happened.
    if (JAYWALK_DISPLAY_MODE_NORMAL == mode) {
        mode = JAYWALK_DISPLAY_MODE_SHAKE;
    } else {
        mode = JAYWALK_DISPLAY_MODE_NORMAL;
    }

    // If we are now displaying exact, set a timeout for returning to
    // normal.  Do it two ways:
    //
    // 1. Set a wakeup timer for 5 seconds from now.  PebbleOS sets
    // restrictions on wakeup timers, however, so try starting 5
    // seconds from now, up to two minutes from now.  If we can't set
    // a wakeup in that time, we'll end up falling over to method #2.
    //
    // 2. If the next minute tick handler fires after <timeout>
    // seconds after we went to exact time, it will reset back to
    // normal.
    if (JAYWALK_DISPLAY_MODE_SHAKE == mode) {
        wakeup_cancel_all();
        wakeup_service_subscribe(jaywalk_wakeup_restore_normal);

        time_t t = time(NULL);
        display_shake_until = t + shake_timeout;

        for (int i = shake_timeout; i < 120; ++i) {
            int ret;
            ret = wakeup_schedule(t + i, 0, false);
            if (0 == ret) {
                break;
            }
        }
    }

    jaywalk_display();
}

/*************************************************************************/

void jaywalk_shake_init(void) {
    accel_tap_service_subscribe(jaywalk_shake_handler);
    jaywalk_display();
}

void jaywalk_shake_update(void) {
    jaywalk_shake_init();
}

jaywalk_display_mode_t jaywalk_shake_get_display_mode(void) {
    // Did we fall into method #2 from jaywalk_shake_handler?
    time_t t = time(NULL);
    if (display_shake_until > 0 && t > display_shake_until) {
        mode = JAYWALK_DISPLAY_MODE_NORMAL;
        wakeup_cancel_all();
    }

    return mode;
}

void jaywalk_shake_finalize(void) {
    wakeup_cancel_all();
    accel_tap_service_unsubscribe();
}
