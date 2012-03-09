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

#include "Stroke.h"

Stroke::Stroke(const Path& path):m_rawPath(path)
{
//DEBUG(__FILE__,__FUNCTION__,__LINE__);
	m_colour = COLOUR_BLUE;
	m_attributes = 0;
	m_origin = m_rawPath.point(0);
	m_rawPath.translate( -m_origin );
	reset();
//DEBUG(__FILE__,__FUNCTION__,__LINE__);
}

Stroke::Stroke(const string& str) 
{
//DEBUG(__FILE__,__FUNCTION__,__LINE__);
	int col = 0;
	m_colour = brush_colours[2];
	m_attributes = 0;
	m_origin = Vec2(400,240);
	reset();
	const char *s = str.c_str();
	
	while (*s && *s!=':' && *s!='\n') 
	{
		switch ( *s ) 
		{
			case 't': setAttribute( ATTRIB_TOKEN ); break;	
			case 'g': setAttribute( ATTRIB_GOAL ); break;	
			case 'f': setAttribute( ATTRIB_GROUND ); break;
			case 's': setAttribute( ATTRIB_SLEEPING ); break;
			case 'd': setAttribute( ATTRIB_DECOR ); break;
			default:
				if ( *s >= '0' && *s <= '9' ) col = col*10 + *s -'0';
				break;
		}
		s++;
	}
	
	if ( col >= 0 && col < NUM_COLOURS ) m_colour = brush_colours[col];

	if ( *s++ == ':' )
	{
		float x,y;      
		while ( sscanf( s, "%f,%f", &x, &y )==2)
		{
			float x1 = x*(CANVAS_WIDTHf/800.0f);
			float y1 = y*(CANVAS_HEIGHTf/480.0f);
			m_rawPath.append( Vec2((int)x1,(int)y1) );
			while ( *s && *s!=' ' && *s!='\t' ) s++;
			while ( *s==' ' || *s=='\t' ) s++;
		}
	}
	
	if ( m_rawPath.size() < 2 )
	{
		printf("invalid stroke def\n");
	}
	
	m_origin = m_rawPath.point(0);
	m_rawPath.translate( -m_origin );
	setAttribute( ATTRIB_DUMMY );
//DEBUG(__FILE__,__FUNCTION__,__LINE__);
}

void Stroke::reset(b2World* world)
{
	if (m_body && world) world->DestroyBody(m_body);

	m_body = NULL;
	m_xformAngle = 7.0f;
	m_drawnBbox.tl = m_origin;
	m_drawnBbox.br = m_origin;
	m_jointed[0] = m_jointed[1] = false;
	m_shapePath = m_rawPath;
	m_hide = 0;
	m_drawn = false;
}

string Stroke::asString()
{
	stringstream s;
	s << 'S';
	
	if (hasAttribute(ATTRIB_TOKEN))    s<<'t';
	if (hasAttribute(ATTRIB_GOAL))     s<<'g';
	if (hasAttribute(ATTRIB_GROUND))   s<<'f';
	if (hasAttribute(ATTRIB_SLEEPING)) s<<'s';
	if (hasAttribute(ATTRIB_DECOR))    s<<'d';
	
	for (int i=0; i<NUM_COLOURS; i++)
	{
		if (m_colour==brush_colours[i])  s<<i;
	}
	
	s << ":";
	transform();
	
	for (int i=0; i<m_xformedPath.size(); i++)
	{
		const Vec2& p = m_xformedPath.point(i);
		float x1 = p.x/(CANVAS_WIDTHf/800.0f);
		float y1 = p.y/(CANVAS_HEIGHTf/480.0f);
		int x2 = (int)x1;
		int y2 = (int)y1;
		s <<' '<< x2 << ',' << y2; 
	}
	
	s << endl;
	return s.str();
}

void Stroke::setAttribute(Stroke::Attribute a)
{
	m_attributes |= a;
	if ( m_attributes & ATTRIB_TOKEN )     m_colour = COLOUR_RED;
	else if ( m_attributes & ATTRIB_GOAL ) m_colour = COLOUR_YELLOW;
}

bool Stroke::hasAttribute(Stroke::Attribute a)
{
	return (m_attributes&a) != 0;
}

void Stroke::setColour(int c) 
{
	m_colour = c;
}

void Stroke::createBodies( b2World& world )
{
	process();
	if (hasAttribute(ATTRIB_DECOR)) return;

	int n = m_shapePath.numPoints();
	
	if ( n > 1 )
	{
		BoxDef boxDef[n];
		b2BodyDef bodyDef;
		for (int i=1; i<n; i++)
		{
			boxDef[i].init(m_shapePath.point(i-1),m_shapePath.point(i),m_attributes );
			bodyDef.AddShape(&boxDef[i]);
		}
		bodyDef.position = m_origin;
		bodyDef.position *= 1.0f/PIXELS_PER_METREf;
		bodyDef.userData = this;
		
		if (m_attributes & ATTRIB_SLEEPING) bodyDef.isSleeping = true;

		m_body = world.CreateBody( &bodyDef );
	}
	transform();
}

