/*
* Copyright (C) 2013, xerpi
*/

#include "ya2d_texture.h"
#include "ya2d_utils.h"
#include <pspgu.h>
#include <pspgum.h>
#include <psputils.h>
#include <pspmath.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include "vram.h"


ya2d_Texture *ya2d_create_texture(int width, int height, int psm, int place)
{
	ya2d_Texture *texture = (ya2d_Texture *)malloc(sizeof(ya2d_Texture));

	texture->width  = width;
	texture->height = height;
	texture->psm    = psm;
	texture->pow2_width  = next_pow2(width);
	texture->pow2_height = next_pow2(height);
	texture->center_x = width >> 1;
	texture->center_y = height >> 1;
	texture->swizzled = GU_FALSE;
	
	switch(psm) {
	case GU_PSM_5650:
	case GU_PSM_5551:
	case GU_PSM_4444:
		texture->stride = texture->pow2_width * 2;
		break;
	case GU_PSM_8888:
	default:
		texture->stride = texture->pow2_width * 4;
		break;
	}
	
	texture->data_size = texture->stride * texture->pow2_height;
	
	//If there's not enough space in the VRAM, then allocate it in the RAM
	if ((place == YA2D_PLACE_RAM) || (texture->data_size > vlargestblock())) {
		texture->data = memalign(16, texture->data_size);
		texture->place = YA2D_PLACE_RAM;
	} else {
		texture->data = valloc(texture->data_size);
		texture->place = YA2D_PLACE_VRAM;
	}

	return texture;
}


void ya2d_free_texture(ya2d_Texture *texture)
{
	if (texture->place == YA2D_PLACE_RAM) {
		free(texture->data);
	} else {
		vfree(texture->data);
	}
	free(texture);
}

void ya2d_set_texture(ya2d_Texture *texture)
{
	sceGuTexImage(0, texture->pow2_width, texture->pow2_height,
			  texture->pow2_width, texture->data);	
}

static void _ya2d_draw_texture_slow(int x, int y, ya2d_Texture *texture)
{	
	ya2d_Tex2SVertex3S *vertices = sceGuGetMemory(2 * sizeof(ya2d_Tex2SVertex3S));
	
	vertices[0].u = 0;
	vertices[0].v = 0;
	vertices[0].x = x;
	vertices[0].y = y;
	vertices[0].z = 0;
	
	vertices[1].u = texture->pow2_width;
	vertices[1].v = texture->pow2_height;
	vertices[1].x = x + texture->pow2_width;
	vertices[1].y = y + texture->pow2_height;
	vertices[1].z = 0;
	
	sceGumDrawArray(GU_SPRITES, GU_TEXTURE_16BIT|GU_VERTEX_16BIT|GU_TRANSFORM_2D, 2, 0, vertices);
}

static void _ya2d_draw_texture_fast(int x, int y, ya2d_Texture *texture)
{	
	int i, k, slice, n_slices = texture->width/YA2D_TEXTURE_SLICE;
	if (texture->width%YA2D_TEXTURE_SLICE != 0) ++n_slices;
	ya2d_Tex2SVertex3S *vertices = sceGuGetMemory(2*n_slices * sizeof(ya2d_Tex2SVertex3S));
	for (k = 0, i = 0, slice = 0; i < n_slices; k += 2, ++i, slice += YA2D_TEXTURE_SLICE) {
		vertices[k].u = slice;
		vertices[k].v = 0;
		vertices[k].x = x+slice;
		vertices[k].y = y;
		vertices[k].z = 0;
		
		register int j = k+1;
		vertices[j].u = slice+YA2D_TEXTURE_SLICE;
		vertices[j].v = texture->height;
		vertices[j].x = x+slice+YA2D_TEXTURE_SLICE;
		vertices[j].y = y + texture->height;
		vertices[j].z = 0;
	}
	sceGumDrawArray(GU_SPRITES, GU_TEXTURE_16BIT|GU_VERTEX_16BIT|GU_TRANSFORM_2D, 2*n_slices, 0, vertices);
}

void ya2d_draw_texture(int x, int y, ya2d_Texture *texture)
{
	sceGuEnable(GU_TEXTURE_2D);
	sceGuTexMode(texture->psm, 0, 0, texture->swizzled);
	ya2d_set_texture(texture);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);	
	
	//There's no need to use the fast algorithm with small textures
	if (texture->pow2_width > YA2D_TEXTURE_SLICE) {
		_ya2d_draw_texture_fast(x, y, texture);
	} else {
		_ya2d_draw_texture_slow(x, y, texture);
	}	
}

void ya2d_draw_rotate_texture(int x, int y, float angle, ya2d_Texture *texture)
{

	sceGuEnable(GU_TEXTURE_2D);
	sceGuTexMode(texture->psm, 0, 0, texture->swizzled);
	ya2d_set_texture(texture);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);	
		
	ya2d_Tex2SVertex3S *vertices = sceGuGetMemory(4 * sizeof(ya2d_Tex2SVertex3S));
	
	vertices[0].u = 0;
	vertices[0].v = 0;
	vertices[0].x = -texture->center_x;
	vertices[0].y = -texture->center_y;
	vertices[0].z = 0;
	
	vertices[1].u = texture->width;
	vertices[1].v = 0;
	vertices[1].x = texture->width-texture->center_x;
	vertices[1].y = -texture->center_y;
	vertices[1].z = 0;	

	vertices[2].u = 0;
	vertices[2].v = texture->height;
	vertices[2].x = -texture->center_x;
	vertices[2].y = texture->height-texture->center_y;
	vertices[2].z = 0;
		
	vertices[3].u = texture->width;
	vertices[3].v = texture->height;
	vertices[3].x = vertices[1].x;
	vertices[3].y = vertices[2].y;
	vertices[3].z = 0;
	
	float c = cosf(angle);
	float s = sinf(angle);
	int i;	
	for (i = 0; i < 4; ++i) {  //Rotate
		int x = vertices[i].x;
		int y = vertices[i].y;
		vertices[i].x = x*c - y*s;
		vertices[i].y = x*s + y*c;
	}
	for (i = 0; i < 4; ++i) {  //Translate
		vertices[i].x += x;
		vertices[i].y += y;
	}
	
	sceGumDrawArray(GU_TRIANGLE_STRIP, GU_TEXTURE_16BIT|GU_VERTEX_16BIT|GU_TRANSFORM_2D, 4, 0, vertices);
}

void ya2d_swizzle_texture(ya2d_Texture *texture)
{
	//There's no need to use swizzle with small textures
	if(texture->swizzled || texture->width < YA2D_TEXTURE_SLICE) return;
	
	void *tmp = malloc(texture->data_size);
	swizzle_fast(tmp, texture->data, texture->stride, texture->pow2_height);
	memcpy(texture->data, tmp, texture->data_size);
	free(tmp);
	texture->swizzled = 1;
}
