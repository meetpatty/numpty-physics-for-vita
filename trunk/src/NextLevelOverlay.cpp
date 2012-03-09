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

#include "NextLevelOverlay.h"
#include "CanvasSoft.h"

const Rect FULLSCREEN_RECT(0, 0, CANVAS_WIDTH-1, CANVAS_HEIGHT-1);

NextLevelOverlay::NextLevelOverlay(GameParams& game, int x, int y, int w, int h):Overlay(game,x,y),m_levelIcon(-2),m_icon(NULL)
{
	m_x = x;
	m_y = y;
	m_w = w;
	m_h = h;
	b = false;
}

NextLevelOverlay::~NextLevelOverlay()
{
	if(b) free(m_icon);
}

void NextLevelOverlay::onShow()
{
	m_selectedLevel = m_game.m_level+1;
}

void NextLevelOverlay::draw(Canvas* screen)
{
	screen->fade(true);
	screen->drawNext(m_x,m_y);
	genIcon();
	if(b)
	{
		screen->drawImage(m_icon,m_x+50,m_y+38, 220, 110);
	}
}

bool NextLevelOverlay::onClick(int x, int y)
{
	//DEBUG2(x,y,-99,88);
	//if (y > 180)
	if((x>=200)&&(y>=159)&&(x<=308)&&(y<=183))
	{
		m_game.gotoLevel(m_selectedLevel);
	}
	else
	if((x>=282)&&(y>=81)&&(x<=310)&&(y<=107))//if (x > 300)
	{
		m_selectedLevel++;
		if (m_selectedLevel>m_game.m_levels.numLevels()) m_selectedLevel = m_game.m_levels.numLevels();
		//printf("NextLevel++ = %d\n",m_selectedLevel);
	}
	else
	if((x>=8)&&(y>=81)&&(x<=34)&&(y<=107)&&(m_selectedLevel>0))//if (x < 100 && m_selectedLevel > 0)
	{
		m_selectedLevel--; 
		//printf("NextLevel-- = %d\n",m_selectedLevel);
	}
	return true;
}

bool NextLevelOverlay::genIcon()
{
	if (m_levelIcon != m_selectedLevel)
	{
		printf("new thumbnail required\n");
		
		//m_icon = NULL;
		if (m_selectedLevel < m_game.m_levels.numLevels())
		{
			Scene scene(true);
			if (scene.load( m_game.m_levels.levelFile(m_selectedLevel)))
			{
				if (b) free(m_icon);
				printf("generating thumbnail %s\n",m_game.m_levels.levelFile(m_selectedLevel).c_str());
				CanvasSoft* temp = new CanvasSoft(CANVAS_WIDTH, CANVAS_HEIGHT);
				scene.draw(temp, FULLSCREEN_RECT);
				m_icon = (char*)malloc(512*272*2);
				b = true;
				memcpy(m_icon,temp->scale(m_selectedLevel),512*272*2);
				delete temp;
				printf("generating thumbnail %s done\n",m_game.m_levels.levelFile(m_selectedLevel).c_str());
			}
			else
			{
				printf("failed to gen scene thumbnail %s\n",
				m_game.m_levels.levelFile(m_selectedLevel).c_str());
			}
		}
		else
		{
			//m_icon = new Image("theend.bmp");
			if (b) 
			{
				free(m_icon);
				b=false;
			}
			m_caption = "no more levels!";
			m_selectedLevel = m_game.m_levels.numLevels();
		}
	m_levelIcon = m_selectedLevel;
	}
	return 0;//m_icon;
}
