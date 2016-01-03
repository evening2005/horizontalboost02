#include "pge.h"
#include "car.h"

#include "pge_collision.h"
#include "statemachine.h"



/*

TextLayer *positionAndGap = NULL;

void make_pos_gap_ui(Window *window) {
    if(positionAndGap == NULL) {
        GRect greyFrame = { {16, 152} , {112, 16} };
        Layer *windowLayer = window_get_root_layer(window);
        positionAndGap = text_layer_create(greyFrame);
        text_layer_set_background_color(positionAndGap, GColorLightGray);
        text_layer_set_text_alignment(positionAndGap, GTextAlignmentLeft);   
        layer_add_child(windowLayer, (Layer *)positionAndGap);      
    }
}

void destroy_pos_gap_ui() {
    text_layer_destroy(positionAndGap);
}

static char posGapText[32];
static bool updateOnceAfterFinishing = false;

void update_position_ui() {
    if(updateOnceAfterFinishing) return;
    
    int gap;
    if(sortedGrid[0] == playerCar) {
        text_layer_set_text_color(positionAndGap, GColorMintGreen);
        gap = sortedGrid[0]->worldPosition.y - sortedGrid[1]->worldPosition.y;
    } else {
        text_layer_set_text_color(positionAndGap, GColorDarkCandyAppleRed);
        gap = sortedGrid[0]->worldPosition.y - playerCar->worldPosition.y;
    }
    int gapMetres = gap / 10;
    snprintf(posGapText, 30, "%2d (%03d)", (int)playerCar->rank+1, gapMetres);        

    text_layer_set_text(positionAndGap, posGapText);

    if(playerCar->finished) {
        updateOnceAfterFinishing = true;   
    }
}


*/

void car_delete(carType *carPtr) {
    pge_sprite_destroy(carPtr->sprite);
}


void car_make_rect(const carType *carPtr, GRect *tempRect) {
    tempRect->origin = carPtr->worldPosition;
    tempRect->size.h = CAR_WIDTH;
    tempRect->size.w = CAR_LENGTH;
}

static int cameraFocus;

void car_set_camera_focus(carType *playerCar) {
    if(playerCar->finished == 0) {
        cameraFocus = playerCar->worldPosition.x;    
    } else {
        cameraFocus = TRACK_FINISH_LINE;
    }
}

int car_get_camera_focus() {
    return cameraFocus;
}

uint64_t get_milli_time() {
    time_t seconds;
    uint16_t millis;
    time_ms(&seconds, &millis);
    return (seconds * 1000 + millis);          
}



static GRect boostUIOuter = { {1, 52}, {12, 66} };
static GRect boostUIInner = { {2, 53}, {10, 64} };

// carPtr would presumably always be the playerCar, but I
//  want to move away from specificity in this iteration
void car_update_boost_ui(GContext *ctx, carType *carPtr) {
    // The boost ui is just a rectangle reflecting how boost times
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_rect(ctx, boostUIOuter);
    if(carPtr->finished) return;
    
    if(carPtr->boosting) {
        // The whole bar is green. The length reflects the proportion
        //  of the boostDurationMillis that remains
        int boostRemaining = carPtr->boostDurationMillis - (get_milli_time() - carPtr->boostStartMillis);
        if(boostRemaining < 0) boostRemaining = 0;
        int16_t barHeight = ((boostRemaining * 64) / carPtr->maxBoostDurationMillis);
        boostUIInner.size.h = barHeight;
        boostUIInner.origin.y = 117 - barHeight;
        graphics_context_set_fill_color(ctx, GColorSpringBud);    
        graphics_fill_rect(ctx, boostUIInner, 0, 0);
    } else {
        int sinceLastBoost = min((get_milli_time() - carPtr->lastBoostMillis), BOOST_COOLDOWN_MILLIS);
        uint16_t barHeight = (sinceLastBoost * 64) / BOOST_COOLDOWN_MILLIS;
        if((sinceLastBoost < BOOST_MINIMUM_COOLDOWN)) graphics_context_set_fill_color(ctx, GColorBulgarianRose);
        else graphics_context_set_fill_color(ctx, GColorSpringBud);
        boostUIInner.size.h = barHeight;
        boostUIInner.origin.y = 117 - barHeight;
        graphics_fill_rect(ctx, boostUIInner, 0, 0);
    }
}




