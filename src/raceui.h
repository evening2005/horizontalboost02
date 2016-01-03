#pragma once
#include <pebble.h>

void make_dashboard_clock(Window *window);
void update_dashboard_clock(struct tm *timeStruct, TimeUnits unitsChanged);
void destroy_dashboard_clock();
