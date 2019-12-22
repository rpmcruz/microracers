/***************************************************************************
                          point.cpp  -  stores point/rectangle info
                             -------------------
    begin                : Tue Nov 9 2004
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

#include "point.h"
#include "color.h"

/* I don't want to create a color.cpp, so this goes here: */
const Color Color::white (255, 255, 255);
const Color Color::black (0, 0, 0);
const Color Color::pink  (255, 0, 255);
const Color Color::yellow(255, 255, 0);

/* Maybe we should do functions here instead of the declaration to speedup
   compilation time. */

/* Some functions we couldnt create cause the needed declarations weren't
   setup yet. */
Point::Point(const Pointf& p)
  : x((int)p.x), y((int)p.y) { }
Point::Point(const Rect& r)
  : x(r.x), y(r.y) { }