bool Stroke::maybeCreateJoint(b2World& world, Stroke* other)
{
	if ((m_attributes&ATTRIB_CLASSBITS) != (other->m_attributes&ATTRIB_CLASSBITS))
	{
		return false;
	} else 
	if (hasAttribute(ATTRIB_GROUND))
	{
		return true;
	} else
	if (m_body && other->body())
	{
		transform();
		int n = m_xformedPath.numPoints();
		for (int end=0; end<2; end++)
		{
			if (!m_jointed[end])
			{
				const Vec2& p = m_xformedPath.point( end ? n-1 : 0 );
				if (other->distanceTo( p ) <= JOINT_TOLERANCE)
				{
					b2Vec2 pw = p;
					pw *= 1.0f/PIXELS_PER_METREf;
					JointDef j( m_body, other->m_body, pw );
					world.CreateJoint( &j );
					m_jointed[end] = true;
				}
			}
		}
	}
	if (m_body)
	{
		return m_jointed[0] && m_jointed[1];
	}
	return true;
}

void Stroke::draw(Canvas* canvas)
{
	if (m_hide < HIDE_STEPS)
	{
		transform();
		canvas->drawPath( m_xformedPath, canvas->makeColour(m_colour), true );
		m_drawn = true;
	}
	m_drawnBbox = m_xformBbox;
}

void Stroke::draw(CanvasSoft* canvas)
{
	if (m_hide < HIDE_STEPS)
	{
		//DEBUG("Stroke::draw"," ",0);
		transform();
		canvas->drawPath( m_xformedPath, canvas->makeColour(m_colour), true );
		m_drawn = true;
	}
	m_drawnBbox = m_xformBbox;
}

void Stroke::addPoint(const Vec2& pp) 
{
	Vec2 p = pp; p -= m_origin;
	if (p == m_rawPath.point(m_rawPath.numPoints()-1))
	{
	} 
	else
	{
		m_rawPath.append( p );
		m_drawn = false;
	}
}

void Stroke::origin(const Vec2& p) 
{
	if (m_body)
	{
		b2Vec2 pw = p;
		pw *= 1.0f/PIXELS_PER_METREf;
		m_body->SetOriginPosition(pw, m_body->GetRotation());
	}
	m_origin = p;
}

b2Body* Stroke::body()
{ 
	return m_body;
}

float Stroke::distanceTo(const Vec2& pt)
{
	float best = 100000.0;
	transform();
	
	for (int i=1; i<m_xformedPath.numPoints(); i++)
	{    
		Segment s(m_xformedPath.point(i-1), m_xformedPath.point(i));
		float d = s.distanceTo( pt );
		if ( d < best ) best = d;
	}
	return best;
}

Rect Stroke::bbox() 
{
	transform();
	return m_xformBbox;
}

Rect Stroke::lastDrawnBbox() 
{
	return m_drawnBbox;
}

bool Stroke::isDirty()
{
	return !m_drawn || transform();
}

void Stroke::hide()
{
	if ( m_hide==0 )
	{
		m_hide = 1;

		if (m_body)
		{
			m_body->SetCenterPosition( b2Vec2(0.0f,CANVAS_HEIGHTf*2.0f), 0.0f );
			m_body->SetLinearVelocity( b2Vec2(0.0f,0.0f) );
			m_body->SetAngularVelocity( 0.0f );
		}
	}
}

bool Stroke::hidden()
{
	return m_hide >= HIDE_STEPS;
}

int Stroke::numPoints()
{
	return m_rawPath.numPoints();
}

float Stroke::vec2Angle( b2Vec2 v ) 
{
	float a=atan(v.y/v.x);
	return v.y>0?a:a+b2_pi;
} 

void Stroke::process()
{
	float thresh = 0.1*SIMPLIFY_THRESHOLDf;
	m_rawPath.simplify( thresh );
	m_shapePath = m_rawPath;

	while (m_shapePath.numPoints() > MULTI_VERTEX_LIMIT)
	{
		thresh += SIMPLIFY_THRESHOLDf;
		m_shapePath.simplify(thresh);
	}
}

bool Stroke::transform()
{
	if (m_hide)
	{
		if (m_hide < HIDE_STEPS)
		{
			Vec2 o = m_xformBbox.centroid();
			m_xformedPath -= o;
			m_xformedPath.scale( 0.99 );
			m_xformedPath += o;
			m_xformBbox = m_xformedPath.bbox();
			m_hide++;
			return true;
		}
	}
	else 
	if (m_body)
	{
		if (hasAttribute(ATTRIB_DECOR))
		{
			return false;
		}
		else
		if (hasAttribute(ATTRIB_GROUND) && (m_xformAngle == m_body->GetRotation()))
		{
			return false;
		}
		else
		if (m_xformAngle != m_body->GetRotation() || !(m_xformPos == m_body->GetOriginPosition()))
		{
			b2Mat22 rot(m_body->GetRotation());
			b2Vec2 orig = PIXELS_PER_METREf * m_body->GetOriginPosition();
			m_xformedPath = m_rawPath;
			m_xformedPath.rotate( rot );
			m_xformedPath.translate( Vec2(orig) );
			m_xformAngle = m_body->GetRotation();
			m_xformPos = m_body->GetOriginPosition();
			m_xformBbox = m_xformedPath.bbox();
		}
		else
		if (!(m_xformPos == m_body->GetOriginPosition()))
		{
			//NOT WORKING printf("transform stroke - pos\n");
			b2Vec2 move = m_body->GetOriginPosition() - m_xformPos;
			move *= PIXELS_PER_METREf;
			m_xformedPath.translate( Vec2(move) );
			m_xformPos = m_body->GetOriginPosition();
			m_xformBbox = m_xformedPath.bbox();
		}
		else
		{
			return false;
		}
	}
	else
	{
		m_xformedPath = m_rawPath;
		m_xformedPath.translate( m_origin );
		m_xformBbox = m_xformedPath.bbox();      
		return false;
	}
	return true;
}
