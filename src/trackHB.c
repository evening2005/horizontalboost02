#include <pebble.h>
#include "trackHB.h"
#include "pge_sprite.h"
#include "pge_collision.h"

static int16_t trackFinishLine = TRACK_FINISH_LINE;

void track_set_length(int16_t length) {
    trackFinishLine = length;
}

int16_t track_get_length() {
    return trackFinishLine;
}

static GRect gridPositions[GRID_SIZE];

#define FRONTX (-20)
#define SECONDX (-60)
#define THIRDX (-100)
#define OUTL (-64)
#define INL  (-32)
#define MID  (0)
#define OUTR (64)
#define INR  (32)

void set_up_grid_positions() {
    gridPositions[0] = GRect(FRONTX,  MID, 32, 20);
    gridPositions[1] = GRect(FRONTX,  INL, 32, 20);
    gridPositions[2] = GRect(FRONTX,  INR, 32, 20);
    gridPositions[3] = GRect(FRONTX, OUTL, 32, 20);
    gridPositions[4] = GRect(FRONTX, OUTR, 32, 20);

    gridPositions[5] = GRect(SECONDX,  MID, 32, 20);
    gridPositions[6] = GRect(SECONDX,  INL, 32, 20);
    gridPositions[7] = GRect(SECONDX,  INR, 32, 20);
    gridPositions[8] = GRect(SECONDX, OUTL, 32, 20);
    gridPositions[9] = GRect(SECONDX, OUTR, 32, 20);

    gridPositions[10] = GRect(THIRDX,  MID, 32, 20);
    gridPositions[11] = GRect(THIRDX,  INL, 32, 20);
    gridPositions[12] = GRect(THIRDX,  INR, 32, 20);
    gridPositions[13] = GRect(THIRDX, OUTL, 32, 20);
    gridPositions[14] = GRect(THIRDX, OUTR, 32, 20);
}


// howMany is how many NPCs there are in the race; 
//  we have to add 1 (inside the loop) to account for the playerCar
void shuffle_grid_positions(int howMany) {
    uint32_t i;
    uint32_t r1, r2;
    GRect temp;
    for(i = 0; i < 50; i++) {
        r1 = rand() % (howMany+1);
        r2 = rand() % (howMany+1);
        temp = gridPositions[r1];
        gridPositions[r1] = gridPositions[r2];
        gridPositions[r2] = temp;
    }
}

GRect get_grid_position(int whichOne) {
    return gridPositions[whichOne];
}


static GBitmap *kerbLeft;
static GBitmap *kerbRight;
static GBitmap *finishLine;

void load_kerb_bitmaps() {
    kerbLeft = gbitmap_create_with_resource(RESOURCE_ID_HB_LEFT_KERB);
    kerbRight = gbitmap_create_with_resource(RESOURCE_ID_HB_RIGHT_KERB);    
}


void destroy_kerb_bitmaps() {
    gbitmap_destroy(kerbLeft);
    gbitmap_destroy(kerbRight);
}

static GRect absoluteFinishLineRect;

void load_finish_line_bitmap() {
    finishLine = gbitmap_create_with_resource(RESOURCE_ID_FINISH_LINE);
    absoluteFinishLineRect.origin.x = trackFinishLine;
    absoluteFinishLineRect.origin.y = 0;
    absoluteFinishLineRect.size.w = 150;
    absoluteFinishLineRect.size.h = 168;
}

void destroy_finish_line_bitmap() {
    gbitmap_destroy(finishLine);
}


static GRect finishLineRect;
void draw_finish_line(GContext *ctx, int cameraFocus) {
    int diff = cameraFocus - trackFinishLine;
    finishLineRect.origin.x = (TRACK_START_LINE - diff - 80);
    finishLineRect.origin.y = 0;
    finishLineRect.size.w = 150;
    finishLineRect.size.h = 168;
    if(abs(diff) <= 500) graphics_draw_bitmap_in_rect(ctx, finishLine, finishLineRect);
}




bool car_crossed_line(int xPos) {
    if(xPos >= trackFinishLine) {
        return true;
    } else {
        return false;
    }
}


// This determines how much distance remains
//  It works in 100s, so it is 100, 200, 300 etc
static GRect distanceMarkers[33];
static GFont distFont;

void set_up_distance_markers() {
    distFont = fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS);
    GRect dist = { {8, 64}, {140, 80} };
    
    for(int i=0; i < 33; i++) {
        dist.origin.x = i*100;
        distanceMarkers[i] = dist;
    }
}


static char distBuf[] = "0000000000000";    

void draw_remaining_distance(GContext *ctx, int cameraFocus) {
    // 10 pixels == 1 metre
    int howFarToGo = (trackFinishLine - cameraFocus) / 10;
    int diff;
    GRect distanceRect;
    howFarToGo = (howFarToGo / 100);
    int m;
    graphics_context_set_text_color(ctx, GColorBlack);
    for(m=(howFarToGo-1); m <= (howFarToGo+1); m++) {
        diff = (m * 1000) - (trackFinishLine - cameraFocus);
        distanceRect = distanceMarkers[m];
        distanceRect.origin.x = TRACK_START_LINE - diff;

        snprintf(distBuf, sizeof(distBuf), "%d", m*100);

        graphics_draw_text(ctx, distBuf, distFont, distanceRect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }
}

GRect kerbLeftRect = { {0, 0}, {180, 8} };
GRect kerbRightRect = { {0, 160}, {180, 8} };
GRect WHOLE_SCREEN = { {0,0}, {144,168}};

void draw_road_surface(GContext *ctx) {
    graphics_context_set_fill_color(ctx, GColorLightGray);
    graphics_fill_rect(ctx, WHOLE_SCREEN, 0, GCornerNone);
}

void draw_kerbs(GContext *ctx, int cameraFocus) {
    int xPos = -(cameraFocus & 31);
    kerbLeftRect.origin.x = xPos;
    kerbRightRect.origin.x = xPos;
    graphics_draw_bitmap_in_rect(ctx, kerbLeft, kerbLeftRect);
    graphics_draw_bitmap_in_rect(ctx, kerbRight, kerbRightRect);
}

void draw_track(GContext *ctx, int cameraFocus) {
    draw_road_surface(ctx);
    draw_remaining_distance(ctx, cameraFocus);
    draw_finish_line(ctx, cameraFocus);
    draw_kerbs(ctx, cameraFocus);
}


