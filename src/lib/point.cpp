/***************************************************************************
                          point.cpp  -  stores point/rectangle info
                             -------------------
    begin                : Tue Nov 9 2004
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

#include <math.h>

#include "point.h"

// Point
Point::Point(const Rect& r)
  : x(r.x), y(r.y)
{
}

SDL_Rect Point::sdl_rect() const
{
SDL_Rect rect;
rect.x = (int)x;
rect.y = (int)y;
rect.w = rect.h = 0;
return rect;
}

float Point::norm() const
{
return sqrt(x*x + y*y);
}

// Rect
SDL_Rect Rect::sdl_rect()
{
SDL_Rect rect;
rect.x = (int)x;
rect.y = (int)y;
rect.w = w;
rect.h = h;
return rect;
}
