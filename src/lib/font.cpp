/***************************************************************************
                          font.cpp  -  handles a font
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

#include <SDL.h>
#include <iostream>

#include "color.h"
#include "screen.h"
#include "font.h"
#include "surface.h"

Font::Font(const std::string& filename, int size)
{
load_font(filename, size);
}

Font::~Font()
{
if(ttf_font)
  TTF_CloseFont(ttf_font);
}

void Font::load_font(const std::string& filename, int size_)
{
size = size_;
ttf_font = TTF_OpenFont(filename.c_str(), size);
if(ttf_font == NULL)
  std::cerr << "Error: Couldn't load font " << filename << std::endl
            << "Possible reason: " << TTF_GetError() << std::endl;
}

void Font::draw_text(const std::string& text, const Color& color, const Point& point, Allignment allign)
{
draw_text_surface(screen->get_screen(), text, color, point, allign);
}

void Font::draw_text_surface(SDL_Surface* surface, const std::string& text, const Color& color, const Point& point, Allignment allign)
{
if(color.alpha <= 0)
  return;

SDL_Color clr;
clr.r = color.red;
clr.g = color.green;
clr.b = color.blue;
SDL_Surface* s_text = TTF_RenderText_Solid(ttf_font, text.c_str(), clr);
if(color.alpha < 255)
  SDL_SetAlpha(s_text, SDL_SRCALPHA, color.alpha);

SDL_Rect r;
r.x = (int)point.x;
r.y = (int)point.y;

if(allign == CENTER_ALLIGN)
  r.x -= s_text->w/2;
else if(allign == RIGHT_ALLIGN)
  r.x -= s_text->w;

SDL_BlitSurface(s_text, NULL, surface, &r);

SDL_FreeSurface(s_text);
}