void car_draw(GContext *ctx, carType *carPtr, carType *playerCar) {
    pge_sprite_draw(carPtr->sprite, ctx);
    if(carPtr == playerCar) {
        // car_update_boost_ui(ctx, carPtr);
        GRect temp;
        temp.origin = carPtr->sprite->position;
        temp.size = GSize(CAR_LENGTH, CAR_WIDTH);
        GRect outlineRect = grect_crop(temp, -1); 
        graphics_context_set_stroke_color(ctx, GColorFromRGB(255, 255, 0));
        graphics_context_set_stroke_width(ctx, 2);
        graphics_draw_rect(ctx, outlineRect);
    }    
}



uint32_t car_calculate_drag(carType *carPtr) {
    uint32_t drag = carPtr->currentSpeed * carPtr->currentSpeed;
    drag = drag >> DRAG_DIVISOR;
    return drag;
}



bool car_check_forward_movement(const carType *carPtr, int amount, uint16_t howManyNPCs, carType *sortedGrid[]) {
    GRect rectUs, rectThem;
    car_make_rect(carPtr, &rectUs);
    rectUs.origin.x += amount;

    int c;
    for(c=0; c <= howManyNPCs; c++) {
        if(sortedGrid[c] == carPtr) break; 
        car_make_rect(sortedGrid[c], &rectThem);
        
        if(pge_collision_rectangle_rectangle(&rectUs, &rectThem)) {
            // A car is in the way; we can't move forward as far as
            //  we want to go
            return false;
        }        
    }
    // If we get here, it means no car is in the way,
    // so we can go forward
    return true;
}



void car_movement(carType *carPtr, uint16_t howManyNPCs, carType *sortedGrid[]) {
    if(carPtr->worldPosition.x > TRACK_FINISH_LINE + 250) {
        return;    
    }
    
    if(carPtr->boosting) {
        carPtr->lastBoostMillis = get_milli_time();
        carPtr->currentSpeed += BOOST_INC;
        // Here we make sure that moving foward at the current speed will not cause
        //  us to crash into a car in front. 
        bool canMove = car_check_forward_movement(carPtr, carPtr->currentSpeed >> 8, howManyNPCs, sortedGrid);
        if(!canMove) {
            carPtr->currentSpeed -= BOOST_INC;
            carPtr->currentSpeed += SPEED_INC;            
        }
        int tempSpeed = carPtr->currentSpeed;
        while(!canMove) {
            tempSpeed -= REAR_END_PENALTY;
            canMove = car_check_forward_movement(carPtr, tempSpeed >> 8, howManyNPCs, sortedGrid);
        }   
        carPtr->worldPosition.x += (tempSpeed >> 8);     
    } else {
        carPtr->currentSpeed += SPEED_INC;
        bool canMove = car_check_forward_movement(carPtr, carPtr->currentSpeed >> 8, howManyNPCs, sortedGrid);
        if(!canMove) {
            carPtr->currentSpeed -= SPEED_INC;
            carPtr->currentSpeed += (SPEED_INC >> 1);            
        }
        int tempSpeed = carPtr->currentSpeed;
        while(!canMove) {
            tempSpeed -= REAR_END_PENALTY;
            canMove = car_check_forward_movement(carPtr, tempSpeed >> 8, howManyNPCs, sortedGrid);
        }   
        carPtr->worldPosition.x += (tempSpeed >> 8);     
    }
    carPtr->currentSpeed -= car_calculate_drag(carPtr);
}



#define BOOST_RANKS (10)
static uint32_t permittedBoostTimes[BOOST_RANKS] = {400, 1250, 2000, 3000, 4000, 4500, 4750, 5000, 5250, 5500};

int car_get_permitted_boost_time(carType *carPtr) {
    uint32_t r = carPtr->rank;
    if(r >= BOOST_RANKS) r = BOOST_RANKS-1;
    
    return permittedBoostTimes[r];
}

void car_switch_on_boost(carType *carPtr, carType *playerCar) {
    // We can't boost if we are currently boosting!
    if(carPtr->boosting) return;
    // How long since we last boosted?
    uint64_t timeSinceBoost = get_milli_time() - carPtr->lastBoostMillis;
    // We must wait a little bit before boosting again!
    if(timeSinceBoost < BOOST_MINIMUM_COOLDOWN) return;
    // This is the MAXIMUM we can have
    time_t maxBoostDuration = permittedBoostTimes[carPtr->rank];
    carPtr->maxBoostDurationMillis = maxBoostDuration;
    uint64_t normalizedTSB = min(timeSinceBoost, BOOST_COOLDOWN_MILLIS);
    uint64_t boostDuration = (maxBoostDuration * normalizedTSB) / BOOST_COOLDOWN_MILLIS;
    carPtr->boostDurationMillis = boostDuration;
    if(carPtr == playerCar) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Rank: %d - boost duration: %d", (int)carPtr->rank+1, (int)boostDuration);
    }
    carPtr->boostStartMillis = get_milli_time();
    carPtr->lastBoostMillis = carPtr->boostStartMillis;
    carPtr->boosting = true;
}


