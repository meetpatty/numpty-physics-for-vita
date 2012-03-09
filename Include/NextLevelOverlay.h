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

#ifndef __NEXTLEVELOVERLAY_H__
#define __NEXTLEVELOVERLAY_H__

#include <string.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>

#include <Box2D/Box2D.h>
#include "Common.h"
#include "Path.h"
#include "Canvas.h"
#include "Config.h"
#include "Levels.h"
#include "Overlay.h"
#include "Scene.h"

#include "SDL_Lite.h"

using namespace std;

class NextLevelOverlay : public Overlay
{

public:
	NextLevelOverlay(GameParams& game, int x, int y, int w, int h);
	~NextLevelOverlay();

	virtual void onShow();
	virtual void draw(Canvas* screen);
	virtual bool onClick(int x, int y);

private:
	bool genIcon();
	int     m_selectedLevel;
	int     m_levelIcon;
	void* m_icon;
	string  m_caption;
	bool b;
};

#endif
