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

#ifndef __CANVASSOFT_H__
#define __CANVASSOFT_H__

#include <string>

#include "Config.h"
#include "Common.h"
#include "Path.h"
#include "SDL_Lite.h"

class Path;

class CanvasSoft
{

public:
	CanvasSoft(int w, int h);
	virtual ~CanvasSoft();
	int width() const;
	int height() const;
	int  makeColour(int c) const;
	int  makeColour(int r, int g, int b) const;
	void resetClip();
	void setClip(int x, int y, int w, int h);
	void setBackground(int c);
	void setBackground(CanvasSoft* bg);
	void clear();
	void clear(const Rect& r);
	void fade();
	void* scale(int factor) const;
	void drawImage(CanvasSoft *canvas, int x, int y);
	void drawPixel(int x, int y, int c);
	int  readPixel(int x, int y) const;
	void drawLine(int x1, int y1, int x2, int y2, int c);
	void drawPath(const Path& path, int color, bool thick=false);
	void drawRect(int x, int y, int w, int h, int c, bool fill=true);
	void drawRect2(char *dst,int x, int y, int w, int h, int c, bool fill=true);
	void drawRect(const Rect& r, int c, bool fill=true);
	void drawWorldLine(b2Vec2 pos1, b2Vec2 pos2, int color, bool thick=false);
	void drawWorldPath(const Path& path, int color, bool thick=false);
	int writeBMP(const char* filename) const;
	
	
protected:
	typedef void* State;
	CanvasSoft(State state=NULL);
	State   m_state;
	int     m_bgColour;
	CanvasSoft* m_bgImage; 
	Rect    m_clip;

};

#endif
