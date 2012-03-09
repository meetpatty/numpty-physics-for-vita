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

#include "Scene.h"

const Rect BOUNDS_RECT(-CANVAS_WIDTH/4, -CANVAS_HEIGHT,CANVAS_WIDTH*5/4, CANVAS_HEIGHT);
			
Scene::Scene(bool noWorld):m_world(NULL),m_bgImage(NULL),m_protect(0)
{
	if (!noWorld)
	{
		b2AABB worldAABB;
		worldAABB.minVertex.Set(-100.0f, -100.0f);
		worldAABB.maxVertex.Set(100.0f, 100.0f);
      
		b2Vec2 gravity(0.0f, 10.0f);
		bool doSleep = true;
		m_world = new b2World(worldAABB, gravity, doSleep);
    }
}

Scene::~Scene()
{
	clear();
	delete m_world;
}

int Scene::numStrokes()
{
	return m_strokes.size();
}

Stroke* Scene::newStroke(const Path& p)
{
	Stroke *s = new Stroke(p);
	m_strokes.append(s);
	return s;
}

void Scene::deleteStroke(Stroke *s)
{ 
	if (s)
	{
		int i = m_strokes.indexOf(s);
		if (i >= m_protect)
		{
			reset(s);
			m_strokes.erase(m_strokes.indexOf(s));
		}
	}
}

void Scene::activate( Stroke *s )
{
	s->createBodies(*m_world);
	createJoints(s);
}

void Scene::activateAll()
{
	for (int i=0; i < m_strokes.size(); i++)
	{
		m_strokes[i]->createBodies(*m_world);
	}
	
	for (int i=0; i < m_strokes.size(); i++)
	{
		createJoints( m_strokes[i] );
	}
}

void Scene::createJoints(Stroke *s)
{
	for (int j=m_strokes.size()-1; j>=0; j--)
	{      
		if (s != m_strokes[j])
		{
			s->maybeCreateJoint(*m_world, m_strokes[j]);
			m_strokes[j]->maybeCreateJoint(*m_world, s);
		}
	}    
}

void Scene::step()
{
	m_world->Step(ITERATION_TIMESTEPf, SOLVER_ITERATIONS);

	for (b2Contact* c = m_world->GetContactList(); c; c = c->GetNext())
	{
		if (c->GetManifoldCount() > 0)
		{
			Stroke* s1 = (Stroke*)c->GetShape1()->GetBody()->GetUserData();
			Stroke* s2 = (Stroke*)c->GetShape2()->GetBody()->GetUserData();
			if (s1 && s2)
			{
				if (s2->hasAttribute(Stroke::ATTRIB_TOKEN)) b2Swap(s1, s2);
			}
			
			if (s1->hasAttribute(Stroke::ATTRIB_TOKEN) && s2->hasAttribute(Stroke::ATTRIB_GOAL)) s2->hide();
		}
	}

	for (int i=0; i < m_strokes.size(); i++)
	{
		if (m_strokes[i]->hasAttribute(Stroke::ATTRIB_TOKEN) && !BOUNDS_RECT.intersects(m_strokes[i]->lastDrawnBbox()))
		{
			reset(m_strokes[i]);
			activate(m_strokes[i]);
		}
	}
}

bool Scene::isCompleted()
{
	for (int i=0; i < m_strokes.size(); i++)
	{
		if (m_strokes[i]->hasAttribute( Stroke::ATTRIB_GOAL ) && !m_strokes[i]->hidden()) return false;
	}

	return true;
}

Rect Scene::dirtyArea()
{
	Rect r(0,0,0,0),temp;
	int numDirty = 0;
	for (int i=0; i<m_strokes.size(); i++)
	{
		if (m_strokes[i]->isDirty())
		{
			temp = m_strokes[i]->bbox();
			if (!temp.isEmpty())
			{
				if (numDirty==0)
				{	
					r = temp;
				}
				else
				{
					r.expand(m_strokes[i]->bbox());
				}
			r.expand(m_strokes[i]->lastDrawnBbox());
			numDirty++;
			}
		}
	}
	
	if (!r.isEmpty())
	{
		r.tl.x--; r.tl.y--;
		r.br.x++; r.br.y++;
	}
	return r;
}

void Scene::draw(Canvas* canvas, const Rect& area)
{
    if (m_bgImage)
	{
		canvas->setBackground(m_bgImage);
    } 
	else
	{
		canvas->setBackground(0);
    }
    canvas->clear();
	//canvas->clear(area);
    Rect clipArea = area;
    clipArea.tl.x--;
    clipArea.tl.y--;
    clipArea.br.x++;
    clipArea.br.y++;
    for (int i=0; i<m_strokes.size(); i++)
	{
		//if (area.intersects(m_strokes[i]->bbox()))
		{
			m_strokes[i]->draw(canvas);
		}
    }
	//canvas.drawRect( area, 0xffff0000, false );
}

