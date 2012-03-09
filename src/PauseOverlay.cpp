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

#include "PauseOverlay.h"

PauseOverlay::PauseOverlay(GameParams& game, int x, int y, int w, int h):Overlay(game,x,y)
{
	m_x = x;
	m_y = y;
	m_w = w;
	m_h = h;
}

void PauseOverlay::draw(Canvas* screen)
{
	screen->drawPause(m_x,m_y);
}

bool PauseOverlay::handleEvent(SceCtrlData &pad, int *x, int *y)
{
	return false;
}
