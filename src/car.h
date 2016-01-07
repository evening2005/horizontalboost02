#pragma once
#include "trackHB.h"
#include "pge_sprite.h"

// STEER_AMOUNT must remain 1 for the AI steering to work properly
#define STEER_AMOUNT (1)
#define STEER_SLOWDOWN (0)
#define REAR_END_PENALTY (1)

#define BOOST_COOLDOWN_MILLIS (2048)
#define BOOST_MINIMUM_COOLDOWN (BOOST_COOLDOWN_MILLIS >> 1)

#define SPEED_INC (3)
#define CRASH_SLOWDOWN (1)
#define BOOST_INC (13)
#define DRAG_DIVISOR (21)
#define BOOST_CHANCE (16384) // This is compared with a random number (0..65535)

#define MAX_NPCS (8)

#define min(a, b) ((a) <= (b) ? (a) : (b))


#define CAR_LENGTH (30)
#define CAR_WIDTH (17)

#define PASSING_CLEARANCE (2)
#define NO_STEERING_PLAN  (-9999)



#define PLAYERFIRSTSCREENX (80)
#define PLAYERLASTSCREENX (6)
#define PLAYERMIDSCREENX (40)



struct CAR {
    uint8_t carNumber;
    PGESprite *sprite;
    GPoint startingPosition;
    GPoint worldPosition;
    uint32_t rank;
    bool boosting;
    uint16_t currentSpeed; 
    uint64_t finished;
    int steeringPlan; // This is a target for worldPosition.y
    uint8_t thisCarBoostInc;
    time_t boostStartMillis;
    time_t boostDurationMillis;
    time_t maxBoostDurationMillis;
    time_t lastBoostMillis;
    GColor8 carColour;
    char carName[12];
};

typedef struct CAR carType;


void car_update_boost_ui(GContext *ctx, carType *carPtr);
void car_initialise(carType *carPtr, int resourceID, GColor colour, char *name);
void car_reset(carType *carPtr, carType *playerCar, uint16_t howManyNPCs, uint64_t raceStartTime);
void car_set_camera_focus(carType *playerCar);
int car_get_camera_focus();
void car_movement(carType *carPtr, uint16_t howManyNPCs, carType *sortedGrid[]);
void car_drive_player(carType *carPtr, carType *playerCar, uint16_t howManyNPCs, carType *sortedGrid[]);
void car_drive_npc(carType *carPtr, carType *playerCar, uint16_t howManyNPCs, carType *sortedGrid[]);
void car_draw(GContext *ctx, carType *carPtr, carType *playerCar);
void car_frame_update();
void car_check_boost_status(carType *carPtr);
void car_check_for_finisher(carType *carPtr, uint64_t raceStartTime);
void car_delete(carType *carPtr);
void car_set_position(carType *carPtr, carType *playerCar, uint16_t howManyNPCs, uint64_t raceStartTime);
uint64_t get_milli_time();
void car_set_player_screen_pos(int posX);
void car_set_difficulty(carType *carPtr, uint8_t boostInc);

/*

void make_pos_gap_ui(Window *window);
void destroy_pos_gap_ui();

*/
