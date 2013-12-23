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

#include <pspgu.h>
#include <pspgum.h>
#include "ya2d_utils.h"


void ya2d_draw_pixel(int x, int y, unsigned int color)
{
	sceGuDisable(GU_TEXTURE_2D);
	ya2d_Col1UIVertex3S *vertices = sceGuGetMemory(sizeof(ya2d_Col1UIVertex3S)); 
	vertices->color = color;
	vertices->x = x;
	vertices->y = y;
	vertices->z = 0;
	
	sceGumDrawArray(GU_POINTS, GU_COLOR_8888|GU_VERTEX_16BIT|GU_TRANSFORM_2D, 1, 0, vertices);
}

void ya2d_draw_line(int x0, int y0, int x1, int y1, unsigned int color)
{
	sceGuDisable(GU_TEXTURE_2D);
	ya2d_Col1UIVertex3S *vertices = sceGuGetMemory(2 * sizeof(ya2d_Col1UIVertex3S)); 
	vertices[0].color = color;
	vertices[0].x = x0;
	vertices[0].y = y0;
	vertices[0].z = 0;
	
	vertices[1].color = color;
	vertices[1].x = x1;
	vertices[1].y = y1;
	vertices[1].z = 0;
	sceGumDrawArray(GU_LINES, GU_COLOR_8888|GU_VERTEX_16BIT|GU_TRANSFORM_2D, 2, 0, vertices);	
}

void ya2d_draw_rect(int x, int y, int w, int h, unsigned int color)
{
	sceGuDisable(GU_TEXTURE_2D);
	ya2d_Col1UIVertex3S *vertices = sceGuGetMemory(5 * sizeof(ya2d_Col1UIVertex3S)); 
	vertices[0].color = color;
	vertices[0].x = x;
	vertices[0].y = y;
	vertices[0].z = 0;
	
	vertices[1].color = color;
	vertices[1].x = x+w;
	vertices[1].y = y;
	vertices[1].z = 0;
	
	vertices[2].color = color;
	vertices[2].x = x+w;
	vertices[2].y = y+h;
	vertices[2].z = 0;

	vertices[3].color = color;
	vertices[3].x = x;
	vertices[3].y = y+h;
	vertices[3].z = 0;
	
	vertices[4].color = color;
	vertices[4].x = x;
	vertices[4].y = y;
	vertices[4].z = 0;
		
	sceGumDrawArray(GU_LINE_STRIP, GU_COLOR_8888|GU_VERTEX_16BIT|GU_TRANSFORM_2D, 5, 0, vertices);		
}

void ya2d_draw_fillrect(int x, int y, int w, int h, unsigned int color)
{
	sceGuDisable(GU_TEXTURE_2D);
	ya2d_Col1UIVertex3S *vertices = sceGuGetMemory(4 * sizeof(ya2d_Col1UIVertex3S)); 
	vertices[0].color = color;
	vertices[0].x = x;
	vertices[0].y = y;
	vertices[0].z = 0;
	
	vertices[1].color = color;
	vertices[1].x = x+w;
	vertices[1].y = y;
	vertices[1].z = 0;
	
	vertices[2].color = color;
	vertices[2].x = x;
	vertices[2].y = y+h;
	vertices[2].z = 0;
	
	vertices[3].color = color;
	vertices[3].x = x+w;
	vertices[3].y = y+h;
	vertices[3].z = 0;
		
	sceGumDrawArray(GU_TRIANGLE_STRIP, GU_COLOR_8888|GU_VERTEX_16BIT|GU_TRANSFORM_2D, 4, 0, vertices);			
}
