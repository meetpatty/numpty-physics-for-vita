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

#include "Game.h"

Uint8 keys[20];

const Rect FULLSCREEN_RECT( 0, 0, CANVAS_WIDTH-1, CANVAS_HEIGHT-1 );

Image *Scene::g_bgImage = NULL;

Game::Game(int t):m_pauseOverlay(*this,430,10,32,32),m_editOverlay(*this,0,0,100,200),completedOverlay(*this,80,20,320,192)
{
	DEBUG(__FILE__,__FUNCTION__,__LINE__);
	iterateCounter = 0;
	SDL_StartTicks();
	lastTick = SDL_GetTicks();
	isComplete = false;
	scc = 0;
	
	m_window = new Window(512,272);
	
	m_createStroke = NULL;
	m_moveStroke = NULL;
	
	m_levels.addPath("data");
	gotoLevel(0);

	x=y=60;
	c_x=x;
	c_y=y;
	memset(keys,0x00,sizeof(keys));
	fast_cursor=0;
}	

void Game::gotoLevel(int l)
{
	m_window->fade(false);
	DEBUG(__FILE__,__FUNCTION__,__LINE__);
	if (l >= 0 && l < m_levels.numLevels())
	{
		DEBUG(__FILE__,m_levels.levelFile(l).c_str(),__LINE__);
		m_scene.load( m_levels.levelFile(l).c_str() );
		m_scene.activateAll();
		m_level = l;
		//m_window.setSubName(m_levels.levelFile(l).c_str());
		m_refresh = true;
		if (m_edit) m_scene.protect(0);
	}
}

bool Game::save(char *file)
{
	string p;
	if (file)
	{
		p = file;
	}
	else
	{
		p = "data/L99_saved.nph";
	}
	if (m_scene.save(p))
	{
		m_levels.addPath(p.c_str());
		int l = m_levels.findLevel(p.c_str());
		if (l >= 0)
		{
			m_level = l;
		}
		return true;
	}
	return false;
}

bool Game::send()
{
	return save();
}

void Game::setTool(int t)
{
	m_colour = t;
}

void Game::editMode(bool set)
{
	m_edit = set;
}

void Game::showOverlay(Overlay& o)
{
	m_overlays.append(&o);
	o.onShow();
}

void Game::hideOverlay(Overlay& o)
{
	o.onHide();
	m_overlays.erase(m_overlays.indexOf(&o));
	m_refresh = true;
}

void Game::pause(bool doPause)
{
	if (m_pause != doPause)
	{
		m_pause = doPause;
		if (m_pause) 
		{
			showOverlay(m_pauseOverlay);
		}
		else
		{
			hideOverlay(m_pauseOverlay);
		}
	}
}

void Game::edit(bool doEdit)
{
	if (m_edit != doEdit)
	{
		m_edit = doEdit;
		if (m_edit)
		{
			showOverlay(m_editOverlay);
			m_scene.protect(0);
		}
		else
		{
			hideOverlay(m_editOverlay);
			m_strokeFixed = false;
			m_strokeSleep = false;
			m_strokeDecor = false;
			if (m_colour < 2) m_colour = 2;
			m_scene.protect();
		}
	}
}

Vec2 Game::mousePoint(SDL_Event ev)
{
	return Vec2(ev.button.x, ev.button.y);
}

Vec2 Game::mousePoint(int x, int y)
{
	return Vec2(x, y);
}

