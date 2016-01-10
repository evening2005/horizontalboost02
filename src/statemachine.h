#pragma once

enum STATEENUM {STATE_SPLASH, 
                STATE_SHOWMAINMENU, 
                STATE_SELECTFUNCTION, 
                STATE_MARATHON, 
                STATE_SPRINT, 
                STATE_BEFORERACE, 
                STATE_RACING, 
                STATE_RESULTS, 
                STATE_AFTERRESULTS, 
                STATE_QUITTING, 
                STATE_DEAD};

typedef enum STATEENUM STATES;


STATES get_current_state();
void set_current_state(STATES newState);

