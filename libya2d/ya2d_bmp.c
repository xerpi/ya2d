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

#include "ya2d_image.h"
#include "ya2d_texture.h"
#include "ya2d_utils.h"


#define YA2D_BMPSIGNATURE (0x4D42)

typedef struct {
    unsigned short  bfType;
    unsigned int    bfSize;
    unsigned short  bfReserved1;
    unsigned short  bfReserved2;
    unsigned int    bfOffBits;
}__attribute__((packed)) BITMAPFILEHEADER;


typedef struct {
    unsigned int    biSize;
    unsigned int    biWidth;
    unsigned int    biHeight;
    unsigned short  biPlanes;
    unsigned short  biBitCount;
    unsigned int    biCompression;
    unsigned int    biSizeImage;
    unsigned int    biXPelsPerMeter;
    unsigned int    biYPelsPerMeter;
    unsigned int    biClrUsed;
    unsigned int    biClrImportant;
}__attribute__((packed)) BITMAPINFOHEADER;

static struct ya2d_texture* _ya2d_load_BMP_generic(BITMAPFILEHEADER* bmp_fh,
                                            BITMAPINFOHEADER* bmp_ih,
                                            void* user_data,
                                            void (*seek_fn)(void* user_data, unsigned int offset),
                                            void (*read_fn)(void* user_data, void* buffer, unsigned int length),
                                            int place)
{
    unsigned int row_size;
    if (bmp_ih->biBitCount == 32) {
        row_size = bmp_ih->biWidth * 4;
    } else if (bmp_ih->biBitCount == 24) {
        row_size = bmp_ih->biWidth * 3;
    } else if (bmp_ih->biBitCount == 16) {
        row_size = bmp_ih->biWidth * 2;
    } else {
        goto exit_error;
    }
    
    if(row_size%4 != 0) {
        row_size += 4-(row_size%4);
    }
    
    struct ya2d_texture* texture = ya2d_create_texture(bmp_ih->biWidth, bmp_ih->biHeight,
                                                GU_PSM_8888, place);
    
    seek_fn(user_data, bmp_fh->bfOffBits);
    
    void *buffer = malloc(row_size);
    unsigned int* tex_ptr, color;
    int i, x, y=bmp_ih->biHeight-1;
    for(i = 0; i < bmp_ih->biHeight; ++i, y=bmp_ih->biHeight-1-i) {
        read_fn(user_data, buffer, row_size);
        tex_ptr = (unsigned int*)(texture->data + y*texture->stride);
        for(x = 0; x < bmp_ih->biWidth; ++x) { 
            if (bmp_ih->biBitCount == 32) { //ABGR8888
                color = *(unsigned int*)(buffer + x*4);
                *tex_ptr = (color&0xFF)<<24 | ((color>>8)&0xFF)<<16 |
                           ((color>>16)&0xFF)<<8 | (color>>24);
            } else if (bmp_ih->biBitCount == 24) { //BGR888
                unsigned char *address = buffer + x*3;
                *tex_ptr =  (*address)<<16 | (*(address+1))<<8 |
                            (*(address+2)) | (0xFF<<24);
            } else if (bmp_ih->biBitCount == 16) { //BGR565
                color = *(unsigned short*)(buffer + x*2);
                unsigned char r = (color&0x1F)*((float)255/31);
                unsigned char g = ((color>>5)&0x3F)*((float)255/63);
                unsigned char b = ((color>>11)&0x1F)*((float)255/31);
                *tex_ptr = ((r<<16) | (g<<8) | b | (0xFF<<24));
            }
            tex_ptr++;
        }
    }
    
    free(buffer);
    ya2d_flush_texture(texture);
    return texture;
    
exit_error:    
    return NULL;
}

static void _ya2d_read_bmp_file_seek_fn(void* user_data, unsigned int offset)
{
    sceIoLseek(*(SceUID*) user_data, offset, SEEK_SET);
}

static void _ya2d_read_bmp_file_read_fn(void* user_data, void* buffer, unsigned int length)
{
    sceIoRead(*(SceUID*) user_data, buffer, length);
}

static void _ya2d_read_bmp_buffer_seek_fn(void* user_data, unsigned int offset)
{
    *(unsigned int*)user_data += offset;
}

static void _ya2d_read_bmp_buffer_read_fn(void* user_data, void* buffer, unsigned int length)
{
    memcpy(buffer, (void*)*(unsigned int*)user_data, length);
    *(unsigned int*)user_data += length;
}

struct ya2d_texture* ya2d_load_BMP_file(const char* filename, int place)
{
    SceUID fd;
    if((fd = sceIoOpen(filename, PSP_O_RDONLY, 0777)) < 0) {
        goto exit_error;
    }
    
    BITMAPFILEHEADER bmp_fh;
    sceIoRead(fd, (void*)&bmp_fh, sizeof(BITMAPFILEHEADER));    
    if(bmp_fh.bfType != YA2D_BMPSIGNATURE) {
        goto exit_close;
    }
    
    BITMAPINFOHEADER bmp_ih;
    sceIoRead(fd, (void*)&bmp_ih, sizeof(BITMAPINFOHEADER));
    
    struct ya2d_texture* texture = _ya2d_load_BMP_generic(&bmp_fh,
                                                       &bmp_ih,
                                                       (void*)&fd,
                                                       _ya2d_read_bmp_file_seek_fn,
                                                       _ya2d_read_bmp_file_read_fn,
                                                       place);
                                                        
    sceIoClose(fd);
    return texture;
    
exit_close:
    sceIoClose(fd);
exit_error:    
    return NULL;
}


struct ya2d_texture* ya2d_load_BMP_buffer(void* buffer, int place)
{
    if(buffer == NULL) {
        goto exit_error;
    }
    
    unsigned int buffer_address = (unsigned int)buffer;
    
    BITMAPFILEHEADER bmp_fh;
    memcpy(&bmp_fh, (void*)buffer_address, sizeof(BITMAPFILEHEADER));
    if(bmp_fh.bfType != YA2D_BMPSIGNATURE) {
        goto exit_error;
    }
    
    BITMAPINFOHEADER bmp_ih;
    memcpy(&bmp_ih, (void*)buffer_address + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
    
    
    struct ya2d_texture* texture = _ya2d_load_BMP_generic(&bmp_fh,
                                                       &bmp_ih,
                                                       (void*)&buffer_address,
                                                       _ya2d_read_bmp_buffer_seek_fn,
                                                       _ya2d_read_bmp_buffer_read_fn,
                                                       place);
    
    return texture;
exit_error:    
    return NULL;    
}
