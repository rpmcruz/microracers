/***************************************************************************
                          font.h  -  handles a font
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

#ifndef FONT_H
#define FONT_H

#include <string>

#include "color.h"
#include "point.h"

enum Allignment {
  LEFT_ALLIGN,
  CENTER_ALLIGN,
  RIGHT_ALLIGN
  };

class Surface;

#define TOTAL_GLYPHS 96
#define GLYPHS_COLS 16
#define GLYPHS_ROWS 6

class Font
  {
  public:
    Font(const std::string& filename);
    ~Font();

    void draw_text(const std::string& text, const Color& color, const Point& point,
                   Allignment allign = LEFT_ALLIGN);
    void draw_text(const std::string& text, const Point& point,
                   Allignment allign = LEFT_ALLIGN, int alpha = 255);

    int get_height()
      { return size; }
    int get_text_width(const std::string& text)
      { return (text.size()-1)*size; }

  private:
    void load_font(const std::string& filename);

    int draw_char(char ch, int x, int y);

    int index_x_position(int i);
    int index_y_position(int i);

    SDL_Surface* surface;
    int glyph_width[96];
    int size;
  };

#endif
