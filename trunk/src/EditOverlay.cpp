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

#include "EditOverlay.h"

EditOverlay::EditOverlay(GameParams& game, int x, int y, int w, int h):Overlay(game,x,y),m_saving(0),m_sending(0)
{
	DEBUG(__FILE__,__FUNCTION__,__LINE__);
	DEBUG2(m_x,m_y,-18,0);
	m_x = 0;
	m_y = 0;
	m_w = w;
	m_h = h;	
}

Rect EditOverlay::pos(int i) 
{
	int c = i%3, r = i/3;
	return Rect(c*28+13, r*28+33, c*28+33, r*28+53);
}

int EditOverlay::index(int x, int y)
{
	int r = (y-33)/28;
	int c = (x-13)/28;
	if (r<0 || c<0 || c>2) return -1; 
	return r*3+c;
}

void EditOverlay::outline(Canvas* screen, int i, int c) 
{
	Rect r = pos(i) + Vec2(m_x,m_y);
	r.tl.x-=2; r.tl.y-=2;
	r.br.x+=2; r.br.y+=2;
	screen->drawRect(r, c, false);
}

void EditOverlay::draw(Canvas* screen)
{
	screen->drawEdit(m_x,m_y);
    for (int i=0; i<NUM_COLOURS; i++)
	{
		screen->drawRect(pos(i)+Vec2(m_x,m_y),brush_colours[i], true);
    }
	outline(screen, m_game.m_colour, 0);
	if (m_game.m_strokeFixed) outline(screen, 12, 0);
	if (m_game.m_strokeSleep) outline(screen, 13, 0);
	if (m_game.m_strokeDecor) outline(screen, 14, 0);
	if (m_sending) outline(screen, 16, screen->makeColour((m_sending--)<<9));
	if (m_saving)  outline(screen, 17, screen->makeColour((m_saving--)<<9));
}

bool EditOverlay::onClick(int x, int y)
{
	int i = index(x,y);
	//DEBUG2(x,y,66,i);
	switch (i)
	{
		case -1: return false;
		case 12: m_game.m_strokeFixed = ! m_game.m_strokeFixed; break;
		case 13: m_game.m_strokeSleep = ! m_game.m_strokeSleep; break;
		case 14: m_game.m_strokeDecor = ! m_game.m_strokeDecor; break;
		case 16: if ( m_game.send() ) m_sending=10; break;
		case 17: if ( m_game.save() ) m_saving=10; break;
		default: if (i<NUM_COLOURS) m_game.m_colour = i; break;
	}
	return true;
}
