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

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <pspiofilemgr.h>
#include <pspgu.h>
#include <psputility.h>
#include <png.h>

#include "ya2d_image.h"
#include "ya2d_texture.h"
#include "ya2d_utils.h"

#define YA2D_PNGSIGSIZE   (8)



static void _ya2d_read_png_file_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
    SceUID fd = *(SceUID*) png_get_io_ptr(png_ptr);
    sceIoRead(fd, data, length);
}

static void _ya2d_read_png_buffer_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
    unsigned int* address = png_get_io_ptr(png_ptr);
    memcpy(data, (void*)(*address), length);
    *address += length;
}


static struct ya2d_texture* _ya2d_load_PNG_generic(void* io_ptr, png_rw_ptr read_data_fn, int place)
{

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                 NULL, NULL, NULL);
    if(png_ptr == NULL) {
        goto exit_error;
    }
    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(info_ptr == NULL) {
        goto exit_destroy_read;
    }
    
    png_bytep* row_ptrs = NULL;
    
    if(setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)0);
        if(row_ptrs != NULL) free(row_ptrs);
        goto exit_error;    
    }

    png_set_read_fn(png_ptr, (png_voidp)io_ptr, read_data_fn);
    png_set_sig_bytes(png_ptr, YA2D_PNGSIGSIZE);
    png_read_info(png_ptr, info_ptr);
    
    unsigned int width, height;
    int bit_depth, color_type;
    
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
                 &color_type, NULL, NULL, NULL);
    
    if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8) png_set_expand(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_expand(png_ptr);
    if (bit_depth == 16) png_set_strip_16(png_ptr);
    
    //PSP: 16 or 32 bpp (not 0xRRGGBB)
    if (bit_depth == 8 && color_type == PNG_COLOR_TYPE_RGB) png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
    
    if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
    }
    
    if(bit_depth < 8) png_set_packing(png_ptr);

    png_read_update_info(png_ptr, info_ptr);
    
    row_ptrs = (png_bytep*)malloc(sizeof(png_bytep) * height);
    struct ya2d_texture* texture = ya2d_create_texture(width, height,
                                                GU_PSM_8888, place);
                                                
    texture->has_alpha = 1;
    int i;
    for (i = 0; i < height; ++i) {
        row_ptrs[i] = (png_bytep)(texture->data + i*texture->stride);
    }
    
    png_read_image(png_ptr, row_ptrs);
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)0);
    free(row_ptrs);
    ya2d_flush_texture(texture);
    return texture;
exit_destroy_read:
    png_destroy_read_struct(&png_ptr, (png_infopp)0, (png_infopp)0);
exit_error:
    return NULL;
}


struct ya2d_texture* ya2d_load_PNG_file_offset(const char* filename, int place, SceOff offset)
{
    png_byte pngsig[YA2D_PNGSIGSIZE];
    SceUID fd;
    
    if ((fd = sceIoOpen(filename, PSP_O_RDONLY, 0777)) < 0) {
        goto exit_error;
    }
    
    sceIoLseek(fd, offset, SEEK_SET);
    
    if (sceIoRead(fd, pngsig, YA2D_PNGSIGSIZE) != YA2D_PNGSIGSIZE) {
        goto exit_close;
    }
    
    if (png_sig_cmp(pngsig, 0, YA2D_PNGSIGSIZE) != 0) {
        goto exit_close;
    }
    
    struct ya2d_texture* texture = _ya2d_load_PNG_generic((void*) &fd, _ya2d_read_png_file_fn, place);
    sceIoClose(fd);
    return texture;
    
exit_close:
    sceIoClose(fd);
exit_error:
    return NULL;
}


struct ya2d_texture* ya2d_load_PNG_file(const char* filename, int place)
{
    return ya2d_load_PNG_file_offset(filename, place, 0);
}


struct ya2d_texture* ya2d_load_PNG_buffer(void *buffer, int place)
{
    if(png_sig_cmp((png_byte*) buffer, 0, YA2D_PNGSIGSIZE) != 0) {
        return NULL;
    }
    unsigned int buffer_address = (unsigned int)buffer + YA2D_PNGSIGSIZE;

    return _ya2d_load_PNG_generic((void*) &buffer_address, _ya2d_read_png_buffer_fn, place);    
}
