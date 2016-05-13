// Define.h - Header file for the game difinitions

#ifndef DEFINE_H
#define DEFINE_H

#include <windows.h>

#define GAME_NAME  "Blue Ball"       //game name

#define GAME_WIDTH     800           //game area dimenstions
#define GAME_HEIGHT    600

// these read the keyboard asynchronously
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define WINDOW_WIDTH   GAME_WIDTH+2  //entire window dimenstions (include 
#define WINDOW_HEIGHT  GAME_HEIGHT+52//menu bar, title bar, borders, etc..

#define PADDLE_WIDTH   60            //paddle width
#define PADDLE_HEIGHT  20            //paddle height

#define BLOCK_WIDTH    130           //block width
#define BLOCK_HEIGHT   20            //block height

#define BLOCK_X_START  40
#define BLOCK_Y_START  30

#define BLOCK_X_GAP    20
#define BLOCK_Y_GAP    15

#define NUM_BLOCK_ROWS    6          //number of blocks rows
#define NUM_BLOCK_COLUMNS 5          //number of blocks cloumns

#define PADDLE_START_X    (GAME_WIDTH / 2) - (PADDLE_WIDTH / 2)
#define PADDLE_START_Y    GAME_HEIGHT - 40
                                     // now, define the general clipping rectangle
#define min_clip_x     0
#define max_clip_x     GAME_WIDTH
#define min_clip_y     0
#define max_clip_y     GAME_HEIGHT

#define BALL_WIDTH     5
#define BALL_HEIGHT    5

#define BALL_START_Y   400

#endif