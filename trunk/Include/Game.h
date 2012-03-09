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

#ifndef __GAME_H__
#define __GAME_H__

#include "SDL_Lite.h"

#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "Common.h"
#include "Array.h"
#include "Config.h"
#include "Path.h"
#include "Canvas.h"
#include "Levels.h"
#include "Stroke.h"
#include "Scene.h"
#include "Overlay.h"
#include "NextLevelOverlay.h"
#include "EditOverlay.h"
#include "PauseOverlay.h"
#include "Window.h"

#define SELECT		1	
#define START		2
#define UP			3
#define DOWN		4
#define LEFT		5
#define RIGHT		6
#define TRIANGLE	7
#define CIRCLE		8
#define CROSS		9
#define SQUARE		10
#define LTRIGGER	11
#define RTRIGGER	12

using namespace std;

class Game : public GameParams
{
	Scene   	    m_scene;
	Stroke  	   *m_createStroke;
	Stroke           *m_moveStroke;
	Array<Overlay*>   m_overlays;
	Window*            m_window;
	PauseOverlay       m_pauseOverlay;
	EditOverlay       m_editOverlay;

public:
	Game(int i);
	void gotoLevel(int l);
	bool save(char *file=NULL);
	bool send();
	void setTool(int t);
	void editMode(bool set);
	void showOverlay(Overlay& o);
	void hideOverlay(Overlay& o);
	void pause(bool doPause);
	void edit(bool doEdit);
	Vec2 mousePoint(SDL_Event ev);
	Vec2 mousePoint(int x, int y);

	bool handleGameEvent(SceCtrlData &pad);
	bool handleModEvent(SceCtrlData &pad);
	bool handlePlayEvent(SceCtrlData &pad);
	bool handleEditEvent(SceCtrlData &pad);
	
	void run();
	int x,y,oldy,oldx;
	float c_x,c_y;
	
private:
	int iterateCounter;
	int lastTick;
	bool isComplete;
	int scc;
	NextLevelOverlay completedOverlay;
	SDL_Event ev;
	SceCtrlData pad;
	int fast_cursor;
	
};

#endif
