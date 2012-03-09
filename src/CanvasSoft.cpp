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

#define SCREEN_PITCH 	(512*2)
#define SCREEN_W		(512)
#define SCREEN_H		(272)

#include "CanvasSoft.h"

#define SURFACE(cANVASpTR) (m_state)

// extract RGB colour components as 8bit values from RGB888
#define R32(p) (((p)>>16)&0xff)
#define G32(p) (((p)>>8)&0xff)
#define B32(p) ((p)&0xff)

// extract RGB colour components as 8bit values from RGB565
#define R16(p) (((p)>>8)&0xf8)
#define G16(p) (((p)>>3)&0xfc)
#define B16(p) (((p)<<3)&0xf8)

#define R16G16B16_TO_RGB888(r,g,b) \
  ((((r)<<8)&0xff0000) | ((g)&0x00ff00) | (((b)>>8)))

#define R16G16B16_TO_RGB565(r,g,b) \
  ((Uint16)( (((r)&0xf800) | (((g)>>5)&0x07e0) | (((b)>>11))&0x001f) ))

#define RGB888_TO_RGB565(p) \
  ((Uint16)( (((p)>>8)&0xf800) | (((p)>>5)&0x07e0) | (((p)>>3)&0x001f) ))


extern unsigned char PaperPic[524288];
  
void ExtractRgb(uint32 c, int& r, int &g, int &b) 
{
	r = R32(c); g = G32(c); b = B32(c);
}

void ExtractRgb(uint16 c, int& r, int &g, int &b)
{
	r = R16(c); g = G16(c); b = B16(c);
}

template <typename PIX> 
inline void AlphaBlend(PIX& p, int cr, int cg, int cb, int a, int ia)
{
}

inline void AlphaBlend(Uint16& p, int cr, int cg, int cb, int a, int ia)
{
	p = R16G16B16_TO_RGB565(a * cr + ia * R16(p),a * cg + ia * G16(p),a * cb + ia * B16(p));
}

inline void AlphaBlend(Uint32& p, int cr, int cg, int cb, int a, int ia)
{
	p = R16G16B16_TO_RGB888(a * cr + ia * R32(p),a * cg + ia * G32(p),a * cb + ia * B32(p));
}

#define ALPHA_MAX 0xff

template <typename PIX, unsigned W>
struct AlphaBrush
{
	int m_r, m_g, m_b, m_c;
	inline AlphaBrush(PIX c)
	{
		m_c = c;
		ExtractRgb(c, m_r, m_g, m_b);
	}
	inline void ink(PIX* pix, int step, int a) 
	{
		int ia = ALPHA_MAX - a;
		int o=-W/2+1;
		AlphaBlend(*(pix+o*step), m_r, m_g, m_b, a, ia);
		for ( ; o<=W/2; o++)
		{
			*(pix+o*step) = m_c;
		} 
		AlphaBlend(*(pix+o*step), m_r, m_g, m_b, ia, a);
	}
};

template <typename PIX>
struct AlphaBrush<PIX,1>
{
	int m_r, m_g, m_b, m_c;
	inline AlphaBrush(PIX c)
	{
		m_c = c;
		ExtractRgb(c, m_r, m_g, m_b);
	}
	inline void ink(PIX* pix, int step, int a) 
	{
		int ia = ALPHA_MAX - a;
		AlphaBlend(*(pix-step), m_r, m_g, m_b, a, ia);
		AlphaBlend(*(pix), m_r, m_g, m_b, ia, a);
	}
};

template <typename PIX>
struct AlphaBrush<PIX,3>
{
	int m_r, m_g, m_b, m_c;
	inline AlphaBrush(PIX c)
	{
		m_c = c;
		ExtractRgb(c, m_r, m_g, m_b);
	}
	inline void ink(PIX* pix, int step, int a) 
	{
		int ia = ALPHA_MAX - a;
		AlphaBlend(*(pix-step), m_r, m_g, m_b, a, ia);
		*(pix) = m_c;
		AlphaBlend(*(pix+step), m_r, m_g, m_b, ia, a);
	}
};



