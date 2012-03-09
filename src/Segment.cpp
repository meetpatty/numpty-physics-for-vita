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


#include "Segment.h"

Segment::Segment(const Vec2& p1, const Vec2& p2):m_p1(p1),m_p2(p2)
{

}

float Segment::distanceTo(const Vec2& p)
{
	bool withinLine;
	float d = calcDistanceToLine(p, m_p1, m_p2, &withinLine);
	if (!(m_p1 == m_p2) && withinLine)
	{
		return d;
	}
	else
	{
		return b2Min(calcDistance(p, m_p2), calcDistance(p, m_p1));
	}
}


float Segment::calcDistanceToLine(const Vec2& pt,const Vec2& l1, const Vec2& l2, bool* withinLine)
{
	b2Vec2 l = l2 - l1; 
	b2Vec2 w = pt - l1;
	float mag = l.Normalize();
	float dist = b2Cross(w, l);
	if (withinLine)
	{
		float dot = b2Dot(l, w);
		*withinLine = (dot >= 0.0f && dot <= mag);
	}  
	return b2Abs(dist);
}


float Segment::calcDistance(const Vec2& l1, const Vec2& l2) 
{
	return b2Vec2(l1-l2).Length();
}


