#pragma once

#include <pebble.h>

// BARRIER INFORMATION 

#define BARRIER_LEFT (0)
#define BARRIER_LENGTH (16)
#define BARRIER_WIDTH (8)
#define BARRIER_RIGHT (168 - BARRIER_WIDTH)

#define LINE_LENGTH (32)
#define LINE_WIDTH (4)

#define IBL (BARRIER_LEFT + BARRIER_WIDTH)
#define TRACK_CENTRE_LINE (84)
#define TRACK_START_LINE (74)


#define TRACK_FINISH_LINE (15000)



// GBitmap *finishLine;
// void load_finish_line_bitmap();
//void draw_track(GContext *ctx, int playerPosition);
//void draw_finish_line(GContext *ctx, int playerPosition);
// bool car_crossed_line(GRect carRect);
// void set_up_distance_markers();


