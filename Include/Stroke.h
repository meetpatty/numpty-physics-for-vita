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

#ifndef __STROKE_H__
#define __STROKE_H__

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
#include "CanvasSoft.h"

using namespace std;

class Stroke
{

public:
	typedef enum {
		ATTRIB_DUMMY = 0,
		ATTRIB_GROUND = 1,
		ATTRIB_TOKEN = 2,
		ATTRIB_GOAL = 4,
		ATTRIB_DECOR = 8,
		ATTRIB_SLEEPING = 16,
		ATTRIB_CLASSBITS = ATTRIB_TOKEN | ATTRIB_GOAL
	} Attribute;

private:
	struct JointDef : public b2RevoluteJointDef
	{
		JointDef(b2Body* b1, b2Body* b2, const b2Vec2& pt)
		{
			body1 = b1;
			body2 = b2;
			anchorPoint = pt;
			motorTorque = 10.0f;
			motorSpeed = 0.0f;
			enableMotor = true;
		}
	};

	struct BoxDef : public b2BoxDef
	{
		void init(const Vec2& p1, const Vec2& p2, int attr)
		{
			b2Vec2 barOrigin = p1;
			b2Vec2 bar = p2 - p1;
			bar *= 1.0f/PIXELS_PER_METREf;
			barOrigin *= 1.0f/PIXELS_PER_METREf;;
			extents.Set( bar.Length()/2.0f, 0.1f );
			localPosition = 0.5f*bar + barOrigin;
			localRotation = vec2Angle( bar );
			friction = 0.3f;
			if (attr & ATTRIB_GROUND)
			{
				density = 0.0f;
			}
			else
			if (attr & ATTRIB_GOAL)
			{
				density = 100.0f;
			}
			else
			if (attr & ATTRIB_TOKEN)
			{
				density = 3.0f;
				friction = 0.1f;
			}
			else
			{
				density = 5.0f;
			}
			restitution = 0.2f;
		}
	};

public:
	Stroke(const Path& path);
	Stroke(const string& str);

	void reset(b2World* world=NULL);
	string asString();
	void setAttribute(Stroke::Attribute a);
	bool hasAttribute(Stroke::Attribute a);
	void setColour(int c);
	void createBodies(b2World& world);
	bool maybeCreateJoint(b2World& world, Stroke* other);
	void draw(Canvas* canvas);
	void draw(CanvasSoft* canvas);
	void addPoint(const Vec2& pp);
	void origin(const Vec2& p);
	b2Body* body();
	float distanceTo(const Vec2& pt);
	Rect bbox();
	Rect lastDrawnBbox();
	bool isDirty();
	void hide();
	bool hidden();
	int numPoints();

private:
	static float vec2Angle(b2Vec2 v);
	void process();
	bool transform();

	Path      m_rawPath;
	int       m_colour;
	int       m_attributes;
	Vec2      m_origin;
	Path      m_shapePath;
	Path      m_xformedPath;
	float     m_xformAngle;
	b2Vec2    m_xformPos;
	Rect      m_xformBbox;
	Rect      m_drawnBbox;
	bool      m_drawn;
	b2Body*   m_body;
	bool      m_jointed[2];
	int       m_hide;
};

#endif
