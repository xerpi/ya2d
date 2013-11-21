/*
* Copyright (C) 2013, xerpi
*/

#ifndef _YA2D_MAIN_H_
#define _YA2D_MAIN_H_

#define BUF_WIDTH  (512)
#define SCR_WIDTH  (480)
#define SCR_HEIGHT (272)

#define uS_PER_SEC (1000000)

#define YA2D_GU_LIST_SIZE 262144


int ya2d_init();
int ya2d_shutdown();

void ya2d_start();
void ya2d_finish();
void ya2d_swapbuffers();

void ya2d_set_clear_color(unsigned int color);
void *ya2d_get_drawbuffer();
float ya2d_get_fps();



#endif
