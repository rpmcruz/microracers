/***************************************************************************
                          color.h  -  stores color info
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

#ifndef COLOR_H
#define COLOR_H

#include <SDL.h>

class Color
{
public:
  Color()
    : red(0), green(0), blue(0), alpha(255)
    {}

  Color(int red_, int green_, int blue_, int alpha_ = 255)
    : red(red_), green(green_), blue(blue_), alpha(alpha_)
    {}


   Color(const Color& c)
     : red(c.red), green(c.green), blue(c.blue), alpha(c.alpha)
     {}

    bool operator == (const Color& c)
      { return red == c.red && green == c.green &&
               blue == c.blue && alpha == c.alpha; }

    Uint32 map_rgb(SDL_Surface* surface) const
      { return SDL_MapRGB(surface->format, red, green, blue); }
    Uint32 map_rgba(SDL_Surface* surface) const
      { return SDL_MapRGBA(surface->format, red, green, blue, alpha); }


    int red, green, blue, alpha;
};

#endif
