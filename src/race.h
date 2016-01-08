#pragma once

#include "car.h"


carType *race_get_finisher(int rank);
void race_set_start_time();
void race_draw_cars(GContext *ctx);
void race_delete_cars();
void race_set_player(carType *pc);
void race_frame_update();
void race_place_cars_on_grid();
void race_add_to_grid(carType *carPtr);
void race_create_cars();
void race_set_difficulty(uint8_t difficulty);
void race_reset_cars();


