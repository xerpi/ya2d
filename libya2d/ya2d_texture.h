/*
* Copyright (C) 2013, xerpi
*/

#ifndef _YA2D_TEXTURE_H_
#define _YA2D_TEXTURE_H_


#define YA2D_TEXTURE_SLICE (128)

enum {
	YA2D_PLACE_RAM,
	YA2D_PLACE_VRAM
};

typedef struct {
	int place;                    /** RAM or VRAM **/
	int swizzled;                 /** 1: swizzled, 0: not swizzled **/
	int psm;                      /** Pixel Storage Format **/
	int width, height;            /** Texture width and height (non base 2) **/
	int center_x, center_y;       /** Center of the image (non base 2) **/
	int pow2_width, pow2_height;  /** Texture width and height (base 2) **/
	int stride, data_size;
	void *data;
	
}ya2d_Texture;

ya2d_Texture *ya2d_create_texture(int width, int height, int psm, int place);
void ya2d_free_texture(ya2d_Texture *texture);

void ya2d_set_texture(ya2d_Texture *texture);
void ya2d_draw_texture(int x, int y, ya2d_Texture *texture);
void ya2d_draw_rotate_texture(int x, int y, float angle, ya2d_Texture *texture);

void ya2d_swizzle_texture(ya2d_Texture *texture);

#endif
