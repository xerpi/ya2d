/*  
  
    libya2d
    Copyright (C) 2013  Sergi (xerpi) Granell (xerpi.g.12@gmail.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
    int center_x, center_y;       /** Center of the image (draw and rotation) **/
    int pow2_width, pow2_height;  /** Texture width and height (base 2) **/
    int stride, data_size;
    void *data;
    
}ya2d_Texture;

ya2d_Texture *ya2d_create_texture(int width, int height, int psm, int place);
void ya2d_free_texture(ya2d_Texture *texture);

void ya2d_set_texture(ya2d_Texture *texture);
void ya2d_draw_texture(int x, int y, ya2d_Texture *texture, int centered);
void ya2d_draw_rotate_texture(int x, int y, float angle, ya2d_Texture *texture);
void ya2d_center_texture(ya2d_Texture *texture);
void ya2d_set_texture_center(int center_x, int center_y, ya2d_Texture *texture);

void ya2d_swizzle_texture(ya2d_Texture *texture);

#endif