// This checks that we are able to steer to the left or the right
//  (ie it checks to be sure that there is no car in the way)
int car_steer(carType *carPtr, int amount, uint16_t howManyNPCs, carType *sortedGrid[]) {
    GRect rectUs, rectThem;
    car_make_rect(carPtr, &rectUs);
    rectUs.origin.y += amount;

    int c;
    for(c=0; c <= howManyNPCs; c++) {
        if(sortedGrid[c] == carPtr) continue;
        car_make_rect(sortedGrid[c], &rectThem);
        if(pge_collision_rectangle_rectangle(&rectUs, &rectThem)) {
            // A car is in the way; we can't steer in the direction
            //  we want to go
            carPtr->currentSpeed -= CRASH_SLOWDOWN;
            return carPtr->worldPosition.y;
        }        
    }
    // If we get here, it means no car is in the way,
    // so we can steer in the direction we want to go
    carPtr->currentSpeed -= STEER_SLOWDOWN;
    return rectUs.origin.y;
}

// This is only called if carPtr is the player car
void car_handle_buttons(carType *carPtr, carType *playerCar, uint16_t howManyNPCs, carType *startingGrid[]) {
    bool left = pge_get_button_state(BUTTON_ID_UP);
    if(left) {
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "!!! LEFT -> y = %d", carPtr->worldPosition.y);
        if( (carPtr->worldPosition.y) > (IBL+STEER_AMOUNT-TRACK_CENTRE_LINE+(CAR_WIDTH / 2)) ) {
            carPtr->worldPosition.y = car_steer(carPtr, -STEER_AMOUNT, howManyNPCs, startingGrid);
        } else {
            // Penalise scraping the wing!
            carPtr->currentSpeed -= CRASH_SLOWDOWN;
        }
    }
    bool right = pge_get_button_state(BUTTON_ID_DOWN);
    if(right) {
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "!!! RIGHT -> y = %d", carPtr->worldPosition.y);
        if( (carPtr->worldPosition.y) < (TRACK_CENTRE_LINE - BARRIER_WIDTH - STEER_AMOUNT - CAR_WIDTH) ) {
            carPtr->worldPosition.y = car_steer(carPtr, +STEER_AMOUNT, howManyNPCs, startingGrid);
        } else {
            // Penalise scraping the wing!
            carPtr->currentSpeed -= CRASH_SLOWDOWN;
        }
    }
    bool boost = pge_get_button_state(BUTTON_ID_SELECT);
    if(boost) {
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "Switching on boost");
        // This only switches it on if it is possible to do so
        car_switch_on_boost(carPtr, playerCar);
        if(carPtr->boosting == false) {
            // This means we tried to switch on boost but failed
            //  So we "punish"" the driver by pushing back the next boost time
            //APP_LOG(APP_LOG_LEVEL_DEBUG, "PUNISHING BOOST FAILURE!!!");
            //APP_LOG(APP_LOG_LEVEL_DEBUG, "%d : %d : %d", (int)(get_milli_time() - carPtr->lastBoostMillis), (int)BOOST_MINIMUM_COOLDOWN, (int)BOOST_COOLDOWN_MILLIS);
            carPtr->lastBoostMillis = get_milli_time();            
        }
    }
}

