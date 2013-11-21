/*
* Copyright (C) 2013, xerpi
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


void ya2d_log(const char* s, ...);

unsigned int next_pow2(unsigned int v);
void swizzle_fast(unsigned char* out, const unsigned char* in, unsigned int width, unsigned int height);

#endif
