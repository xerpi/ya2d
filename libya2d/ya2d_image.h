/*
* Copyright (C) 2013, xerpi
*/

#ifndef _YA2D_IMAGE_H_
#define _YA2D_IMAGE_H_

#include "ya2d_texture.h"

#define YA2D_PNGSIGSIZE   (8)
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


ya2d_Texture* ya2d_load_PNG_file(const char* filename, int place);
ya2d_Texture* ya2d_load_PNG_buffer(void* buffer, int place);

ya2d_Texture* ya2d_load_BMP_file(const char* filename, int place);
ya2d_Texture* ya2d_load_BMP_buffer(void* buffer, int place);

ya2d_Texture* ya2d_load_JPEG_file(const char* filename, int place);
ya2d_Texture* ya2d_load_JPEG_buffer(void* buffer, unsigned long buffer_size, int place);


#endif
