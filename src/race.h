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
void race_create_cars(int difficulty);

struct DIFFICULTY {
    uint8_t aiRankMin;
    uint8_t aiRankMax;
    uint8_t aiChanceMin;
    uint8_t aiChanceMax;
};

typedef struct DIFFICULTY difficultyType;

static difficultyType levels[10] = {
    {4,6,32,64},
    {3,6,64,128},
    {3,5,64,128},
    {2,4,64,128},
    {1,3,64,128},
    {1,3,160,200},
    {1,3,208,224},
    {0,2,208,224},
    {0,2,224,240},
    {0,1,240,255}
};




