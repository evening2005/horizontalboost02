#include <pebble.h>
#include "race.h"
#include "statemachine.h"

carType blueCar;
carType orangeCar;
carType yellowCar;
carType greenCar;
carType blackCar;
carType blackTruck;
carType orangeTruck;
carType yellowTruck;
carType greenTruck;


static uint16_t howManyNPCs = 0;
static carType *playerCar;
static carType *startingGrid[MAX_NPCS + 1]; 
static carType *sortedGrid[MAX_NPCS + 1];
static uint64_t raceStartTime = 0;
static carType *finishingOrder[MAX_NPCS + 1];

void race_set_start_time() {
    raceStartTime = get_milli_time();
}


// rank is numbered from 0
carType *race_get_finisher(int rank) {
    if(rank < 0) rank = 0;
    if(rank > MAX_NPCS) rank = MAX_NPCS;
    return finishingOrder[rank];
}

void race_delete_cars() {
    int c;
    carType *carPtr;
    for(c=0; c <= howManyNPCs; c++) {
        carPtr = startingGrid[c];
        car_delete(carPtr);
    }
}


void race_add_to_grid(carType *carPtr) {
    if(carPtr == playerCar) {
        startingGrid[0] = carPtr;
        carPtr->carNumber = 1;
    } else {
        if(howManyNPCs >= MAX_NPCS) return;
        howManyNPCs++;
        startingGrid[howManyNPCs] = carPtr;
        carPtr->carNumber = howManyNPCs + 1;
    }
}




// The "player Car" is the one at the centre of the window
void race_set_player(carType *pc) {
    playerCar = pc;
    race_add_to_grid(pc);
}



void race_sort_cars() {
    int c=0;
    for(c=0; c <= howManyNPCs; c++) {
        sortedGrid[c] = startingGrid[c];
    }
    int i, j;
    carType *temp;
    for(i=0; i < howManyNPCs; i++) {
        for(j=i+1; j < howManyNPCs + 1; j++) {
            if(sortedGrid[j]->worldPosition.x > sortedGrid[i]->worldPosition.x) {
                temp = sortedGrid[i]; 
                sortedGrid[i] = sortedGrid[j];
                sortedGrid[j] = temp;
            }
        }
    }
    for(c=0; c <= howManyNPCs; c++) {
        sortedGrid[c]->rank = c;
    }
}

void race_sort_cars_by_finishing_time() {
    int c=0;
    for(c=0; c <= howManyNPCs; c++) {
        finishingOrder[c] = startingGrid[c];
    }
    int i, j;
    carType *temp;
    for(i=0; i < howManyNPCs; i++) {
        for(j=i+1; j < howManyNPCs + 1; j++) {
            if(finishingOrder[j]->finished < finishingOrder[i]->finished) {
                temp = finishingOrder[i]; 
                finishingOrder[i] = finishingOrder[j];
                finishingOrder[j] = temp;
            }
        }
    }
    for(c=0; c <= howManyNPCs; c++) {
        finishingOrder[c]->rank = c;
    } 
}



void race_draw_cars(GContext *ctx) {
    int c;
    carType *carPtr;
    
    //draw_finish_line(ctx, cameraFocus);
    for(c=0; c <= howManyNPCs; c++) {
        carPtr = startingGrid[c];
        car_draw(ctx, carPtr, playerCar);
    }
    car_update_boost_ui(ctx, playerCar);
}



void race_show_results(void *data) {
    // Switching to this state will cause the results to appear
    // layer_set_hidden((Layer *)positionAndGap, true);
    set_current_state(STATE_RESULTS);
}

bool race_all_cars_finished() {
    int c;
    for(c=0; c < MAX_NPCS; c++) {
        if(startingGrid[c]->finished == 0) return false;
    }
    // If we get here it means they've all finished
    race_sort_cars_by_finishing_time();
    return true;
}


static AppTimer *endOfRaceTimer;