template <typename PIX, unsigned THICK> 
void renderLine(void *buf,int byteStride,int x1, int y1, int x2, int y2,PIX color)
{
	PIX *pix = (PIX*)((char*)buf+byteStride*y1) + x1;
	int lg_delta, sh_delta, cycle, lg_step, sh_step;
	int alpha, alpha_step, alpha_reset;
	int pixStride = byteStride/sizeof(PIX);
	AlphaBrush<PIX,THICK> brush(color);

	lg_delta = x2 - x1;
	sh_delta = y2 - y1;
	lg_step = SGN(lg_delta);
	lg_delta = ABS(lg_delta);
	sh_step = SGN(sh_delta);
	sh_delta = ABS(sh_delta);
	if (sh_step < 0)  pixStride = -pixStride;

	if (sh_delta < lg_delta)
	{
		cycle = lg_delta >> 1;
		alpha = ALPHA_MAX >> 1;
		alpha_step = -(ALPHA_MAX * sh_delta/(lg_delta+1));
		alpha_reset = alpha_step < 0 ? ALPHA_MAX : 0;
		int count = lg_step>0 ? x2-x1 : x1-x2;
		
		while (count--)
		{
			brush.ink(pix, pixStride, alpha);
			cycle += sh_delta;
			alpha += alpha_step;
			pix += lg_step;
			if (cycle > lg_delta)
			{
				cycle -= lg_delta;
				alpha = alpha_reset;
				pix += pixStride;
			}
		}
	}
	else
	{
		cycle = sh_delta >> 1;
		alpha = ALPHA_MAX >> 1;
		alpha_step = -lg_step * ABS(ALPHA_MAX * lg_delta/(sh_delta+1));
		alpha_reset = alpha_step < 0 ? ALPHA_MAX : 0;
		int count = sh_step>0 ? y2-y1 : y1-y2;
		
		while (count--)
		{
			brush.ink(pix, 1, alpha);
			cycle += lg_delta;
			alpha += alpha_step;
			pix += pixStride;
			if (cycle > sh_delta)
			{
				cycle -= sh_delta;
				alpha = alpha_reset;
				pix += lg_step;
			}
		}
	}
}

CanvasSoft::CanvasSoft(int w, int h):m_state(NULL),m_bgColour(0),m_bgImage(NULL)
{
	m_state = (char*)malloc(512*272*2);
	setClip(0, 0, width(), height());
}


CanvasSoft::CanvasSoft(State state):m_state(state),m_bgColour(0),m_bgImage(NULL)
{
	resetClip();
}

CanvasSoft::~CanvasSoft()
{
	free(m_state);
}

int CanvasSoft::width() const
{
	return SCREEN_W;
}

int CanvasSoft::height() const
{
	return SCREEN_H;
}

int CanvasSoft::makeColour(int r, int g, int b) const
{
	return 0;//47104;//SDL_MapRGB(SURFACE(this)->format, r, g, b);
}

int CanvasSoft::makeColour(int c) const
{
	return Color8888To5650(c);//RGB888_TO_RGB565(c);//47104;//SDL_MapRGB(SURFACE(this)->format,(c>>16)&0xff, (c>>8)&0xff, (c>>0)&0xff);
}

void CanvasSoft::resetClip()
{
	if (m_state)
	{
		setClip(0, 0, width(), height());
	}
	else
	{
		setClip(0, 0, 0, 0);
	}
}

void CanvasSoft::setClip(int x, int y, int w, int h)
{
	m_clip = Rect(x,y,x+w-1,y+h-1);
}

void CanvasSoft::setBackground(int c)
{
	m_bgColour = 0xFFFF;//c;
}

void CanvasSoft::setBackground(CanvasSoft* bg)
{
	m_bgImage = bg;
}

void CanvasSoft::clear()
{
	memcpy(m_state,PaperPic,SCREEN_W*SCREEN_H*2);
}

void CanvasSoft::fade() 
{

}

void* CanvasSoft::scale(int factor) const
{
	return m_state;
}

void CanvasSoft::clear(const Rect& r)
{	

}

void CanvasSoft::drawImage(CanvasSoft *canvas, int x, int y)
{

}

void CanvasSoft::drawPixel(int x, int y, int c)
{
	Uint32 bpp, ofs;

	bpp = 2;//SURFACE(this)->format->BytesPerPixel;
	ofs = SCREEN_PITCH*y;//SURFACE(this)->pitch*y;
	char* row = (char*)m_state+ofs;//(char*)SURFACE(this)->pixels + ofs;

	switch(bpp)
	{
		case 2: ((Uint16*)row)[x] = c; break;
		case 4: ((Uint32*)row)[x] = c; break;
	}
}

int CanvasSoft::readPixel(int x, int y) const
{
	Uint32 bpp, ofs;
	int c;

	bpp = 2;//SURFACE(this)->format->BytesPerPixel;
	ofs = SCREEN_PITCH*y;//SURFACE(this)->pitch*y;
	char* row = (char*)m_state+ofs;//(char*)SURFACE(this)->pixels + ofs;

	switch(bpp)
	{
		case 2: c = ((Uint16*)row)[x]; break;
		case 4: c = ((Uint32*)row)[x]; break;
		default: c=0; break;
	}

	return c;
}

