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

#include "Canvas.h"
#include "Pics.h"
#include <vita2d.h>

#define SCREEN_PITCH 	(960*2)
#define SCREEN_W		(960)
#define SCREEN_H		(544)

//int i_fade = 0;

vita2d_texture *paper_pic, *paper_pic_dark, *pause_pic, *next_pic, *img_pic, *edit_pic;
unsigned short *paper_pic_data, *paper_pic_dark_data, *pause_pic_data, *next_pic_data, *img_pic_data, *edit_pic_data;

struct Vertex
{
	float x,y,z;
};

struct VertexUV
{
	float u, v;
	unsigned int color;
	float x,y,z;
};

struct Vertex __attribute__((aligned(16))) plane[2*3] =
{
	{2,		1,	0},
	{-1,	1,	0},
	{-1,	-1,	0},
	{-1,	-1, 0},
	{2,		-1, 0},
	{2,		1,	0}
};

struct VertexUV __attribute__((aligned(16))) vertices[2*3] =
{
	{0, 0, 0, 4.9,		1.95, 	0},
	{1, 0, 0, -1.95,	1.95, 	0},
	{1, 1, 0, -1.9,		-1.95, 	0},
	{1, 1, 0, -1.95,	-1.95,	0},
	{0, 1, 0, 4.9,		-1.95, 	0},
	{0, 0, 0, 4.9,		1.95, 	0}
};

Canvas::Canvas(int w, int h):m_state(NULL),m_bgColour(0),m_bgImage(NULL)
{
	b_fade = false;
	resetClip();
}


Canvas::Canvas(State state):m_state(state),m_bgColour(0),m_bgImage(NULL)
{
	b_fade = false;
	resetClip();


}

Canvas::~Canvas()
{
}

int Canvas::width() const
{
	return SCREEN_W;
}

int Canvas::height() const
{
	return SCREEN_H;
}

int Canvas::makeColour(int c) const
{
	if (b_fade) return c&0xFFBBAABB;
		else return c;
}

void Canvas::resetClip()
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

void Canvas::setClip(int x, int y, int w, int h)
{
	m_clip = Rect(x,y,x+w-1,y+h-1);
}

void Canvas::setBackground(int c)
{
	m_bgColour = c;
}

void Canvas::setBackground(Canvas* bg)
{
	m_bgImage = bg;
}

void Canvas::LoadAssets()
{

	paper_pic = vita2d_create_empty_texture_format(512, 512, SCE_GXM_TEXTURE_FORMAT_U5U6U5_BGR);
	paper_pic_data = (unsigned short*)vita2d_texture_get_datap(paper_pic);

	for (int i = 0; i < 512; i++)
	{
		for (int j = 0; j < 512; j++)
			paper_pic_data[j + 512 * i] = PaperPic[j * 2 + 512 * i * 2] | PaperPic[j * 2 + 1 + 512 * i * 2] << 8;
	}

	paper_pic_dark = vita2d_create_empty_texture_format(512, 512, SCE_GXM_TEXTURE_FORMAT_U5U6U5_BGR);
	paper_pic_dark_data = (unsigned short*)vita2d_texture_get_datap(paper_pic_dark);

	for (int i = 0; i < 512; i++)
	{
		for (int j = 0; j < 512; j++)
			paper_pic_dark_data[j + 512 * i] = PaperDarkPic[j * 2 + 512 * i * 2] | PaperDarkPic[j * 2 + 1 + 512 * i * 2] << 8;
	}

	next_pic = vita2d_create_empty_texture_format(320, 192, SCE_GXM_TEXTURE_FORMAT_U5U6U5_BGR);
	next_pic_data = (unsigned short*)vita2d_texture_get_datap(next_pic);

	for (int i = 0; i < 192; i++)
	{
		for (int j = 0; j < 320; j++)
			next_pic_data[j + 320 * i] = NextPic[j * 2 + 512 * i * 2] | NextPic[j * 2 + 1 + 512 * i * 2] << 8;
	}

	img_pic = vita2d_create_empty_texture_format(960, 544, SCE_GXM_TEXTURE_FORMAT_U5U6U5_BGR);
	img_pic_data = (unsigned short*)vita2d_texture_get_datap(img_pic);

	edit_pic = vita2d_create_empty_texture_format(128, 256, SCE_GXM_TEXTURE_FORMAT_U5U6U5_BGR);
	edit_pic_data = (unsigned short*)vita2d_texture_get_datap(edit_pic);

	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 128; j++)
			edit_pic_data[j + 128 * i] = EditPic[j * 2 + 128 * i * 2] | EditPic[j * 2 + 1 + 128 * i * 2] << 8;
	}

	pause_pic = vita2d_create_empty_texture_format(32, 32, SCE_GXM_TEXTURE_FORMAT_U5U6U5_BGR);
	pause_pic_data = (unsigned short*)vita2d_texture_get_datap(pause_pic);

	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
			pause_pic_data[j + 32 * i] = PausePic[j * 2 + 32 * i * 2] | PausePic[j * 2 + 1 + 32 * i * 2] << 8;
	}

}

