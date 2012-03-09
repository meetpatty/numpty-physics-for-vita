/*
 * This file is part of NumptyPhysics
 * Copyright (C) 2008 Tim Edmonds
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */
/*
* PSP port by rock88: rock88a@gmail.com
* http://rock88dev.blogspot.com
*/

#include <stdio.h>
#include "SDL_Lite.h"
#include <pspctrl.h>

int SDL_PollEvent(SDL_Event *event)
{
	return -1;
}

int SDL_PushEvent(SDL_Event *event)
{
	//printf("Fake: SDL_PushEvent!!\n");
	return 0;
}

static struct timeval start;

void SDL_StartTicks(void)
{
	//gettimeofday(&start, NULL);
}

Uint32 SDL_GetTicks()
{
#if 0
	struct timeval now;
	Uint32 ticks;

	gettimeofday(&now, NULL);
	ticks=(now.tv_sec-start.tv_sec)*1000+(now.tv_usec-start.tv_usec)/1000;
	return(ticks);
#else
	return 1000;
#endif
}

#define SCREEN_PITCH 	(512*2)
#define SCREEN_W		(512)
#define SCREEN_H		(272)
#define SCREEN_BITS		(16)
#define SCREEN_BYTES	(2)

int SDL_FillRect(char *dst, SDL_Rect *dstrect, Uint16 color)
{
	int x1 = dstrect->x;
	int y1 = dstrect->y;
	int x2 = x1 + dstrect->w;
	int y2 = y1 + dstrect->h;

	if(x1<0) x1=0;
	if(y1<0) y1=0;
	if(x1>511) x1 = 511;
	if(y1>271) y1 = 271;
	if(x2<0) x2=0;
	if(y2<0) y2=0;
	if(x2>511) x2 = 511;
	if(y2>271) y2 = 271;
	Sint16 x, y;
	Uint16 *row, *pixel;
	
	for (y = y1; y <= y2; y++)
	{
#if 1
		row = (Uint16*)dst + y * SCREEN_PITCH / 2;
		for (x = x1; x <= x2; x++)
		{
			pixel = row + x;
			*pixel = color;
		}
#else
		row = (Uint16*)dst + y * SCREEN_PITCH / 2;
		memset(row,0,dstrect->w*2);
#endif
	}
}

unsigned short Color8888To5650(unsigned int Color32b)
{
	unsigned int Color32 = Color32b;// = 0xFF007FFF;
	unsigned char Red = (Color32);
	unsigned short Red16 = Red>>3;
	unsigned char Green = (Color32>>8);
	unsigned short Green16 = (Green>>2)<<5;
	unsigned char Blue = (Color32>>16);
	unsigned short Blue16 = (Blue>>3)<<11;
	//unsigned char Alpha = (Color32>>24);
	unsigned short Color16 = Red16 | Green16 | Blue16;
	return Color16;
}

int SDL_BlitSurface(char *dst, SDL_Rect *dstrect, char *src, SDL_Rect *srcrect)
{
	int x1 = dstrect->x;
	int y1 = dstrect->y;
	int x2 = x1 + dstrect->w;
	int y2 = y1 + dstrect->h;
	
	if(x1<0) x1=0;
	if(y1<0) y1=0;
	if(x1>511) x1 = 511;
	if(y1>271) y1 = 271;
	if(x2<0) x2=0;
	if(y2<0) y2=0;
	if(x2>511) x2 = 511;
	if(y2>271) y2 = 271;
	Sint16 x, y;
	Uint16 *row, *row2, *pixel, *pixel2;
	
	for (y = y1; y <= y2; y++)
	{
		row = (Uint16*)dst + y * SCREEN_PITCH / 2;
		row2 = (Uint16*)src + y * SCREEN_PITCH / 2;
		for (x = x1; x <= x2; x++)
		{
			pixel = row + x;
			pixel2 = row2 +x;
			*pixel2 = *pixel;
		}
	}
}
