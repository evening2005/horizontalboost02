/**
 * This is the bare minimum to make a looping game with PGE!

#include <pebble.h>
 
#include "pge.h"
#include "car.h"
#include "track.h"
#include "statemachine.h"
#include "raceresult.h"
#include "raceui.h"
#include "gamelight.h"

static Window *gameWindow;

carType blueCar;
carType orangeCar;
carType yellowCar;
carType greenCar;
carType blackCar;
carType blackTruck;
carType orangeTruck;
carType yellowTruck;
carType greenTruck;


static void OLD_game_logic() {
    // Per-frame game logic here
    if(get_current_state() == STATE_BEFORERACE) {
        make_pos_gap_ui(gameWindow);
        make_dashboard_clock(gameWindow);
        race_result_create_position_layers();
        place_cars_on_grid();
        set_race_start_time();
        switch_on_light();
        set_current_state(STATE_RACING);        
    } else if(get_current_state() == STATE_RACING) {
        car_frame_update();
    } else if(get_current_state() == STATE_AFTERRACE) {
        psleep(100); // Trying to save the battery!
    }
}

static void OLD_game_draw(GContext *ctx) {
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    // Per-frame game rendering here
    if(get_current_state() == STATE_RACING) { 
        draw_track(ctx, get_camera_focus());
        draw_cars(ctx);
        update_boost_ui(ctx);
    } else if(get_current_state() == STATE_AFTERRACE) {
        // This actually only gets done once
        race_result_populate_position_layers(ctx);
    }
}

static void OLD_game_click(int buttonID, bool longClick) {
    if(buttonID == BUTTON_ID_SELECT) {
        if(get_current_state() == STATE_AFTERRACE) {
            race_result_destroy_assets();
            set_current_state(STATE_BEFORERACE);
        }
    }
}




void OLD_pge_init() {
    // Load the results backdrop
    load_result_backdrop_and_font();
    // Load the finishing line bitmap
    load_finish_line_bitmap();
    set_up_distance_markers();

    srand(time(NULL));
        
    initialise_car(&blueCar, RESOURCE_ID_BLUE_CAR, GColorCadetBlue, "Player");    
    set_player_car(&blueCar);
    initialise_car(&orangeCar, RESOURCE_ID_ORANGE_CAR, GColorOrange, "Orange");
    car_add_to_grid(&orangeCar);
    initialise_car(&yellowCar, RESOURCE_ID_YELLOW_CAR, GColorYellow, "Yellow");    
    car_add_to_grid(&yellowCar);
    initialise_car(&greenCar, RESOURCE_ID_GREEN_CAR, GColorGreen, "Green");    
    car_add_to_grid(&greenCar);
    initialise_car(&orangeTruck, RESOURCE_ID_ORANGE_TRUCK, GColorOrange, "OrangeT");
    car_add_to_grid(&orangeTruck);
    initialise_car(&yellowTruck, RESOURCE_ID_YELLOW_TRUCK, GColorYellow, "YellowT");    
    car_add_to_grid(&yellowTruck);
    initialise_car(&greenTruck, RESOURCE_ID_GREEN_TRUCK, GColorGreen, "GreenT");    
    car_add_to_grid(&greenTruck);
//    initialise_car(&blackCar, RESOURCE_ID_BLACK_CAR, GColorBlack, "Black");
//    car_add_to_grid(&blackCar);
//    initialise_car(&blackTruck, RESOURCE_ID_BLACK_TRUCK, GColorBlack, "BlackT");
//    car_add_to_grid(&blackTruck);
    set_current_state(STATE_BEFORERACE);
    // Start the game
    pge_begin(GColorDarkGray, game_logic, game_draw, game_click);
    // Keep a Window reference for adding other UI
    gameWindow = pge_get_window();
}

void OLD_pge_deinit() {
    // Finish the game
    delete_cars();
    destroy_pos_gap_ui();
    destroy_dashboard_clock();
    gbitmap_destroy(finishLine);
    destroy_result_backdrop_and_font();
    race_result_destroy_assets();
    race_result_destroy_animations();
    light_off(NULL);
    pge_finish();
}



*/