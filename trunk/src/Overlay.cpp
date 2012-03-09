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

#include "Overlay.h"

extern Uint8 keys[20];

#define CROSS		9

Overlay::Overlay(GameParams& game, int x, int y, int w, int h):m_game(game),m_x(x),m_y(y),m_canvas(NULL),m_dragging(false),m_buttonDown(false)
{
	m_w = w;
	m_h = h;
}

Overlay::~Overlay()
{

}

Rect Overlay::dirtyArea() 
{
	return Rect(m_x,m_y,m_x+m_canvas->width()-1,m_y+m_canvas->height()-1);
}

void Overlay::onShow()
{

}

void Overlay::onHide()
{
	m_dragging = false;
	m_buttonDown = false;
	keys[CROSS]=0;
}

void Overlay::onTick(int tick)
{

}

void Overlay::draw(Canvas* screen)
{
	if (m_canvas) screen->drawImage(m_canvas, m_x, m_y);
}

bool Overlay::handleEvent(SceCtrlData &pad, int *x, int *y)
{
	int k = 0;
	int n_x = *x;
	int n_y = *y;
	float c_x = n_x;
	float c_y = n_y;
	//DEBUG(__FILE__,__FUNCTION__,__LINE__);
	if((keys[CROSS]==0)&&(pad.Buttons & PSP_CTRL_CROSS))
	{
		//if((n_x >= m_x)&&(n_x <= m_x + m_canvas->width())&&(n_y >= m_y)&&(n_y <= m_y + m_canvas->height()))
		if((n_x >= m_x)&&(n_x <= m_x + m_w)&&(n_y >= m_y)&&(n_y <= m_y + m_h))
		{
			m_orgx = n_x;
			m_orgy = n_y;
			m_prevx = n_x;
			m_prevy = n_y;
			m_buttonDown = true;
			keys[CROSS]=2;
			return true;		
		}
	}
	
	if((!(pad.Buttons & PSP_CTRL_CROSS))&&(keys[CROSS]==2))
	{
		keys[CROSS]=0;
		if (m_dragging)
		{
			m_dragging = false;
		} 
		else
		{
			if (ABS(n_x-m_orgx) < CLICK_TOLERANCE && ABS(n_y-m_orgy) < CLICK_TOLERANCE)
			{
				onClick(m_orgx-m_x, m_orgy-m_y);
			}
		}
		m_buttonDown = false;	
	}
	
	if(pad.Buttons & PSP_CTRL_UP)
	{
		n_y--;
		if (n_y < 0) n_y = 0;
		k=1;
	}
	if(pad.Buttons & PSP_CTRL_DOWN)
	{
		n_y++;
		if (n_y > 271) n_y = 271;
		k=1;
	}
	if(pad.Buttons & PSP_CTRL_LEFT)
	{
		n_x--;
		if (n_x < 0) n_x = 0;
		k=1;
	}
	if(pad.Buttons & PSP_CTRL_RIGHT)
	{
		n_x++;
		if (n_x > 480) n_x = 480;
		k=1;
	}

	if (pad.Lx<STICK_MIN)
	{
		float f = CUR_MINf;
		if (pad.Lx<STICK_MID) f=CUR_MIDf;
		if (pad.Lx<STICK_MAX) f=CUR_MAXf;
		c_x-=f;
		if (c_x < 0) c_x = 0;
		n_x=round(c_x);
		k=1;
	}
	if (pad.Lx>(256-STICK_MIN))
	{
		float f = CUR_MINf;
		if (pad.Lx>(256-STICK_MID)) f=CUR_MIDf;
		if (pad.Lx>(256-STICK_MAX)) f=CUR_MAXf;
		c_x+=f;
		if (c_x > 480) c_x = 480;
		n_x=round(c_x);
		k=1;
	}
	if (pad.Ly<STICK_MIN)
	{
		float f = CUR_MINf;
		if (pad.Ly<STICK_MID) f=CUR_MIDf;
		if (pad.Ly<STICK_MAX) f=CUR_MAXf;
		c_y-=f;
		if (c_y < 0) c_y = 0;
		n_y=round(c_y);
		k=1;
	}
	if (pad.Ly>(256-STICK_MIN))
	{
		float f = CUR_MINf;
		if (pad.Ly>(256-STICK_MID)) f=CUR_MIDf;
		if (pad.Ly>(256-STICK_MAX)) f=CUR_MAXf;
		c_y+=f;
		if (c_y > 480) c_y = 480;
		n_y=round(c_y);
		k=1;
	}
	
	if(k==1)
	{
		if (!m_dragging && m_buttonDown && (ABS(n_x-m_orgx) >= CLICK_TOLERANCE || ABS(n_y-m_orgy) >= CLICK_TOLERANCE))
		{
			m_dragging = true;
		}
		if (m_dragging)
		{
			m_x += n_x - m_prevx;
			m_y += n_y - m_prevy;
			m_prevx = n_x;
			m_prevy = n_y;
			m_game.m_refresh = true;
		}
		*x = n_x;
		*y = n_y;
		//return true;		
	}
	return false;
}

bool Overlay::onClick(int x, int y)
{
	DEBUG2(x,y,6,2);
	return false;
}
