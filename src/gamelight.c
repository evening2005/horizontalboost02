#include <pebble.h>
#include "gamelight.h"

static AppTimer *lightTimer;

void light_off(void *data) {
    light_enable(false);
}

void switch_on_light() {
    light_enable(true);
    if(!app_timer_reschedule(lightTimer,35000)) {
        lightTimer = app_timer_register(35000,light_off,NULL);
    }
}

