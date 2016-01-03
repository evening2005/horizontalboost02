#include "raceui.h"
#include "statemachine.h"


TextLayer *dashboardClock = NULL;
static ResHandle dashboardFontHandle;
GFont dashboardFont;
static char dashClockBuffer[10] = "??:??";

void make_dashboard_clock(Window *window) {
    if(dashboardClock == NULL) {
        dashboardFontHandle = resource_get_handle(RESOURCE_ID_FONT_LARABIE_16);
        dashboardFont = fonts_load_custom_font(dashboardFontHandle);
        GRect clockFrame = { {82, 152} , {62, 16} };
        Layer *windowLayer = window_get_root_layer(window);
        dashboardClock = text_layer_create(clockFrame);
        text_layer_set_font(dashboardClock, dashboardFont);
        text_layer_set_text_color(dashboardClock, GColorIcterine);
        text_layer_set_background_color(dashboardClock, GColorBlack);
        text_layer_set_text_alignment(dashboardClock, GTextAlignmentCenter);
        text_layer_set_text(dashboardClock, dashClockBuffer);
        layer_add_child(windowLayer, (Layer *)dashboardClock);
        layer_set_hidden((Layer *)dashboardClock, true);
        // Subscribe to the tick timer
        // We want to call our function every time the minute changes
        //  handle_tick_timer is in gameui.c
        tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler)update_dashboard_clock);
    }
}

void update_dashboard_clock(struct tm *timeStruct, TimeUnits unitsChanged) {
    STATES cs = get_current_state();
    if(cs == STATE_RACING || cs == STATE_RESULTS || cs == STATE_AFTERRESULTS) {
        //Write the time to the buffer in a safe manner
        strftime(dashClockBuffer, sizeof("00:00"), "%H:%M", timeStruct);
        layer_set_hidden((Layer *)dashboardClock, false);
        layer_mark_dirty((Layer *)dashboardClock);
    }
}




void destroy_dashboard_clock() {
    tick_timer_service_unsubscribe();
    fonts_unload_custom_font(dashboardFont);
    text_layer_destroy(dashboardClock);
}


