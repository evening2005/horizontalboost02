#include <pebble.h>
#include "statemachine.h"


static STATES currentState = STATE_SPLASH;

STATES get_current_state() {
    return currentState;
}

void set_current_state(STATES newState) {
    currentState = newState;
}