void Canvas::clear()
{
	if (b_fade) vita2d_draw_texture_scale(paper_pic_dark, 0.0f, 0.0f, 2.0f, 2.0f);
	else vita2d_draw_texture_scale(paper_pic, 0.0f, 0.0f, 2.0f, 2.0f);
}

void Canvas::fade(bool f) 
{
	b_fade = f;
}

Canvas* Canvas::scale(int factor) const
{

}

void Canvas::clear(const Rect& r)
{	

}

void Canvas::drawImage(Canvas *canvas, int x, int y)
{

}

void Canvas::drawLine(int x1, int y1, int x2, int y2, int color)
{
	vita2d_draw_line(x1, y1, x2, y2, color);
}

void Canvas::drawPath(const Path& path, int color, bool thick)
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
			drawLine(p1.x, p1.y, p2.x, p2.y, color);
			drawLine(p1.x+1, p1.y, p2.x+1, p2.y, color&0xFFEEFFEE);
			drawLine(p1.x, p1.y+1, p2.x, p2.y+1, color&0xFFFFEEFF);
		}
		else
		{
			for ( ;i<n && !clip.contains( path.point(i) ); i++)
			{
			}
		}
	}
}

void Canvas::drawRect(int x, int y, int w, int h, int c, bool fill)
{
	if(fill)
	{
		vita2d_draw_rectangle(x, y, w, h, c);
	}
	else
	{
		drawLine(x, y, x+w, y, c);
		drawLine(x+w, y, x+w, y+h, c);
		drawLine(x+w, y+h, x, y+h, c);
		drawLine(x, y+h, x, y, c);
	}
}

void Canvas::drawRect(const Rect& r, int c, bool fill)
{
	drawRect(r.tl.x, r.tl.y, r.br.x-r.tl.x, r.br.y-r.tl.y, c, fill);
}

void Canvas::drawWorldLine(b2Vec2 pos1, b2Vec2 pos2, int color, bool thick)
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


void Canvas::drawWorldPath(const Path& path, int color, bool thick)
{
	for(int i=1; i<path.numPoints(); i++)
	{
		drawWorldLine(path.point(i-1), path.point(i), color, thick);
	}  
}

void Canvas::drawEdit(int x, int y)
{
	vita2d_draw_texture(edit_pic, x, y);
}

void Canvas::drawPause(int x, int y)
{
	vita2d_draw_texture_scale(pause_pic, x, y, 2.0f, 2.0f);
}

void Canvas::drawNext(int x, int y)
{
	vita2d_draw_texture_scale(next_pic, x, y, 2.0f, 2.0f);
}

void Canvas::drawImage(void *img, int x, int y, int w, int h)
{
	memcpy(img_pic_data, img, 960 * 544 * 2);
	vita2d_draw_texture_scale(img_pic, x, y, w * 2.0f / 960, h * 2.0f / 544);
}