void CanvasSoft::drawLine(int x1, int y1, int x2, int y2, int color)
{
	int lg_delta, sh_delta, cycle, lg_step, sh_step;
	lg_delta = x2 - x1;
	sh_delta = y2 - y1;
	lg_step = SGN(lg_delta);
	lg_delta = ABS(lg_delta);
	sh_step = SGN(sh_delta);
	sh_delta = ABS(sh_delta);
	if(sh_delta < lg_delta)
	{
		cycle = lg_delta >> 1;
		while (x1 != x2)
		{
			drawPixel(x1, y1, color);
			cycle += sh_delta;
			if (cycle > lg_delta)
			{
				cycle -= lg_delta;
				y1 += sh_step;
			}
			x1 += lg_step;
		}
		drawPixel(x1, y1, color);
	}
	cycle = sh_delta >> 1;
	while (y1 != y2)
	{
		drawPixel(x1, y1, color);
		cycle += lg_delta;
		if(cycle > sh_delta)
		{
			cycle -= sh_delta;
			x1 += lg_step;
		}
		y1 += sh_step;
	}
	drawPixel(x1, y1, color);
}

void CanvasSoft::drawPath(const Path& path, int color, bool thick)
{
	Rect clip = m_clip;
	clip.tl.x++; clip.tl.y++;
	clip.br.x--; clip.br.y--;

	int i=0;
	const int n = path.numPoints();

	for ( ;i<n && !clip.contains(path.point(i));i++)
	{
	}
	i++;
	for ( ;i<n;i++)
	{
		const Vec2& p2 = path.point(i);
		if (clip.contains(p2))
		{
			const Vec2& p1 = path.point(i-1);
			renderLine<Uint16,3>(SURFACE(this),SCREEN_PITCH,p1.x, p1.y, p2.x, p2.y, color);
			//DEBUG2(p1.x, p1.y, p2.x, p2.y);
		}
		else
		{
			for ( ;i<n && !clip.contains( path.point(i) ); i++)
			{
			}
		}
	}
}

void CanvasSoft::drawRect(int x, int y, int w, int h, int c, bool fill)
{

}

void CanvasSoft::drawRect2(char *dst,int x, int y, int w, int h, int c, bool fill)
{

}

void CanvasSoft::drawRect(const Rect& r, int c, bool fill)
{
	drawRect(r.tl.x, r.tl.y, r.br.x-r.tl.x, r.br.y-r.tl.y, c, fill);
}

void CanvasSoft::drawWorldLine(b2Vec2 pos1, b2Vec2 pos2, int color, bool thick)
{
	if(pos1.y < -5.0 || pos1.y >= 45.0 || pos2.y < -5.0 || pos2.y >= 45.0  || pos1.x <= -40.0 || pos1.x >= 40.0 || pos2.x <= -40.0 || pos2.x >= 40.0)
	{
		return; 
	} 

	if(thick)
	{
		b2Vec2 dir = (pos2 - pos1);
		dir.Normalize();
		b2Vec2 norm(-dir.y, dir.x);
		norm = 1/PIXELS_PER_METREf * norm;

		drawLine((int)(pos1.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),(int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos1.y*PIXELS_PER_METREf),
				(int)(pos2.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),(int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos2.y*PIXELS_PER_METREf),color);	
		pos1 += norm; pos2 += norm;
		drawLine((int)(pos1.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),(int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos1.y*PIXELS_PER_METREf),
				(int)(pos2.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),(int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos2.y*PIXELS_PER_METREf),color);	
		pos1 -= norm; pos2 -= norm;
		pos1 -= norm; pos2 -= norm;
		drawLine((int)(pos1.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),(int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos1.y*PIXELS_PER_METREf),
				(int)(pos2.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),(int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos2.y*PIXELS_PER_METREf),color);	
	}
	else
	{
		drawLine((int)(pos1.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),(int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos1.y*PIXELS_PER_METREf),
				(int)(pos2.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),(int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos2.y*PIXELS_PER_METREf),color);	
	}
}


void CanvasSoft::drawWorldPath(const Path& path, int color, bool thick)
{
	for(int i=1; i<path.numPoints(); i++)
	{
		drawWorldLine(path.point(i-1), path.point(i), color, thick);
	}  
}

int CanvasSoft::writeBMP(const char* filename) const
{

}
