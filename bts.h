#ifndef BTS_H_INCLUDED
#define BTS_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "sql.h"

/** BTS Settings **/
#define BTS_BOARD_H_MM 340 // in mm
#define BTS_BOARD_W_MM 1392 // in mm
#define BTS_BOARD_OFFSET_MM 10 // in mm
#define BTS_PIXEL_DENSITY 12 // 12pix/mm
#define BTS_BOARD_H_PX (BTS_BOARD_H_MM * BTS_PIXEL_DENSITY) // in px
#define BTS_BOARD_W_PX (BTS_BOARD_W_MM * BTS_PIXEL_DENSITY) // in px
#define BTS_BOARD_OFFSET_PX (BTS_BOARD_OFFSET_MM * BTS_PIXEL_DENSITY) // in px

/** BTS Font **/
#define BTS_FONT_THRESHOLD 75 // in pixels

#define BTS_FONT_BIG "FONT/Avenir-Medium-09.ttf"
#define BTS_FONT_SMALL "FONT/Avenir-Light-07.ttf"

/** BTS Gfx **/
#define BTS_WOOD_BG "GFX/wood_bg.bmp"
#define BTS_CENTERFANCY "GFX/centerFancy.bmp"


/** BTS Window Settings **/
#define BTS_MAX_SCALING_W 10
#define BTS_MAX_SCALING_H 5
#define BTS_MAX_SCALING 7
#define BTS_WIN_W (BTS_BOARD_W_PX / BTS_MAX_SCALING_W)
#define BTS_WIN_H (BTS_BOARD_H_PX / BTS_MAX_SCALING_H)




#endif // BTS_H_INCLUDED
