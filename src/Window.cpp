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

#include "Window.h"

#define SURFACE(cANVASpTR) ((SDL_Surface*)((cANVASpTR)->m_state))



Window::Window(int w, int h, const char* title, const char* winclass)
{
	//screen = m_state;
	m_state = (void*)1;
	resetClip();
}


void Window::update(const Rect& r)
{
	if (r.tl.x < width() && r.tl.y < height())
	{
		int x1 = MAX(0, r.tl.x);
		int y1 = MAX(0, r.tl.y);
		int x2 = MIN(width()-1, r.br.x);
		int y2 = MIN(height()-1, r.br.y);
		int w  = MAX(0, x2-x1+1);
		int h  = MAX(0, y2-y1+1);

		if ( w > 0 && h > 0 )
		{
			//SDL_UpdateRect(SURFACE(this), x1, y1, w, h);
		}
	}
	
	//SDL_Flip(SURFACE(this));
}

void Window::raise()
{

}

void Window::setSubName(const char *sub)
{

}
