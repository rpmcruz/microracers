/***************************************************************************
                          point.h  -  stores point/rectangle info
                             -------------------
    begin                : un Oct 30 2004
    copyright            : (C) 2004 by Ricardo Cruz
    email                : rick2@aeiou.pt
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef POINT_H
#define POINT_H

#include <SDL.h>

/* Point and Rect classes are in same file cause they're small and
   when one is needed, most likely, the other would also be used. */

// TODO: add some relations between Point and Rect
// ie. Rect r = Rect(10,20,5,5); Point p = r;

class Rect;

class Point
{
public:
  Point(float x_, float y_)
    : x(x_), y(y_)
    { }
  Point(const Point& p)
    : x(p.x), y(p.y)
    { }
  Point(const Rect& r);
  Point()
    : x(0), y(0)
    { }

  bool operator ==(const Point& p) const
    { return x == p.x && y == p.y; }
  bool operator !=(const Point& p) const
    { return !(x == p.x && y == p.y); }

  bool operator > (const Point& p) const
    { return norm() > p.norm(); }
  bool operator < (const Point& p) const
    { return norm() < p.norm(); }

  const Point& operator = (const Point& p)
    { x = p.x; y = p.y; return *this; }
  void operator = (int i)
    { x = i; }

  Point operator + (const Point& p) const
    { return Point(x + p.x, y + p.y); }
  Point operator - (const Point& p) const
    { return Point(x - p.x, y - p.y); }
  Point operator * (const Point& p) const
    { return Point(x * p.x, y * p.y); }
  Point operator / (const Point& p) const
    { return Point(x / p.x, y / p.y); }

  const Point& operator += (const Point& p)
    { x += p.x; y += p.y; return *this; }
  const Point& operator -= (const Point& p)
    { x -= p.x; y -= p.y; return *this; }
  const Point& operator *= (const Point& p)
    { x *= p.x; y *= p.y; return *this; }
  const Point& operator /= (const Point& p)
    { x /= p.x; y /= p.y; return *this; }

  Point operator + (float num) const
    { return Point(x + num, y + num); }
  Point operator / (float num) const
    { return Point(x / num, y / num); }
  Point operator * (float num) const
    { return Point(x * num, y * num); }

  Point operator - () const
    { return Point(-x, -y); }

  SDL_Rect sdl_rect() const;

  float norm() const;

  float x, y;
};

class Rect
{
public:
  Rect(float x_, float y_, int w_, int h_)
    : x(x_), y(y_), w(w_), h(h_)
    { }
  Rect(const Rect& r)
    : x(r.x), y(r.y), w(r.w), h(r.h)
    { }
  Rect()
    : x(0), y(0), w(0), h(0)
    { }

  bool operator ==(const Rect& r) const
    { return x == r.x && y == r.y && w == r.w && h == r.h; }
  bool operator !=(const Rect& r) const
    { return !(x == r.x && y == r.y); }

  const Rect& operator=(const Rect& p)
    { x = p.x; y = p.y; w = p.w; h = p.h; return *this; }

  Rect operator + (const Point& p) const
    { return Rect(x + p.x, y + p.y, w, h); }

  SDL_Rect sdl_rect();

  float x, y;
  int w, h;
};

#endif
