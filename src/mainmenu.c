#include <pebble.h>
#include "statemachine.h"
#include "mainmenu.h"
#include "pebble.h"

#define NUM_MENU_SECTIONS 2
#define NUM_FIRST_MENU_ITEMS 3

static Window *mainMenuWindow;
static SimpleMenuLayer *mainMenuLayer;
SimpleMenuItem mainMenuItems[3];
SimpleMenuSection mainMenuSection;

static void mm_marathon_not_a_sprint(int index, void *ctx) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "MARATHON SELECTED");
    set_current_state(STATE_MARATHON);
    window_stack_pop(true);
}

static void mm_sprint_not_a_marathon(int index, void *ctx) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "SPRINT SELECTED");
    set_current_state(STATE_SPRINT);
    window_stack_pop(true);
}


// window should be gameWindow, passed from horizontalboost.c
Window *main_menu_create() {
    // Unless we hear otherwise, leaving the menu quits the game
    // Check out mm_marathon_not_a_sprint(..) above to see how to prevent this!
    set_current_state(STATE_QUITTING);
    
    mainMenuItems[0].title  = "MARATHON";
    mainMenuItems[0].subtitle = "It's a marathon, not a sprint!";
    mainMenuItems[0].icon = NULL;
    mainMenuItems[0].callback = mm_marathon_not_a_sprint;

    
    mainMenuItems[1].title  = "SPRINT";
    mainMenuItems[1].subtitle = "5 cars, 600m, no compromise.";
    mainMenuItems[1].icon = NULL;
    mainMenuItems[1].callback = mm_sprint_not_a_marathon;

    mainMenuSection.title = "MAIN MENU";
    mainMenuSection.items = mainMenuItems;
    mainMenuSection.num_items = 2;
    
    mainMenuWindow = window_create();
    window_set_window_handlers(mainMenuWindow, (WindowHandlers) {
        .unload = main_menu_destroy,
    });

    Layer *windowLayer = window_get_root_layer(mainMenuWindow);
    GRect bounds = layer_get_frame(windowLayer);

    mainMenuLayer = simple_menu_layer_create(bounds, mainMenuWindow, &mainMenuSection, 1, NULL);

    layer_add_child(windowLayer, simple_menu_layer_get_layer(mainMenuLayer));

    window_stack_push(mainMenuWindow, false);
    
    return mainMenuWindow;
}


void main_menu_destroy() {
    if(mainMenuLayer != NULL) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "hello from main_menu_destroy!!!!!");
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Current state is: %d!!!!", get_current_state());
        simple_menu_layer_destroy(mainMenuLayer);
        layer_remove_from_parent((Layer *)mainMenuLayer);
        window_stack_remove(mainMenuWindow, false);
        window_destroy(mainMenuWindow);
        mainMenuLayer = NULL;
        mainMenuWindow = NULL;
    }
    if(mainMenuWindow != NULL) {
        window_stack_remove(mainMenuWindow, false);
        window_destroy(mainMenuWindow);
        mainMenuLayer = NULL;
        mainMenuWindow = NULL;        
    }
}

/*
void main_menu_show() {
    layer_set_hidden((Layer *)mainMenuLayer, false);
}

void main_menu_hide() {
    layer_set_hidden((Layer *)mainMenuLayer, true);
}
*/