// This function develops a "plan" for steering. The idea is that we don't 
//  try to decide what to do every frame, because this leads to "oscillating"
//  back and forth in what the car is trying to do.
// The "plan" is simply a target value for worldPosition.y - if we already have a "plan",
//  we just continue to follow it until it becomes impossible, or until it is already
//  achieved or until it is no longer required.
void car_ai_create_steering_plan(carType *carPtr, carType *sortedGrid[]) {
    if(carPtr->rank != 0) {
        carType *carInFront = sortedGrid[carPtr->rank-1];
        if((carInFront->worldPosition.x - carPtr->worldPosition.x) < (CAR_LENGTH*3)) {
            // We DO have to worry about the car in front
            // Check to see whether we already have a plan
            int diffFront = (carInFront->sprite->position.y - carPtr->sprite->position.y);
            if(abs(diffFront) <= (CAR_WIDTH+PASSING_CLEARANCE)) {
                if(carPtr->steeringPlan == NO_STEERING_PLAN) {
                    if(diffFront < 0) {
                        // Car in front is to our left, so steer right (but not if too close to barrier)
                        int newY = carInFront->worldPosition.y + (CAR_WIDTH + PASSING_CLEARANCE);
                        if(newY < (168 - BARRIER_WIDTH - PASSING_CLEARANCE - CAR_WIDTH)) {
                            carPtr->steeringPlan = newY;
                        } else {
                            carPtr->steeringPlan = carInFront->worldPosition.y - (CAR_WIDTH + PASSING_CLEARANCE);
                        }
                    } else if(diffFront > 0) {
                        // Car in front is to our right, so steer left (but not if too close to barrier)
                        int newY = carInFront->worldPosition.y - (CAR_WIDTH + PASSING_CLEARANCE);
                        if(newY > (0 + BARRIER_WIDTH + PASSING_CLEARANCE)) {
                            carPtr->steeringPlan = newY;
                        } else {
                            carPtr->steeringPlan = carInFront->worldPosition.y + (CAR_WIDTH + PASSING_CLEARANCE);
                        }
                    } else {
                        // We are directly behind, so head towards the centre
                        if(carPtr->worldPosition.y + (CAR_WIDTH/2) <= TRACK_CENTRE_LINE) {
                            carPtr->steeringPlan = carInFront->worldPosition.y + (CAR_WIDTH + PASSING_CLEARANCE);
                        } else {
                            carPtr->steeringPlan = carInFront->worldPosition.y - (CAR_WIDTH + PASSING_CLEARANCE);
                        }
                    }      
                }
            } else {
                // This means the car in front is not actually in our way
                carPtr->steeringPlan = NO_STEERING_PLAN;
            }
        } else { 
            // This means the car in front is too far ahead to worry about
            carPtr->steeringPlan = NO_STEERING_PLAN;
        }
    }
}


void car_ai_execute_steering_plan(carType *carPtr, uint16_t howManyNPCs, carType *startingGrid[]) {
    if(carPtr->steeringPlan == NO_STEERING_PLAN) return;
    if(carPtr->worldPosition.y < carPtr->steeringPlan) {
        int newY = car_steer(carPtr, +STEER_AMOUNT,howManyNPCs, startingGrid);
        if(newY == carPtr->worldPosition.y) {
            // Something stopped us, so ditch the plan
            carPtr->steeringPlan = NO_STEERING_PLAN;   
        } else {
            carPtr->worldPosition.y = newY;
        }
    } else if(carPtr->worldPosition.y > carPtr->steeringPlan) {
        int newY = car_steer(carPtr, -STEER_AMOUNT,howManyNPCs, startingGrid);
        if(newY == carPtr->worldPosition.y) {
            // Something stopped us, so ditch the plan
            carPtr->steeringPlan = NO_STEERING_PLAN;   
        } else {
            carPtr->worldPosition.y = newY;
        }
    } else {
        // We have completed the plan
        carPtr->steeringPlan = NO_STEERING_PLAN;
    }
}


// This is only called if this is NOT the player car
void car_handle_ai(carType *carPtr, carType *playerCar, uint16_t howManyNPCs, carType *sortedGrid[]) {
    car_ai_create_steering_plan(carPtr, sortedGrid);
    car_ai_execute_steering_plan(carPtr, howManyNPCs, sortedGrid);
    // This functionality is for the "per-car" AI
    if(carPtr->rank >= carPtr->aiBoostRank) {
        uint8_t r = rand() & 255;
        if(r <= carPtr->aiBoostChance) {
            car_switch_on_boost(carPtr, playerCar);
        }
    }
}



void car_drive_npc(carType *carPtr, carType *playerCar, uint16_t howManyNPCs, carType *sortedGrid[]) {
    car_handle_ai(carPtr, playerCar, howManyNPCs, sortedGrid);
}

void car_drive_player(carType *carPtr, carType *playerCar, uint16_t howManyNPCs, carType *sortedGrid[]) {
    car_handle_buttons(carPtr, playerCar, howManyNPCs, sortedGrid);
}

