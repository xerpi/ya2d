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

#ifdef USE_PSP_JPEG
#include <pspav/jpeg.h>
#else
#include <jpeglib.h>
#endif

#ifdef USE_PSP_JPEG

struct ya2d_texture* ya2d_load_JPEG_buffer(void* jpegbuf, unsigned long jpeg_size, int place){
    int w = 0, h = 0;
    struct ya2d_texture *texture = NULL;

    // get JPEG info
    pspavGetJpegInfo(jpegbuf, jpeg_size, &w, &h);
    
    // create texture based on gathered info
    texture = ya2d_create_texture(w, h, GU_PSM_8888, place);

    // load jpeg
    int res = pspavLoadJpegBuffer(jpegbuf, jpeg_size, w, h, texture->pow2_w, texture->pow2_h, texture->data, texture->stride);

    if (res < 0){
        ya2d_free_texture(texture);
        return NULL;
    }

    // finish and return
    texture->has_alpha = 0;
    ya2d_flush_texture(texture);
    return texture;
}

struct ya2d_texture* ya2d_load_JPEG_file(const char* filename, int place){
    SceUID fd = sceIoOpen(filename, PSP_O_RDONLY, 0777);
    size_t filesize = sceIoLseek(fd, 0, PSP_SEEK_END);
    sceIoLseek(fd, 0, PSP_SEEK_SET);

    void* buf = malloc(filesize);
    if (!buf) return NULL;

    sceIoRead(fd, buf, filesize);
    struct ya2d_texture* res = ya2d_load_JPEG_buffer(buf, filesize, place);

    sceIoClose(fd);
    free(buf);
    return res;
}

#else

static struct ya2d_texture* _ya2d_load_JPEG_generic(struct jpeg_decompress_struct* jinfo, struct jpeg_error_mgr* jerr, int place)
{
    int row_bytes;
    switch (jinfo->out_color_space) {
    case JCS_RGB:
        row_bytes = jinfo->image_width * 3;
        break;
    default:
        goto exit_error;
    }
    
    struct ya2d_texture *texture = ya2d_create_texture(jinfo->image_width,
                                                jinfo->image_height,
                                                GU_PSM_8888, place);
    JSAMPARRAY buffer = (JSAMPARRAY)malloc(sizeof(JSAMPROW));
    buffer[0] = (JSAMPROW)malloc(sizeof(JSAMPLE) * row_bytes);
    
    unsigned int i, color, *tex_ptr;
    unsigned char* jpeg_ptr;
    void* row_ptr = texture->data;
    jpeg_start_decompress(jinfo);
    
    while (jinfo->output_scanline < jinfo->output_height) {
        jpeg_read_scanlines(jinfo, buffer, 1);
        tex_ptr = (row_ptr += texture->stride);
        for (i = 0, jpeg_ptr = buffer[0]; i < jinfo->output_width; ++i) {
            color = *(jpeg_ptr++);
            color |= *(jpeg_ptr++)<<8;
            color |= *(jpeg_ptr++)<<16;
            *(tex_ptr++) = color | 0xFF000000; 
        }
    }

    
    free(buffer[0]);
    free(buffer);
    ya2d_flush_texture(texture);
    return texture;
exit_error:    
    return NULL;
}

struct ya2d_texture* ya2d_load_JPEG_file(const char* filename, int place)
{
    FILE *fd;
    if ((fd = fopen(filename, "rb")) < 0) {
        fclose(fd);
        return NULL;
    }
    
    struct jpeg_decompress_struct jinfo;
    struct jpeg_error_mgr jerr;
    jinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&jinfo);
    jpeg_stdio_src(&jinfo, fd);
    jpeg_read_header(&jinfo, 1);
    
    struct ya2d_texture* texture = _ya2d_load_JPEG_generic(&jinfo, &jerr, place);
    
    jpeg_finish_decompress(&jinfo);
    jpeg_destroy_decompress(&jinfo);
    
    fclose(fd);
    return texture;
}

struct ya2d_texture* ya2d_load_JPEG_buffer(void* buffer, unsigned long buffer_size, int place)
{
    if (buffer == NULL) {
        return NULL;
    }
    
    struct jpeg_decompress_struct jinfo;
    struct jpeg_error_mgr jerr;
    jinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&jinfo);
    jpeg_mem_src(&jinfo, buffer, buffer_size);
    jpeg_read_header(&jinfo, 1);
    
    struct ya2d_texture* texture = _ya2d_load_JPEG_generic(&jinfo, &jerr, place);
    
    jpeg_finish_decompress(&jinfo);
    jpeg_destroy_decompress(&jinfo);
    
    return texture;
}

#endif