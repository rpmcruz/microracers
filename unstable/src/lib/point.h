/***************************************************************************
                    point.h  -  stores point/rectangle info
                             -------------------
    begin                : Sun Oct 30 2004
    copyright            : (C) 2004 by Ricardo Cruz
    email                : rpmcruz@clix.pt
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
#include <algorithm>

/* Point and Rect classes are in same file cause they're small and
   when one is needed, most likely, the other would also be used. */

// TODO: add some relations between Point and Rect
// ie. Rect r = Rect(10,20,5,5); Point p = r;

struct Rect;
struct Pointf;

struct Point
  {
  Point(int x_, int y_)
    : x(x_), y(y_)
    { }
  Point(const Point& p)
    : x(p.x), y(p.y)
    { }
  Point(const Pointf& p);
  Point(const Rect& r);
  Point()
    : x(0), y(0)
    { }

  bool operator ==(const Point& p) const
    { return x == p.x && y == p.y; }
  bool operator !=(const Point& p) const
    { return !(x == p.x && y == p.y); }

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

  Point operator + (int num) const
    { return Point(x + num, y + num); }
  Point operator / (int num) const
    { return Point(x / num, y / num); }
  Point operator * (int num) const
    { return Point(x * num, y * num); }
  Point operator * (float num) const
    { return Point((int)(x * num), (int)(y * num)); }

  const Point& operator *= (int n)
    { x *= n; y *= n; return *this; }
  const Point& operator /= (int n)
    { x /= n; y /= n; return *this; }

  Point operator - () const
    { return Point(-x, -y); }

  SDL_Rect sdl_rect() const
    { SDL_Rect r; r.x = x; r.y = y; r.w = r.h = 0; return r; }

  int x, y;
  };

/* The same as Point, but uses floats for its x and y variables.
   Useful to be used on physics and stuff.
   The reason why I don't join the both into one is because someone may
   want to run Microracers on a mobile device who don't deal that well with
   floats because of the lack of a CPU. This reduces the problem. */

struct Pointf
  {
  Pointf(float x_, float y_)
    : x(x_), y(y_)
    { }
  Pointf(const Point& p)
    : x(p.x), y(p.y)
    { }
  Pointf(const Pointf& p)
    : x(p.x), y(p.y)
    { }
  Pointf(const Rect& r);
  Pointf()
    : x(0), y(0)
    { }

  bool operator ==(const Point& p) const
    { return x == p.x && y == p.y; }
  bool operator !=(const Point& p) const
    { return !(x == p.x && y == p.y); }

  bool operator > (const Pointf& p) const
    { return norm() > p.norm(); }
  bool operator < (const Pointf& p) const
    { return norm() < p.norm(); }


  const Pointf& operator = (const Pointf p)
    { x = p.x; y = p.y; return *this; }
  void operator = (int i)
    { x = i; }

  Pointf operator + (const Pointf& p) const
    { return Pointf(x + p.x, y + p.y); }
  Pointf operator - (const Pointf& p) const
    { return Pointf(x - p.x, y - p.y); }
  Pointf operator * (const Pointf& p) const
    { return Pointf(x * p.x, y * p.y); }
  Pointf operator / (const Pointf& p) const
    { return Pointf(x / p.x, y / p.y); }

  const Pointf& operator += (const Pointf& p)
    { x += p.x; y += p.y; return *this; }
  const Pointf& operator -= (const Pointf& p)
    { x -= p.x; y -= p.y; return *this; }
  const Pointf& operator *= (const Pointf& p)
    { x *= p.x; y *= p.y; return *this; }
  const Pointf& operator /= (const Pointf& p)
    { x /= p.x; y /= p.y; return *this; }

  Pointf operator + (float num) const
    { return Pointf(x + num, y + num); }
  Pointf operator / (float num) const
    { return Pointf(x / num, y / num); }
  Pointf operator * (float num) const
    { return Pointf(x * num, y * num); }

  Pointf operator - () const
    { return Pointf(-x, -y); }

  /* This ain't really the norm as there is no sqrt(), but for the current
     purpose, it works. */
  float norm() const
    { return x*x + y*y; }

  float x, y;
  };

struct Rect
  {
  Rect(int x_, int y_, int w_, int h_)
    : x(x_), y(y_), w(w_), h(h_)
    { }
  Rect(const Pointf& p1, const Point& p2)
    : x((int)p1.x), y((int)p1.y), w(p2.x), h(p2.y)
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

  const Rect& operator += (const Point& p)
    { x += p.x; y += p.y; return *this; }
  const Rect& operator -= (const Point& p)
    { x -= p.x; y -= p.y; return *this; }

  Rect operator + (const Point& p) const
    { return Rect(x + p.x, y + p.y, w, h); }
  Rect operator - (const Point& p) const
    { return Rect(x - p.x, y - p.y, w, h); }

  Rect operator * (float num) const
    { return Rect((int)(x * num), (int)(y * num), w, h); }

  SDL_Rect sdl_rect() const
    { SDL_Rect r; r.x = x; r.y = y; r.w = w; r.h = h; return r; }

  /* Check if there is a collision on those "boxes". */
  static bool test_collision(const Rect& rect1, const Rect& rect2)
    {
    // very simple box collision detection
    return !(rect1.x + rect1.w < rect2.x || rect2.x + rect2.w < rect1.x ||
            rect1.y + rect1.h < rect2.y || rect2.y + rect2.h < rect1.y);
    }
  /* A point with a rect now. */
  static bool test_collision(const Point& point, const Rect& rect)
    {
    return rect.x <= point.x && rect.x+rect.w > point.x &&
             rect.y <= point.y && rect.y+rect.h > point.y;
    }
  /* Check if "exterior" is really exterior to "inside". */
  static bool is_inside(const Rect& inside, const Rect& exterior)
    {
    return inside.x >= exterior.x && inside.x+inside.w <= exterior.x+exterior.w &&
             inside.y >= exterior.y && inside.y+inside.h <= exterior.y+exterior.h;
    }

  int x, y, w, h;
  };

#endif
