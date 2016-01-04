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
void race_create_cars(int level);

struct DIFFICULTY {
    uint8_t aiRankMin;
    uint8_t aiRankMax;
    uint8_t aiChanceMin;
    uint8_t aiChanceMax;
};

typedef struct DIFFICULTY difficultyType;







