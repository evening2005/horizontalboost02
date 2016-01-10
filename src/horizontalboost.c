/**
 * Horizontal Boost, version 2
*/

#include <pebble.h>
 
#include "pge.h"
#include "race.h"
#include "statemachine.h"
#include "gamelight.h"
#include "raceresult.h"
#include "mainmenu.h"
#include "trackHB.h"

static uint8_t DIFFICULTY = 5;

static Window *gameWindow;

static void game_logic() {
    // Per-frame game logic here
    if(get_current_state() == STATE_SHOWMAINMENU) {
        main_menu_create();
    } else if(get_current_state() == STATE_MARATHON) {
        race_reset_cars();
        race_set_difficulty(DIFFICULTY);
        track_set_length(25000);
        main_menu_destroy();
        set_current_state(STATE_BEFORERACE);
    } else if(get_current_state() == STATE_SPRINT) {
        race_reset_cars();
        race_set_difficulty(DIFFICULTY);
        track_set_length(6000);        
        main_menu_destroy();
        set_current_state(STATE_BEFORERACE);
    } else if(get_current_state() == STATE_BEFORERACE) {
        race_result_create_position_layers();
        race_place_cars_on_grid();
        race_set_start_time();
        switch_on_light();
        set_current_state(STATE_RACING);
    } else if(get_current_state() == STATE_RACING) {
        race_frame_update();
    } else if(get_current_state() == STATE_AFTERRESULTS) {
        psleep(100); // Trying to save the battery!
    } else if(get_current_state() == STATE_QUITTING) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "!!!   QUITTING from horizontalboost !!!!");        
        window_stack_pop(false); 
    }
}

static void game_draw(GContext *ctx) {
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    // Per-frame game rendering here
    if(get_current_state() == STATE_RACING) {
        draw_track(ctx, car_get_camera_focus());
        race_draw_cars(ctx);        
    } else if(get_current_state() == STATE_RESULTS) {
        // This actually only gets done once
        race_result_populate_position_layers(ctx);
    }

}

static void game_click(int buttonID, bool longClick) {
    if(buttonID == BUTTON_ID_SELECT) {
        if(get_current_state() == STATE_RESULTS) {
            // We really don't want to do anything with it...
        } else if(get_current_state() == STATE_AFTERRESULTS) {
            race_result_destroy_assets();
            set_current_state(STATE_SHOWMAINMENU);
        }
    } else if(buttonID == BUTTON_ID_BACK) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "!!!   hello BACK pressed !!!! (%d)", get_current_state());
        switch(get_current_state()) {
            case STATE_RESULTS:
            case STATE_QUITTING:
                    break;
            case STATE_AFTERRESULTS:
                    race_result_destroy_assets();
                    set_current_state(STATE_SHOWMAINMENU);
                    break;
            default:
                    set_current_state(STATE_SHOWMAINMENU); 
                    break;
        }
    }
}


static unsigned int fpsOutputFrequency = 5000;
static AppTimer *fpsTimer;

void show_fps(void *data) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "FPS : %d", pge_get_average_framerate());
    fpsTimer = app_timer_register(fpsOutputFrequency, (AppTimerCallback)show_fps, NULL);
}


void pge_init() {
    fpsTimer = app_timer_register(fpsOutputFrequency, (AppTimerCallback)show_fps, NULL);

    //srand(get_milli_time());
    load_kerb_bitmaps();
    // Load the results backdrop
    load_results_title_fonts();
    
    // Load the finishing line bitmap
    load_finish_line_bitmap();
    set_up_distance_markers();
    
    // The number represents the difficulty (range 0-9)
    race_create_cars();
    race_set_difficulty(DIFFICULTY);
    // Start the game
    // Keep a Window reference for adding other UI
    pge_begin(GColorBrass, game_logic, game_draw, game_click);
    gameWindow = pge_get_window();

    set_current_state(STATE_SHOWMAINMENU);
}

void pge_deinit() {
    // Finish the game
    race_result_destroy_assets();
    destroy_results_title_fonts();
    race_result_destroy_animations();
    race_delete_cars();
    destroy_kerb_bitmaps();
    destroy_finish_line_bitmap();
    light_off(NULL);
    main_menu_destroy();
    pge_finish();
}

