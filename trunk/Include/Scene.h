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

#ifndef __SCENE_H__
#define __SCENE_H__

#include <string.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>

#include <Box2D/Box2D.h>
#include "Common.h"
#include "Path.h"
#include "Canvas.h"
#include "CanvasSoft.h"
#include "Config.h"
#include "Stroke.h"
#include "Image.h"

using namespace std;

class Scene
{
public:
	Scene(bool noWorld=false);
	~Scene();

	int numStrokes();
	Stroke* newStroke(const Path& p);
	void deleteStroke(Stroke *s);
	void activate(Stroke *s);
	void activateAll();
	void createJoints(Stroke *s);
	void step();
	bool isCompleted();
	Rect dirtyArea();
	void draw(Canvas* canvas, const Rect& area);
	void draw(CanvasSoft* canvas, const Rect& area);
	void reset(Stroke* s=NULL);
	Stroke* strokeAtPoint(const Vec2 pt, float max);
	void clear();
	bool load(const string& file);
	void protect(int n=-1);
	bool save(const std::string& file);
	Array<Stroke*>& strokes();
/*
	Array<Stroke*>& strokes() 
	{
		return m_strokes;
	}
*/

private:
	b2World        *m_world;
	Array<Stroke*>  m_strokes;
	string          m_title, m_author, m_bg;
	Image          *m_bgImage;
	static Image   *g_bgImage;
	int             m_protect;
	
};

#endif
