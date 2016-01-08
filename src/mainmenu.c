#include <pebble.h>
#include "statemachine.h"
#include "mainmenu.h"
#include "pebble.h"

#define NUM_MENU_SECTIONS 2
#define NUM_FIRST_MENU_ITEMS 3

static Window *mainMenuWindow;
static SimpleMenuLayer *mainMenu_layer;
SimpleMenuItem mainMenuItems[3];
SimpleMenuSection mainMenuSection;

static void marathon_not_a_sprint(int index, void *ctx) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "MARATHON SELECTED");
    window_stack_pop(false);
    set_current_state(STATE_MARATHON);
}

static void sprint_not_a_marathon(int index, void *ctx) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "SPRINT SELECTED");
    window_stack_pop(false);
    set_current_state(STATE_SPRINT);
}



static void main_window_load(Window *window) {
    simple_menu_layer_destroy(mainMenu_layer);

    mainMenuItems[0].title  = "MARATHON";
    mainMenuItems[0].subtitle = "It's a marathon, not a sprint!";
    mainMenuItems[0].icon = NULL;
    mainMenuItems[0].callback = marathon_not_a_sprint;

    
    mainMenuItems[1].title  = "SPRINT";
    mainMenuItems[1].subtitle = "5 cars, 600m, no compromise.";
    mainMenuItems[1].icon = NULL;
    mainMenuItems[1].callback = sprint_not_a_marathon;

    mainMenuSection.title = "MAIN MENU";
    mainMenuSection.items = mainMenuItems;
    mainMenuSection.num_items = 2;
    Layer *windowLayer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(windowLayer);

    mainMenu_layer = simple_menu_layer_create(bounds, window, &mainMenuSection, 1, NULL);

    layer_add_child(windowLayer, simple_menu_layer_get_layer(mainMenu_layer));
    set_current_state(STATE_SELECTFUNCTION);
}


void mainmenu_destroy() {
    simple_menu_layer_destroy(mainMenu_layer);
    window_destroy(mainMenuWindow);
}

static void main_window_unload(Window *window) {
}

void mainmenu_create() {
    mainMenuWindow = window_create();
    window_set_window_handlers(mainMenuWindow, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload,
    });
    
}


void mainmenu_show() {
    window_stack_push(mainMenuWindow, true);
}
