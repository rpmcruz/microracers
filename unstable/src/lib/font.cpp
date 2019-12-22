/***************************************************************************
                          font.cpp  -  handles a font
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

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

#include "screen.h"
#include "font.h"
#include "primitives.h"

Font::Font(const std::string& filename)
  {
  load_font(filename);
  }

Font::~Font()
  {
  SDL_FreeSurface(surface);
  }

void Font::load_font(const std::string& filename)
  {
  surface = IMG_Load(filename.c_str());
  if(surface == NULL)
    {
    std::cerr << "Error: couldn't load font bitmap: " << filename << std::endl
              << "Reason: " << IMG_GetError() << std::endl;
    return;
    }

  size = surface->w / GLYPHS_COLS;

  // let's apply some colors to our 8-bit indexed surface
  if(surface->format->BitsPerPixel == 8)
    {
    SDL_Color colors[3] = { { 0, 0, 0} /* font */, { 255, 0, 255 } /* colorkey */,
                            { 255, 255, 255 } /* separators */ };
    if(SDL_SetColors(surface, colors, 0, 3) == 0)
      std::cerr << "Warning: not able to set pallet to font bitmap: " << filename << std::endl;
    }

  /* Let's find the width of this character, by looking at
     a possible white line. */
  int w, h = size;
  SDL_LockSurface(surface);
  for(int i = 0; i < TOTAL_GLYPHS; i++)
    {
    int pos_x = index_x_position(i), pos_y = index_y_position(i);
    for(w = 0; w < size; w++)
      {
      int y;
      for(y = 0; y < h; y++)
        if(Primitives::get_pixel(surface, pos_x + w, pos_y + y) != SDL_MapRGB(surface->format, 255,255,255))
          break;
      if(y == h)
        break;
      }
    glyph_width[i] = w;
    }
  SDL_UnlockSurface(surface);

  if(SDL_SetColorKey(surface, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(surface->format, 255,0,255)) == -1)
    std::cerr << "Warning: Could not set colorkey to fonts\n";
  }

int Font::index_x_position(int i)
  { return (i % GLYPHS_COLS) * size; }
int Font::index_y_position(int i)
  { return (i / GLYPHS_COLS) * size; }

void Font::draw_text(const std::string& text, const Color& color, const Point& point, Allignment allign)
  {
  SDL_Color colors[1] = { { color.red, color.green, color.blue} };
  SDL_SetColors(surface, colors, 0, 1);

  draw_text(text, point, allign, color.alpha);
  }

void Font::draw_text(const std::string& text, const Point& point, Allignment allign, int alpha)
  {
  /* Now, just draw the text. */
  if(alpha <= 0)
    return;  // no alpha

  Point p = point;

  if(allign == CENTER_ALLIGN)
    p.x -= (text.size()*size) / 2;
  else if(allign == RIGHT_ALLIGN)
    p.x -= text.size()*size;

  if(alpha != 255)
    SDL_SetAlpha(surface, SDL_SRCALPHA, alpha);

  for(unsigned int i = 0; i < text.size(); i++)
    {
    if(text[i] == '\n')
      {
      p.x = point.x;
      p.y += size+2;
      }
    else
      p.x += draw_char(text[i], p.x, p.y);
    }

  if(alpha != 255)
    SDL_SetAlpha(surface, SDL_SRCALPHA, 255);
  }

int Font::draw_char(char ch, int x, int y)
  {
  if(ch == ' ')
    return size/2;
  if(ch < '!' || ch > '~')
    ch = '-';  // out of range

  SDL_Rect src = { index_x_position(ch-32), index_y_position(ch-32), glyph_width[ch-32], size };
  SDL_Rect dst = { x, y, 0, 0 };

  SDL_BlitSurface(surface, &src, Screen::surface, &dst);

  return glyph_width[ch-32];
  }
