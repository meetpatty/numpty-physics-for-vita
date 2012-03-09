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

#define SCREEN_PITCH 	(512*2)
#define SCREEN_W		(512)
#define SCREEN_H		(272)

//int i_fade = 0;

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

void Canvas::clear()
{
	sceGuTexMode(GU_COLOR_5650,0,0,0);
	if(b_fade) sceGuTexImage(0,512,512,512,PaperDarkPic);
		else sceGuTexImage(0,512,512,512,PaperPic);
	//sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA);
	sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
	sceGuTexFilter(GU_LINEAR,GU_LINEAR);
	sceGuTexScale(1.0f,1.0f);
	sceGuTexOffset(0.0f,0.0f);
	sceGuAmbientColor(0xffffffff);
		
	sceGuColor(0xffffffff);
	struct VertexUV* vertices = (struct VertexUV*)sceGuGetMemory(2 * sizeof(struct VertexUV));
	vertices[0].u = 0; vertices[0].v = 0;
	vertices[0].x = 0; vertices[0].y = 0; vertices[0].z = 0;
	vertices[1].u = 512; vertices[1].v = 272;
	vertices[1].x = 512; vertices[1].y = 274; vertices[1].z = 0;
	sceGuDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_COLOR_5650|GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,vertices);

	//sceGuTexFilter(0,0);
	sceGuTexFunc(0,0);
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
	//sceGuTexFunc(0,0);
	sceGuColor(color);
	struct Vertex* vertices = (Vertex*)sceGuGetMemory(2 * sizeof(struct Vertex));
	vertices[0].x = x1;
	vertices[0].y = y1;
	vertices[0].z = 0;
	vertices[1].x = x2;
	vertices[1].y = y2;
	vertices[1].z = 0;
	sceGuDrawArray(GU_LINES,GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,vertices);
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
		sceGuTexFunc(0,0);
		sceGuColor(c);
		struct Vertex* vertices = (Vertex*)sceGuGetMemory(6 * sizeof(struct Vertex));
		vertices[0].x = x;
		vertices[0].y = y;
		vertices[0].z = 0;
		vertices[1].x = x+w;
		vertices[1].y = y;
		vertices[1].z = 0;
		vertices[2].x = x+w;
		vertices[2].y = y+h;
		vertices[2].z = 0;		
		vertices[3].x = x;
		vertices[3].y = y;
		vertices[3].z = 0;
		vertices[4].x = x+w;
		vertices[4].y = y+h;
		vertices[4].z = 0;
		vertices[5].x = x;
		vertices[5].y = y+h;
		vertices[5].z = 0;
		sceGuDrawArray(GU_TRIANGLES,GU_VERTEX_32BITF|GU_TRANSFORM_2D,6,0,vertices);	
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
	sceGuTexMode(GU_COLOR_5650,0,0,0);
	sceGuTexImage(0,128,256,128,EditPic);
	//sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA);
	sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
	sceGuTexFilter(GU_LINEAR,GU_LINEAR);
	sceGuTexScale(1.0f,1.0f);
	sceGuTexOffset(0.0f,0.0f);
	sceGuAmbientColor(0xffffffff);
		
	sceGuColor(0xffffffff);
	struct VertexUV* vertices = (struct VertexUV*)sceGuGetMemory(2 * sizeof(struct VertexUV));
	vertices[0].u = 0; vertices[0].v = 0;
	vertices[0].x = x; vertices[0].y = y; vertices[0].z = 0;
	vertices[1].u = 128; vertices[1].v = 256;
	vertices[1].x = x+100-1; vertices[1].y = y+200-1; vertices[1].z = 0;
	sceGuDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_COLOR_5650|GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,vertices);
	//sceGuTexFilter(0,0);
	sceGuTexFunc(0,0);
}

void Canvas::drawPause(int x, int y)
{
	sceGuTexMode(GU_COLOR_5650,0,0,0);
	sceGuTexImage(0,32,32,32,PausePic);
	//sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA);
	sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
	sceGuTexFilter(GU_LINEAR,GU_LINEAR);
	sceGuTexScale(1.0f,1.0f);
	sceGuTexOffset(0.0f,0.0f);
	sceGuAmbientColor(0xffffffff);
		
	sceGuColor(0xffffffff);
	struct VertexUV* vertices = (struct VertexUV*)sceGuGetMemory(2 * sizeof(struct VertexUV));
	vertices[0].u = 0; vertices[0].v = 0;
	vertices[0].x = x; vertices[0].y = y; vertices[0].z = 0;
	vertices[1].u = 32; vertices[1].v = 32;
	vertices[1].x = x+32-1; vertices[1].y = y+32-1; vertices[1].z = 0;
	sceGuDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_COLOR_5650|GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,vertices);
	//sceGuTexFilter(0,0);
	sceGuTexFunc(0,0);
}

void Canvas::drawNext(int x, int y)
{
	sceGuTexMode(GU_COLOR_5650,0,0,0);
	sceGuTexImage(0,512,256,512,NextPic);
	//sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA);
	sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
	sceGuTexFilter(0,0);
	sceGuTexScale(1.0f,1.0f);
	sceGuTexOffset(0.0f,0.0f);
	sceGuAmbientColor(0xffffffff);
		
	sceGuColor(0xffffffff);
	struct VertexUV* vertices = (struct VertexUV*)sceGuGetMemory(2 * sizeof(struct VertexUV));
	vertices[0].u = 0; vertices[0].v = 0;
	vertices[0].x = x; vertices[0].y = y; vertices[0].z = 0;
	vertices[1].u = 320; vertices[1].v = 192;
	vertices[1].x = x+320-1; vertices[1].y = y+192-1; vertices[1].z = 0;
	sceGuDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_COLOR_5650|GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,vertices);
	//sceGuTexFilter(0,0);
	sceGuTexFunc(0,0);
}

void Canvas::drawImage(void *img, int x, int y, int w, int h)
{
	sceGuTexMode(GU_COLOR_5650,0,0,0);
	sceGuTexImage(0,512,256,512,img);
	//sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA);
	sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
	sceGuTexFilter(GU_LINEAR,GU_LINEAR);
	sceGuTexScale(1.0f,1.0f);
	sceGuTexOffset(0.0f,0.0f);
	sceGuAmbientColor(0xffffffff);
		
	sceGuColor(0xffffffff);
	struct VertexUV* vertices = (struct VertexUV*)sceGuGetMemory(2 * sizeof(struct VertexUV));
	vertices[0].u = 0; vertices[0].v = 0;
	vertices[0].x = x; vertices[0].y = y; vertices[0].z = 0;
	vertices[1].u = 512; vertices[1].v = 256;
	vertices[1].x = x+w-1; vertices[1].y = y+h-1; vertices[1].z = 0;
	sceGuDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_COLOR_5650|GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,vertices);
	//sceGuTexFilter(0,0);
	sceGuTexFunc(0,0);
}