void Scene::draw(CanvasSoft* canvas, const Rect& area)
{

	//canvas->setBackground(0);
    canvas->clear();
    Rect clipArea = area;
    clipArea.tl.x--;
    clipArea.tl.y--;
    clipArea.br.x++;
    clipArea.br.y++;
	DEBUG("Scene::draw"," ",m_strokes.size());
    for (int i=0; i<m_strokes.size(); i++)
	{
		//if (area.intersects(m_strokes[i]->bbox()))
		{
			m_strokes[i]->draw(canvas);
		}
    }
	//canvas.drawRect( area, 0xffff0000, false );
}

void Scene::reset(Stroke* s)
{
	for (int i=0; i<m_strokes.size(); i++)
	{
		if (s==NULL || s==m_strokes[i]) m_strokes[i]->reset(m_world);
	}    
}

Stroke* Scene::strokeAtPoint(const Vec2 pt, float max)
{
	Stroke* best = NULL;
	for (int i=0; i<m_strokes.size(); i++)
	{
		float d = m_strokes[i]->distanceTo(pt);
		if (d < max)
		{
			max = d;
			best = m_strokes[i];
		}
	}
	return best;
}

void Scene::clear()
{
	reset();
	while(m_strokes.size())
	{
		delete m_strokes[0];
		m_strokes.erase(0);
	}
	if (m_world)
	{
		while(m_world->GetBodyList())
		{
			m_world->DestroyBody( m_world->GetBodyList() );	       
		}
		m_world->Step(ITERATION_TIMESTEPf, SOLVER_ITERATIONS);
	}
}

bool Scene::load(const string& file)
{
	DEBUG(__FILE__,__FUNCTION__,__LINE__);
	clear();
	DEBUG(__FILE__,__FUNCTION__,__LINE__);
	if (g_bgImage==NULL) g_bgImage = NULL;//!new Image("paper.bmp");
	DEBUG(__FILE__,__FUNCTION__,__LINE__);
	m_bgImage = g_bgImage;
	DEBUG(__FILE__,__FUNCTION__,__LINE__);
#if 0
	m_title = "";
	m_bg = "";
	m_author = "test";
	DEBUG(__FILE__,__FUNCTION__,__LINE__);
	m_strokes.append( new Stroke("Sf2: 5,250 500,200") ); 
	m_strokes.append( new Stroke("Sgs: 296,191 305,170 304,179 307,189 323,190 319,192 315,199 313,199 317,209 312,207 311,208 312,207 314,209 316,215 306,202 304,202 299,207 298,210 296,210 294,213 293,212 291,205 293,199 287,197 282,191 291,188 298,188") );
	m_strokes.append( new Stroke("Sf4: 400,136") );
	m_strokes.append( new Stroke("St: 128,68 129,71 127,71 124,72 123,73 119,76 118,78 116,79 115,83 114,85 118,98 120,101 123,103 125,105 127,106 131,109 140,109 146,107 149,104 151,103 153,100 154,99 156,96 157,91 158,91 157,77 154,77 151,73 150,70 142,66 139,65 131,65 130,68") );
#endif
#if 0
	FILE* fp = fopen(file.c_str(),"r");
	string line(mygetline(fp));

	while(strcmp(line.c_str(),"\n"))
	{
		switch(line[0])
		{
			case 'T': m_title = line.substr(line.find(':')+1); break;
			case 'B': m_bg = line.substr(line.find(':')+1); break;
			case 'A': m_author = line.substr(line.find(':')+1); break;
			case 'S': m_strokes.append( new Stroke(line) ); break;
		}	
		line = mygetline(fp);
	}
	fclose(fp);
#endif
    string line;
    ifstream i( file.c_str(), ios::in );
    while ( i.is_open() && !i.eof() ) {
      getline( i, line );
      //cout << "read: " << line << endl;;
      switch( line[0] ) {
      case 'T': m_title = line.substr(line.find(':')+1); break;
      case 'B': m_bg = line.substr(line.find(':')+1); break;
      case 'A': m_author = line.substr(line.find(':')+1); break;
      case 'S': m_strokes.append( new Stroke(line) ); break;
      }
    }
    i.close();
	DEBUG(__FILE__,__FUNCTION__,__LINE__);
	protect();
	DEBUG(__FILE__,__FUNCTION__,__LINE__);
	return true;
}

void Scene::protect(int n)
{
	m_protect = (n==-1 ? m_strokes.size() : n);
}

bool Scene::save(const std::string& file)
{
	printf("saving to %s\n",file.c_str());
	ofstream o(file.c_str(), ios::out);
	if (o.is_open())
	{
		o << "Title: "<<m_title<<endl;
		o << "Author: "<<m_author<<endl;
		o << "Background: "<<m_bg<<endl;
		for (int i=0; i<m_strokes.size(); i++ )
		{
			o << m_strokes[i]->asString();
		}
		o.close();
		return true;
	} 
	else 
	{
		return false;
	}
}

Array<Stroke*>& Scene::strokes() 
{
	return m_strokes;
}
