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

#ifndef __COMMON_H__
#define __COMMON_H__

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspctrl.h>

const int brush_colours[] = {
	0xFF0000FF,//0xb80000, //red
	0x3300FFFF,//0xeec900, //yellow
	0xFFFF0000,//0x000077, //blue
	0xFF10BB10,//0x108710, //green
	0xFF101010,//0x101010, //black
	0xFF004DB8,//0x8b4513, //brown
	0xFFF4B469,//0x87cefa, //lightblue 
	0xFFA76AEE,//0xee6aa7, //pink
	0xFFFF00AA,//0xb23aee, //purple
	0xFF9AFA00,//0x00fa9a, //lightgreen
	0xFF0078FF,//0xff7f00, //orange
	0xFF9C958F//0x6c7b8b, //grey
};

#define NUM_COLOURS (sizeof(brush_colours)/sizeof(brush_colours[0]))

#define CUR_MINf 0.5f
#define CUR_MIDf 1.0f
#define CUR_MAXf 1.7f

#define STICK_MIN 94
#define STICK_MID 60
#define STICK_MAX 20

#define FAST_MOVE 3

#define COLOUR_RED     brush_colours[0]
#define COLOUR_YELLOW  brush_colours[1] 
#define COLOUR_BLUE    brush_colours[2] 

#if 1
#define DEBUG(fl,s,n)
#define DEBUG2(x1,x2,x3,x4)
#define DEBUG3(c1,c2,x1,c3)
#else
#define DEBUG(fl,s,n) \
	{ \
	char text[80]; \
	sprintf(text,"[%s] %s - %i\n",fl,s,n);\
	FILE *f = fopen("log.txt","a+"); \
	fwrite(text,strlen(text),1,f); \
	fclose(f);\
	}

#define DEBUG2(x1,x2,x3,x4) \
	{ \
	char text[80]; \
	sprintf(text,"drawLine( %i, %i, %i, %i)\n",x1,x2,x3,x4);\
	FILE *f = fopen("log.txt","a+"); \
	fwrite(text,strlen(text),1,f); \
	fclose(f);\
	}

#define DEBUG3(c1,c2,x1,c3) \
	{ \
	char text[80]; \
	sprintf(text,"[%s:%s:0x%X] %s\n",c1,c2,x1,c3);\
	FILE *f = fopen("log.txt","a+"); \
	fwrite(text,strlen(text),1,f); \
	fclose(f);\
	}
#endif

#include <Box2D/Box2D.h>
#define ARRAY_SIZE(aRR) (sizeof(aRR)/sizeof((aRR)[0]))
#define ASSERT(a)

#define FIXED_SHIFT 16
#define FIXED_ONE (1<<FIXED_SHIFT)
#define FLOAT_TO_FIXED(fLOAT) ((int)(fLOAT*(float)FIXED_ONE))
#define FIXED_TO_INT(iNT) ((iNT)>>FIXED_SHIFT)

struct Vec2 {
  Vec2() {}
  Vec2( const Vec2& o ) : x(o.x), y(o.y) {}
  explicit Vec2( const b2Vec2& o ) : x((int)o.x), y((int)o.y) {}
  Vec2( int xx, int yy ) : x(xx), y(yy) {}
  void operator+=( const Vec2& o ) { x+=o.x; y+=o.y; }
  void operator-=( const Vec2& o ) { x-=o.x; y-=o.y; }
  Vec2 operator-() { return Vec2(-x,-y); }
  void operator*=( int o ) { x*=o; y*=o; }
  bool operator==( const Vec2& o ) const { return x==o.x && y==y; }
  operator b2Vec2() const { return b2Vec2((float)x,(float)y); } 
  Vec2 operator+( const Vec2& b ) const { return Vec2(x+b.x,y+b.y); }
  Vec2 operator-( const Vec2& b ) const { return Vec2(x-b.x,y-b.y); }
  Vec2 operator/( int r ) const { return Vec2(x/r,y/r); }
  int x,y;
};

template <typename T> inline T MIN( T a, T b )
{
  return a < b ? a : b;
}

inline Vec2 MIN( const Vec2& a, const Vec2& b )
{
  Vec2 r;
  r.x = MIN(a.x,b.x);
  r.y = MIN(a.y,b.y);
  return r;
}

template <typename T> inline T MAX( T a, T b )
{
  return a >= b ? a : b;
}

inline Vec2 MAX( const Vec2& a, const Vec2& b )
{
  Vec2 r;
  r.x = MAX(a.x,b.x);
  r.y = MAX(a.y,b.y);
  return r;
}

#define SGN(a) ((a)<0?-1:1)
#define ABS(a) ((a)<0?-(a):(a))



struct Rect {
  Rect() {}
  Rect( const Vec2& atl, const Vec2& abr ) : tl(atl), br(abr) {} 
  Rect( int x1, int y1, int x2, int y2 ) : tl(x1,y1), br(x2,y2) {}
  void clear() { tl.x=tl.y=br.x=br.y=0; }
  bool isEmpty() { return tl.x==0 && br.x==0; }
  void expand( const Vec2& v ) { tl=MIN(tl,v); br=MAX(br,v); }
  void expand( const Rect& r ) { expand(r.tl); expand(r.br); }
  bool contains( const Vec2& p ) const {
    return p.x >= tl.x && p.x <= br.x && p.y >= tl.y && p.y <= br.y;
  }
  bool contains( const Rect& p ) const {
    return contains(p.tl) && contains(p.br);
  }
  bool intersects( const Rect& r ) const {
    return r.tl.x <= br.x
      && r.tl.y <= br.y
      && r.br.x >= tl.x 
      && r.br.y >= tl.y;
  }
  Vec2 centroid() const { return (tl+br)/2; }
  Rect operator+( const Vec2& b ) const {
    Rect r=*this;
    r.tl += b; r.br += b;
    return r;
  }
  Vec2 tl, br;
};


#endif //COMMON_H