bool Game::handleGameEvent(SceCtrlData &pad)
{
	if (pad.Buttons)
	{
		if(pad.Buttons & PSP_CTRL_LTRIGGER)
		{
			if(pad.Buttons & PSP_CTRL_LEFT)
			{
				gotoLevel(m_level-1);
			}
			if(pad.Buttons & PSP_CTRL_RIGHT)
			{
				gotoLevel(m_level+1);
			}
		}
		if(keys[RTRIGGER]==0)
		{
			fast_cursor=1;
			keys[RTRIGGER]=1;
		}
		if(keys[SQUARE]==0)
		{
			if(pad.Buttons & PSP_CTRL_SQUARE) gotoLevel(m_level);
			keys[SQUARE]=1;
		}
		if(keys[TRIANGLE]==0)
		{
			if(pad.Buttons & PSP_CTRL_TRIANGLE) pause(!m_pause);
			keys[TRIANGLE]=1;
		}
		if(keys[START]==0)
		{
			if(pad.Buttons & PSP_CTRL_START) pause(!m_pause);
			keys[START]=1;
		}
		if(keys[SELECT]==0)
		{
			if(pad.Buttons & PSP_CTRL_SELECT) edit(!m_edit);
			keys[SELECT]=1;
		}
	}
	if(!(pad.Buttons & PSP_CTRL_LTRIGGER)) keys[LTRIGGER] = 0;
	if(!(pad.Buttons & PSP_CTRL_RTRIGGER))
	{
		keys[RTRIGGER] = 0;
		fast_cursor=0;
	}
	if(!(pad.Buttons & PSP_CTRL_SQUARE)) keys[SQUARE] = 0;
	if(!(pad.Buttons & PSP_CTRL_TRIANGLE)) keys[TRIANGLE] = 0;
	if(!(pad.Buttons & PSP_CTRL_START)) keys[START] = 0;
	if(!(pad.Buttons & PSP_CTRL_SELECT)) keys[SELECT] = 0;
	return false;
}

bool Game::handleModEvent(SceCtrlData &pad)
{

}

bool Game::handlePlayEvent(SceCtrlData &pad)
{
	int k = 0;
	if (pad.Buttons)
	{
		if(keys[CROSS]==0)
		{
			if(pad.Buttons & PSP_CTRL_CROSS)
			{
				//DEBUG(__FILE__,"DOWN",__LINE__);
				keys[CROSS]=1;
				if(!m_createStroke)
				{
					m_createStroke = m_scene.newStroke(Path()&mousePoint(x,y));
					if (m_createStroke)
					{
						switch (m_colour)
						{
							case 0: m_createStroke->setAttribute(Stroke::ATTRIB_TOKEN); break;
							case 1: m_createStroke->setAttribute(Stroke::ATTRIB_GOAL); break;
							default: m_createStroke->setColour(brush_colours[m_colour]); break;
						}
						if (m_strokeFixed)
						{
							m_createStroke->setAttribute(Stroke::ATTRIB_GROUND);
						}
						if (m_strokeSleep)
						{
							m_createStroke->setAttribute(Stroke::ATTRIB_SLEEPING);
						}
						if (m_strokeDecor)
						{
							m_createStroke->setAttribute(Stroke::ATTRIB_DECOR);
						}			
					}
				}
			}
		}
		
		if(pad.Buttons & PSP_CTRL_UP)
		{
			if(fast_cursor) y-=FAST_MOVE;
				else y--;
			if (y < 0) y = 0;
			k=1;
		}
		if(pad.Buttons & PSP_CTRL_DOWN)
		{
			if(fast_cursor) y+=FAST_MOVE;
				else y++;
			if (y > 271) y = 271;
			k=1;
		}
		if(pad.Buttons & PSP_CTRL_LEFT)
		{
			if(fast_cursor) x-=FAST_MOVE;
				else x--;
			if (x < 0) x = 0;
			k=1;
		}
		if(pad.Buttons & PSP_CTRL_RIGHT)
		{
			if(fast_cursor) x+=FAST_MOVE;
				else x++;
			if (x > 480) x = 480;
			k=1;
		}
		if(keys[CIRCLE]==0)
		{
			if(pad.Buttons & PSP_CTRL_CIRCLE)
			{
				keys[CIRCLE]=1;
				if (m_createStroke)
				{
					m_scene.deleteStroke(m_createStroke);
					m_createStroke = NULL;
				}
				else
				{
					m_scene.deleteStroke(m_scene.strokes().at(m_scene.strokes().size()-1));
				}
				m_refresh = true;
			}
		}
	}
	if((!(pad.Buttons & PSP_CTRL_CROSS))&&(keys[CROSS]==1))
	{	
		//DEBUG(__FILE__,"UP",__LINE__);
		if(m_createStroke)
		{
			if (m_createStroke->numPoints() > 1)
			{
				m_scene.activate(m_createStroke);
			}
			else
			{
				m_scene.deleteStroke(m_createStroke);
			}
			m_createStroke = NULL;		
		}
		keys[CROSS] = 0;
	}
	if(!(pad.Buttons & PSP_CTRL_CIRCLE)) keys[CIRCLE] = 0;
	
	if(k==1)
	{
		if (m_createStroke)
		{
			m_createStroke->addPoint(mousePoint(x,y));
		}
		c_x = x;
		c_y = y;
	}
	
	if (pad.Lx<STICK_MIN)
	{
		float f = CUR_MINf;
		if (pad.Lx<STICK_MID) f=CUR_MIDf;
		if (pad.Lx<STICK_MAX) f=CUR_MAXf;
		if(fast_cursor) f*=FAST_MOVE;
		c_x-=f;
		if (c_x < 0) c_x = 0;
		x=round(c_x);
		k=1;
	}
	if (pad.Lx>(256-STICK_MIN))
	{
		float f = CUR_MINf;
		if (pad.Lx>(256-STICK_MID)) f=CUR_MIDf;
		if (pad.Lx>(256-STICK_MAX)) f=CUR_MAXf;
		if(fast_cursor) f*=FAST_MOVE;
		c_x+=f;
		if (c_x > 480) c_x = 480;
		x=round(c_x);
		k=1;
	}
	if (pad.Ly<STICK_MIN)
	{
		float f = CUR_MINf;
		if (pad.Ly<STICK_MID) f=CUR_MIDf;
		if (pad.Ly<STICK_MAX) f=CUR_MAXf;
		if(fast_cursor) f*=FAST_MOVE;
		c_y-=f;
		if (c_y < 0) c_y = 0;
		y=round(c_y);
		k=1;
	}
	if (pad.Ly>(256-STICK_MIN))
	{
		float f = CUR_MINf;
		if (pad.Ly>(256-STICK_MID)) f=CUR_MIDf;
		if (pad.Ly>(256-STICK_MAX)) f=CUR_MAXf;
		if(fast_cursor) f*=FAST_MOVE;
		c_y+=f;
		if (c_y > 480) c_y = 480;
		y=round(c_y);
		k=1;
	}
	
	if(k==1)
	{
		if (m_createStroke)
		{
			m_createStroke->addPoint(mousePoint(x,y));
		}	
	}
		
	return false;
}

