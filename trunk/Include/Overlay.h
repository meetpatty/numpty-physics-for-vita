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

#ifndef __OVERLAY_H__
#define __OVERLAY_H__

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
#include "Canvas.h"
#include "Levels.h"

#include "SDL_Lite.h"

#define UP			3
#define DOWN		4
#define LEFT		5
#define RIGHT		6

using namespace std;

struct GameParams
{
	GameParams() : m_quit(false),
		m_pause(false),
		m_edit( false ),
		m_refresh( true ),
		m_colour( 2 ),
		m_strokeFixed( false ),
		m_strokeSleep( false ),
		m_strokeDecor( false ),
		m_levels(),
		m_level(0)
		{}
	virtual ~GameParams() {}
	virtual bool save( char *file=NULL ) { return false; }
	virtual bool send() { return false; }
	virtual void gotoLevel( int l ) {}
	bool  m_quit;
	bool  m_pause;
	bool  m_edit;
	bool  m_refresh;
	int   m_colour;
	bool  m_strokeFixed;
	bool  m_strokeSleep;
	bool  m_strokeDecor;
	Levels m_levels;
	int    m_level;
};

class Overlay
{
public:
	Overlay(GameParams& game, int x=10, int y=10, int w=10, int h=10);
	virtual ~Overlay();

	Rect dirtyArea();
	virtual void onShow();
	virtual void onHide();
	virtual void onTick(int tick);
	virtual void draw(Canvas* screen);
	virtual bool handleEvent(SceCtrlData &pad, int *x, int *y);
	virtual bool onClick(int x, int y);

protected:
	GameParams& m_game;
	int     m_x, m_y, m_w, m_h;
	Canvas *m_canvas;
  
private:
	int     m_orgx, m_orgy;
	int     m_prevx, m_prevy;
	bool    m_dragging;
	bool    m_buttonDown;

};

#endif
