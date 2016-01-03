#pragma once
#include "pebble.h"

#define RESULTS_TOP (50)
#define RESULTS_LEFT (8)
#define RESULTS_GAP  (2)
#define RESULTS_CAR_LEFT (108)

void race_result_create_position_layers();
void race_result_destroy_assets();
void race_result_destroy_animations();
void race_result_populate_position_layers(GContext *ctx);
// bool race_result_check_for_select();
void load_results_title_fonts() ;
void destroy_results_title_fonts();
