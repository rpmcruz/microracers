/***************************************************************************
            screen.cpp  -  convinience layer to deal with the screen
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

#include <iostream>

#include "screen.h"
#include "primitives.h"

int Screen::w, Screen::h;
SDL_Surface* Screen::surface = NULL;

int Screen::open(int w, int h, int bpp, int flags)
  {
  surface = SDL_SetVideoMode(w, h, bpp, flags);
  if(surface == NULL)
    {
    std::cerr << "Error: Couldn't set video mode " << w << "x" << h
              << "x" << bpp << std::endl
              << "Possible reason: " << SDL_GetError() << std::endl;
    return -1;
    }

  Screen::w = w;
  Screen::h = h;
  surface = surface;

  return 0;
  }

void Screen::close()
  {
  SDL_FreeSurface(surface);
  }

void Screen::toggle_fullscreen()
  {
  // unfortunately, this command doesn't work for non-X11 platforms:
  //SDL_WM_ToggleFullScreen(surface);
  Uint32 flags = surface->flags;
  if(is_fullscreen())
    flags = ~flags & SDL_FULLSCREEN;
  else
    flags |= SDL_FULLSCREEN;
  int w_ = surface->w;
  int h_ = surface->h;
  Uint8 bpp = surface->format->BitsPerPixel;
  SDL_FreeSurface(surface);
  surface = SDL_SetVideoMode(w_, h_, bpp, flags);
  }

bool Screen::is_fullscreen()
  {
  return surface->flags & SDL_FULLSCREEN;
  }

void Screen::update()
  {
  SDL_Flip(surface);
  }
void Screen::update_part(Rect& rect)
  {
  SDL_UpdateRect(surface, (int)rect.x, (int)rect.y, rect.h, rect.w);
  }

void Screen::draw_rect(const Rect& r, const Color& c)
  {
  Primitives::draw_rect(surface, r.x, r.y, r.w, r.h, c);
  }

void Screen::fill_rect(const Rect& r, const Color& c)
  {
  Primitives::fill_rect(surface, r.x, r.y, r.w, r.h, c);
  }

void Screen::clear_screen(const Color& color)
  {
  SDL_FillRect(surface, NULL, color.map_rgb(surface));
  }

void Screen::draw_horizontal_gradient(const Rect& rect, const Color& top, const Color& bot)
  {
  // calculates the color for each line, based in the generic equation for functions:
  // y = mx + b
  Color clr;
  for(int x = 0; x < rect.w; x++)
    {
    clr = Color((((top.red-bot.red) * x) / (0-rect.w)) + top.red,
                (((top.green-bot.green) * x) / (0-rect.w)) + top.green,
                (((top.blue-bot.blue) * x) / (0-rect.w)) + top.blue);
    fill_rect(Rect(rect.x+x, rect.y, 1, rect.h), clr);
    }
  }

void Screen::set_clip_area(const Rect& rect)
  {
  SDL_Rect r;
  r.x = (int)rect.x; r.y = (int)rect.y;
  r.w = rect.w; r.h = rect.h;
  SDL_SetClipRect(surface, &r);
  }

void Screen::unset_clip_area()
  {
  SDL_SetClipRect(surface, NULL);
  }

#define LOOP_DELAY 20.0

void Screen::fadeout(int fade_time)
  {
  float alpha_inc  = 256 / (fade_time / LOOP_DELAY);
  float alpha = 255;

  SDL_Surface *screen_copy, *black_surface;

  screen_copy = SDL_CreateRGBSurface (surface->flags|SDL_SRCALPHA,
    surface->w, surface->h, surface->format->BitsPerPixel,
    surface->format->Rmask, surface->format->Gmask, surface->format->Bmask,
    surface->format->Amask);
  if(screen_copy == NULL)
    {
    std::cerr << "Error: could not create screen copy for fading.\n"
      "SDL error: " << SDL_GetError() << std::endl;
    return;
    }
  SDL_BlitSurface(surface, NULL, screen_copy, NULL) ;

  black_surface = SDL_CreateRGBSurface (surface->flags|SDL_SRCALPHA,
    surface->w, surface->h, surface->format->BitsPerPixel,
    surface->format->Rmask, surface->format->Gmask, surface->format->Bmask,
    surface->format->Amask);
  if(black_surface == NULL)
    {
    std::cerr << "Error: could not create black surface for fading.\n"
      "SDL error: " << SDL_GetError() << std::endl;
    SDL_FreeSurface(screen_copy);
    return;
    }
  SDL_FillRect(black_surface, NULL, 0);

  while(alpha > 0)
    {
    SDL_SetAlpha(black_surface, SDL_SRCALPHA, (Uint8)(255-alpha));

    SDL_BlitSurface(screen_copy, NULL, surface, NULL);
    SDL_BlitSurface(black_surface, NULL, surface, NULL);

    SDL_Flip(surface);
    SDL_Delay(int(LOOP_DELAY));

    alpha -= alpha_inc;
    }

  clear_screen(Color::black);
  update();
  }
