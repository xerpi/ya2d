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

#ifndef _YA2D_UTILS_H_
#define _YA2D_UTILS_H_

#define YA2D_LOGFILE "ms0:/ya2d_log.txt"

typedef struct
{
	float u, v;
	short x, y, z;
}ya2d_Tex2FVertex3S;

typedef struct
{
	short u, v;
	float x, y, z;
}ya2d_Tex2SVertex3F;

typedef struct
{
	float u, v;
	float x, y, z;
}ya2d_Tex2FVertex3F;

typedef struct
{
	short u, v;
	short x, y, z;
}ya2d_Tex2SVertex3S;

typedef struct
{
	unsigned int color;
	short x, y, z;
}ya2d_Col1UIVertex3S;

void ya2d_log(const char* s, ...);

unsigned int next_pow2(unsigned int v);
void swizzle_fast(unsigned char* out, const unsigned char* in, unsigned int width, unsigned int height);

#endif
