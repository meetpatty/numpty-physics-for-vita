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

#ifndef __PATH_H__
#define __PATH_H__

#include "Common.h"
#include "Array.h"
#include "Segment.h"

class Path : public Array<Vec2>
{

public:
  Path();
  Path( int n, Vec2* p );

  void makeRelative();
  Path& translate(const Vec2& xlate);
  Path& rotate(const b2Mat22& rot);
  Path& scale(float factor);

  inline Vec2& origin() { return at(0); }

  inline Path& operator&(const Vec2& other) 
  {
    append(other);
    return *this; 
  }
  
  inline Path& operator&(const b2Vec2& other) 
  {
    append(Vec2(other));
    return *this; 
  }
  
  inline Path operator+(const Vec2& p) const
  {
    Path r( *this );
    return r.translate( p );
  }

  inline Path operator-(const Vec2& p) const
  {
    Path r( *this );
    Vec2 n( -p.x, -p.y );
    return r.translate( n );
  }

  inline Path operator*(const b2Mat22& m) const
  {
    Path r( *this );
    return r.rotate( m );
  }

  inline Path& operator+=(const Vec2& p) 
  {
    return translate( p );
  }

  inline Path& operator-=(const Vec2& p) 
  {
    Vec2 n( -p.x, -p.y );
    return translate( n );
  }

  inline int   numPoints() const { return size(); }
  inline const Vec2& point(int i) const { return at(i); }
  inline Vec2& point(int i) { return at(i); }
  inline Vec2& first() { return at(0); }
  inline Vec2& last() { return at(size()-1); }

  void simplify( float threshold );
  Rect bbox() const;

 private:
  void simplifySub(int first, int last, float threshold, bool* keepflags);  
};

#endif //PATH_H
