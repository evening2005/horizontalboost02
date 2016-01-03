/*
#include <pebble.h>
#include "track.h"
#include "pge_sprite.h"
#include "pge_collision.h"



    


void load_finish_line_bitmap() {
    finishLine = gbitmap_create_with_resource(RESOURCE_ID_FINISH_LINE);
}


void draw_barriers(GContext *ctx, int playerPosition) {
    GRect leftBarrier = { {4, 0}, {BARRIER_WIDTH,168} };
    GRect rightBarrier = { {BARRIER_RIGHT, 0} , {BARRIER_WIDTH, 168} };
    // Draw a white "stripe" on the edges of the road
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, leftBarrier, 0, 0);
    graphics_fill_rect(ctx, rightBarrier, 0, 0);
    
    // Now we want to draw red "stripes" on the white lines
    int loopStartY = (playerPosition % (2 * BARRIER_LENGTH));
    graphics_context_set_fill_color(ctx, GColorRed);
    
    int y;
    for(y = (loopStartY - BARRIER_LENGTH); y < 220; y += (BARRIER_LENGTH*2)) {
        GRect leftLine = { {4, y}, {BARRIER_WIDTH, BARRIER_LENGTH}};
        GRect rightLine = { {BARRIER_RIGHT, y}, {BARRIER_WIDTH, BARRIER_LENGTH}};
        graphics_fill_rect(ctx, leftLine, 0, 0);
        graphics_fill_rect(ctx, rightLine, 0, 0);
    }
    
    
}

void draw_lines(GContext *ctx, int playerPosition) {
    int loopStartY = (playerPosition % (2 * LINE_LENGTH));
    int xPos = TRACK_CENTRE_LINE;
    graphics_context_set_fill_color(ctx, GColorYellow);
    
    int y;
    for(y = (loopStartY - LINE_LENGTH); y < 220; y += (LINE_LENGTH*2)) {
        GRect line = { {xPos, y}, {LINE_WIDTH, LINE_LENGTH}};
        graphics_fill_rect(ctx, line, 0, 0);
    }
    
}

static GRect finishLineRect;
void draw_finish_line(GContext *ctx, int cameraFocus) {
    int diff = cameraFocus - TRACK_FINISH_LINE;
    finishLineRect.origin.x = 5;
    finishLineRect.origin.y = (TRACK_START_LINE + diff);
    finishLineRect.size.w = 132;
    finishLineRect.size.h = 120;
    if(abs(diff) <= 500) graphics_draw_bitmap_in_rect(ctx, finishLine, finishLineRect);
}


// This determines how much distance remains
//  It works in 100s, so it is 100, 200, 300 etc
static GRect distanceMarkers[33];
static GFont distFont;

void set_up_distance_markers() {
    distFont = fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS);
    GRect dist = { {8, 0}, {128, 80} };
    
    for(int i=0; i < 33; i++) {
        dist.origin.y = i*100;
        distanceMarkers[i] = dist;
    }
}


 
void draw_remaining_distance(GContext *ctx, int cameraFocus) {
    // 10 pixels == 1 metre
    int howFarToGo = (TRACK_FINISH_LINE - cameraFocus) / 10;
    int diff;
    GRect distanceRect;
    char buf[] = "00000000000";    
    howFarToGo = (howFarToGo / 100);
    int m;
    graphics_context_set_text_color(ctx, GColorSpringBud);
    for(m=(howFarToGo-1); m <= (howFarToGo+1); m++) {
        diff = -(TRACK_FINISH_LINE - cameraFocus) + (m * 1000);
        distanceRect = distanceMarkers[m];
        distanceRect.origin.y = TRACK_START_LINE + diff;

        snprintf(buf, sizeof(buf), "%d", m*100);

        graphics_draw_text(ctx, buf, distFont, distanceRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }
}

bool car_crossed_line(GRect carRect) {
    if(pge_collision_rectangle_rectangle(&finishLineRect, &carRect)) {
        return true;
    } else {
        return false;
    }
}


// The position of the barriers and lines depends on where the car is
void draw_track(GContext *ctx, int cameraFocus) {
    // draw_lines(ctx, cameraFocus);
    draw_remaining_distance(ctx, cameraFocus);
    draw_finish_line(ctx, cameraFocus);
    draw_barriers(ctx, cameraFocus);
}

*/

