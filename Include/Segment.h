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

#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include "Common.h"
#include "Array.h"


class Segment
{

public:
	Segment(const Vec2& p1, const Vec2& p2);
	float distanceTo(const Vec2& p);

private:
	float calcDistanceToLine(const Vec2& pt,const Vec2& l1, const Vec2& l2, bool* withinLine=NULL);
	float calcDistance(const Vec2& l1, const Vec2& l2);
	Vec2 m_p1, m_p2;
	
};


#endif