void race_frame_update() {
    car_set_camera_focus(playerCar);
    
    race_sort_cars();
    // update_position_ui();
    int c;
    carType *carPtr;
    for(c=0; c <= howManyNPCs; c++) {
        carPtr = sortedGrid[c];
        car_check_boost_status(carPtr);
        if(carPtr == playerCar) {
            car_drive_player(carPtr, playerCar, howManyNPCs, sortedGrid);
        } else {
            car_drive_npc(carPtr, playerCar, howManyNPCs, sortedGrid);
        }
        car_movement(carPtr, howManyNPCs, sortedGrid);
        car_set_position(carPtr, playerCar, howManyNPCs, raceStartTime);
        car_check_for_finisher(carPtr, raceStartTime);
    }
    if(race_all_cars_finished()) {
        // Switch to the results screen after a short delay
        endOfRaceTimer = app_timer_register(2000, (AppTimerCallback)race_show_results, NULL);
    }
}



// This is used to position the cars on the grid
// *** It can only be called after the startingGrid has been filled! ***
void race_place_cars_on_grid() {
    car_set_player_screen_pos(PLAYERMIDSCREENX);
//    playerScreenPosX = PLAYERMIDSCREENX;
    GRect gridRect;
    // This sets up the positions of the grid on the track (trackHB)
    set_up_grid_positions();
    shuffle_grid_positions(howManyNPCs);
    playerCar->finished = 0;
    car_set_camera_focus(playerCar);
    // Now go through each car and find its position on the starting line
    for(uint32_t c=0; c <= howManyNPCs; c++) {
        car_reset(startingGrid[c], playerCar, howManyNPCs, raceStartTime);
        gridRect = get_grid_position(c);
        startingGrid[c]->startingPosition = gridRect.origin;
        startingGrid[c]->worldPosition = startingGrid[c]->startingPosition;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "starting positions: %d : %d", startingGrid[c]->worldPosition.x, startingGrid[c]->worldPosition.y);
    }
    // NB!!!! REINSTATE WHEN ALL THIS IS WORKING !!!!!
    //layer_set_hidden((Layer *)positionAndGap, false);
    //updateOnceAfterFinishing = false;
}



void race_create_cars(int difficulty) {
    
    car_initialise(&blueCar, RESOURCE_ID_BLUE_CAR, GColorCadetBlue, "Player", 0, 64);    
    race_set_player(&blueCar);
    car_initialise(&orangeCar, RESOURCE_ID_ORANGE_CAR, GColorOrange, "Orange", 0, 64);
    race_add_to_grid(&orangeCar);
    car_initialise(&yellowCar, RESOURCE_ID_YELLOW_CAR, GColorChromeYellow, "Yellow", 0, 64);    
    race_add_to_grid(&yellowCar);
    car_initialise(&greenCar, RESOURCE_ID_GREEN_CAR, GColorGreen, "Green", 0, 64);    
    race_add_to_grid(&greenCar);
    car_initialise(&orangeTruck, RESOURCE_ID_ORANGE_TRUCK, GColorOrange, "OrangeT", 0, 64);
    race_add_to_grid(&orangeTruck);
    car_initialise(&yellowTruck, RESOURCE_ID_YELLOW_TRUCK, GColorChromeYellow, "YellowT", 0, 64);    
    race_add_to_grid(&yellowTruck);
    car_initialise(&greenTruck, RESOURCE_ID_GREEN_TRUCK, GColorGreen, "GreenT", 0, 64);    
    race_add_to_grid(&greenTruck);
    car_initialise(&blackCar, RESOURCE_ID_BLACK_CAR, GColorBlack, "Black", 0, 64);
    race_add_to_grid(&blackCar);
    car_initialise(&blackTruck, RESOURCE_ID_BLACK_TRUCK, GColorBlack, "BlackT", 0, 64);
    race_add_to_grid(&blackTruck);
    
    // All the resets are done together here - previously, car_intialise(..) called car_reset(..)
    for(int i=0; i <= howManyNPCs; i++) {
        car_reset(startingGrid[i], playerCar, howManyNPCs, raceStartTime);
    }
    
}












