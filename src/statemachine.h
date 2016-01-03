#pragma once

enum STATEENUM {STATE_SPLASH, STATE_BEFORERACE, STATE_RACING, STATE_RESULTS, STATE_AFTERRESULTS};

typedef enum STATEENUM STATES;


STATES get_current_state();
void set_current_state(STATES newState);

