/***************************************************************************
                          font.h  -  handles a font
                             -------------------
    begin                : ¦un Oct 30 2004
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

#ifndef FONT_H
#define FONT_H

#include <SDL_ttf.h>
#include <string>

#include "color.h"
#include "point.h"

enum Allignment {
  LEFT_ALLIGN,
  CENTER_ALLIGN,
  RIGHT_ALLIGN
  };

class Surface;

class Font
{
public:
  Font(const std::string& filename, int size);
  ~Font();

  void load_font(const std::string& filename, int size);

  void draw_text(const std::string& text, const Color& color, const Point& point, Allignment allign = LEFT_ALLIGN);

  // draw text into specified surface
  void draw_text_surface(SDL_Surface* surface, const std::string& text, const Color& color, const Point& point, Allignment allign = LEFT_ALLIGN);

  int get_height()
    { return size; }
  int get_text_width(const std::string& text)
    { return (text.size()-1)*size; }

private:
  TTF_Font* ttf_font;
  int size;
};

#endif