bool Game::handleEditEvent(SceCtrlData &pad)
{

}

void Game::run()
{
	{
		for (int i=0; i<m_overlays.size(); i++)
		{
			m_overlays[i]->onTick(lastTick);
		}

		sceCtrlReadBufferPositive(&pad, 1);
		{ 
			bool handled = false;
			for (int i=m_overlays.size()-1; i>=0 && !handled; --i)
			{
				handled = m_overlays[i]->handleEvent(pad,&x,&y);
			}
			if (!handled)
			{
				handled = false || handleModEvent(pad) || handleGameEvent(pad) || handleEditEvent(pad) || handlePlayEvent(pad);
			}
		}

		if (isComplete && m_edit)
		{
			hideOverlay(completedOverlay);
			isComplete = false;
		}
		if (m_scene.isCompleted() != isComplete && !m_edit)
		{
			isComplete = m_scene.isCompleted();
			if (isComplete)
			{
				showOverlay(completedOverlay);
			}
			else
			{
				hideOverlay(completedOverlay);
			}
		}

		Rect r = m_scene.dirtyArea();
		if (m_refresh || isComplete)
		{
			r = FULLSCREEN_RECT;
		}

		//if (!r.isEmpty())
		{
			m_scene.draw(m_window, r);
		}

		for (int i=0; i<m_overlays.size(); i++)
		{
			m_overlays[i]->draw(m_window);
			r.expand(m_overlays[i]->dirtyArea());
		}

		//temp
		if (m_refresh)
		{
			//m_window.update(FULLSCREEN_RECT);
			m_refresh = false;
		}
		else
		{
			r.br.x++; r.br.y++;
			//m_window.update(r);
		} 


		if (!m_pause)
		{
			//assumes RENDER_RATE <= ITERATION_RATE
			while(iterateCounter < ITERATION_RATE)
			{
				m_scene.step();
				iterateCounter += RENDER_RATE;
			}
			iterateCounter -= ITERATION_RATE;
		}

		int sleepMs = lastTick + RENDER_INTERVAL -  SDL_GetTicks();

		lastTick = SDL_GetTicks();

		m_window->drawRect(x-1, y-1, 3, 3, 0, true);
		m_window->drawRect(x, y, 1, 1, 0xFFFFFFFF, true);
	}
}