void car_check_for_finisher(carType *carPtr, uint64_t raceStartTime) {
    GRect carRect;

    carRect.origin = carPtr->sprite->position;
    carRect.size.w = CAR_WIDTH;
    carRect.size.h = CAR_LENGTH;
    if(car_crossed_line(carRect)) {
        if(carPtr->finished != 0) return;
        int ft = get_milli_time() - raceStartTime;
        carPtr->finished = ft;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "FINISHED: %d : %d - SPEED: %d - POSITION: %d", carPtr->carNumber, ft, (int)carPtr->currentSpeed >> 8, carPtr->worldPosition.y);
   }
}


void car_check_boost_status(carType *carPtr) {
    if(!carPtr->boosting) return;
    carPtr->lastBoostMillis = get_milli_time();
    time_t howLongBoosting = carPtr->lastBoostMillis - carPtr->boostStartMillis;
    if(howLongBoosting >= carPtr->boostDurationMillis) {
        // We are out of boost!
        carPtr->boosting = false;
    }
    
}

static int playerScreenPosX = PLAYERMIDSCREENX;

void car_set_player_screen_pos(int posX) {
    playerScreenPosX = posX;
}

// Where the player car "sits" on the screen depends what position it is in
//  If the player is leading, the player car sits near the top of the screen
//  If the player is in last, the player car sits near the bottom..
//  This gives us more chance to keep the other cars "in view"
int car_player_screen_position(carType *playerCar, uint16_t howManyNPCs, uint64_t raceStartTime) {
    static uint32_t counter = 0; // used to slow down transition in screenPosY
    counter++;
    
    if((counter & 15) == 0) {
        if((get_milli_time() - raceStartTime) < 2200) {
            return PLAYERMIDSCREENX;
        }
        if(playerCar->rank == 0) { // Player is in the lead
            if(playerScreenPosX > PLAYERFIRSTSCREENX) {
                playerScreenPosX -= 1;
            } else if(playerScreenPosX < PLAYERFIRSTSCREENX) {
                playerScreenPosX += 1;
            }
        } else if(playerCar->rank > (howManyNPCs / 2)) { // Oh dear! Player in back half
            if(playerScreenPosX < PLAYERLASTSCREENX) {
                playerScreenPosX += 1;
            } else if(playerScreenPosX > PLAYERLASTSCREENX) {
                playerScreenPosX -= 1;
            }
        } else { // Player is somewhere in the middle of the field...
            if(playerScreenPosX < PLAYERMIDSCREENX) {
                playerScreenPosX += 1;
            } else if(playerScreenPosX > PLAYERMIDSCREENX) {
                playerScreenPosX -= 1;
            }
        }
    }

    return playerScreenPosX;
}




// Your yPos comes from your carType struct
//  But your xPos is defined by your xPos 
//  relative to playerCar 
void car_set_position(carType *carPtr, carType *playerCar, uint16_t howManyNPCs, uint64_t raceStartTime) {
    GPoint screenPosition;
    screenPosition.y = TRACK_CENTRE_LINE + carPtr->worldPosition.y - (CAR_WIDTH / 2);
    screenPosition.x = -(car_get_camera_focus() - carPtr->worldPosition.x) + car_player_screen_position(playerCar, howManyNPCs, raceStartTime);
    carPtr->sprite->position = screenPosition;
}





// This is done at the start of each race it MUST NOT be called before
//  all the cars are added to the grid
void car_reset(carType *carPtr, carType *playerCar, uint16_t howManyNPCs, uint64_t raceStartTime) {
    carPtr->worldPosition = carPtr->startingPosition;
    carPtr->boosting = false;
    carPtr->steeringPlan = NO_STEERING_PLAN;
    carPtr->boostStartMillis = get_milli_time();
    carPtr->lastBoostMillis = get_milli_time();
    carPtr->boostDurationMillis = 0;
    carPtr->rank = 1;
    carPtr->currentSpeed = 0;
    carPtr->finished = 0;
    car_set_position(carPtr, playerCar, howManyNPCs, raceStartTime);
}





// This stuff gets done only once
void car_initialise(carType *carPtr, int resourceID, GColor colour, char *name, uint8_t aiRank, uint8_t aiChance) {
    GPoint pos;
    carPtr->sprite = pge_sprite_create(pos, resourceID);
    strncpy(carPtr->carName, name, 11);
    carPtr->carColour = colour;
    carPtr->aiBoostRank = aiRank; 
    carPtr->aiBoostChance = aiChance;
}




