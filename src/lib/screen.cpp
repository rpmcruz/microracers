/***************************************************************************
                          ini_parser.h  -  to be included in all cpp files
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

#include <iostream>

#include "screen.h"

Screen* screen = 0;

Screen::Screen(int w, int h, int bpp, int flags)
{
set_video_mode(w, h, bpp, flags);
}

Screen::~Screen()
{
SDL_FreeSurface(sdl_screen);
}

int Screen::set_video_mode(int w_, int h_, int bpp, int flags)
{
sdl_screen = SDL_SetVideoMode(w_, h_, bpp, flags);
if(sdl_screen == NULL)
  {
  std::cerr << "Error: Couldn't set video mode " << w_ << "x" << h_ << "x" << bpp << std::endl <<
               "Possible reason: " << SDL_GetError() << std::endl;
  return -1;
  }

w = w_; h = h_;
return 0;
}

void Screen::toggle_fullscreen()
{
// unfortunately, this command doesn't work for non-X11 platforms:
//SDL_WM_ToggleFullScreen(sdl_screen);
Uint32 flags = sdl_screen->flags;
if(is_fullscreen())
  flags = ~flags & SDL_FULLSCREEN;
else
  flags |= SDL_FULLSCREEN;
int w_ = sdl_screen->w;
int h_ = sdl_screen->h;
Uint8 bpp = sdl_screen->format->BitsPerPixel;
SDL_FreeSurface(sdl_screen);
sdl_screen = SDL_SetVideoMode(w_, h_, bpp, flags);
}

bool Screen::is_fullscreen()
{
return sdl_screen->flags & SDL_FULLSCREEN;
}

void Screen::update()
{
SDL_Flip(sdl_screen);
}


void Screen::update_part(Rect& rect)
{
SDL_UpdateRect(sdl_screen, (int)rect.x, (int)rect.y, rect.h, rect.w);
}

void Screen::fill_rect(const Rect& rect, const Color& color)
{
if(color.alpha == 255)
  {
  SDL_Rect r;
  r.x = (int)rect.x;
  r.y = (int)rect.y;
  r.w = rect.w;
  r.h = rect.h;
  Color c = color;
  SDL_FillRect(sdl_screen, &r, c.map_rgb(sdl_screen));
  }
else
  {                                                                               
  SDL_Surface* temp = SDL_CreateRGBSurface(sdl_screen->flags, rect.w, rect.h,
             sdl_screen->format->BitsPerPixel,
             sdl_screen->format->Rmask, sdl_screen->format->Gmask,
             sdl_screen->format->Bmask, sdl_screen->format->Amask);

  SDL_Rect src;
  src.x = src.y = 0;
  src.w = rect.w;
  src.h = rect.h;
  Color c = color;
  SDL_FillRect(temp, &src, c.map_rgb(sdl_screen));
  SDL_SetAlpha(temp, SDL_SRCALPHA, color.alpha);

  src.x = (int)rect.x;
  src.y = (int)rect.y;
  src.w = rect.w;
  src.h = rect.h;
  SDL_BlitSurface(temp, NULL, sdl_screen, &src);

  SDL_FreeSurface(temp);
  }
}

SDL_Surface* Screen::get_screen()
{
return sdl_screen;
}

void Screen::set_clip_area(const Rect& rect)
{
SDL_Rect r;
r.x = (int)rect.x; r.y = (int)rect.y;
r.w = rect.w; r.h = rect.h;
SDL_SetClipRect(sdl_screen, &r);
}

void Screen::unset_clip_area()
{
SDL_SetClipRect(sdl_screen, NULL);
}

#define LOOP_DELAY 20.0

void Screen::fadeout(int fade_time)
{
float alpha_inc  = 256 / (fade_time / LOOP_DELAY);
float alpha = 255;

SDL_Surface *screen_copy, *black_surface;

screen_copy = SDL_CreateRGBSurface (sdl_screen->flags|SDL_SRCALPHA,
  sdl_screen->w, sdl_screen->h, sdl_screen->format->BitsPerPixel,
  sdl_screen->format->Rmask, sdl_screen->format->Gmask, sdl_screen->format->Bmask,
  sdl_screen->format->Amask);
if(screen_copy == NULL)
  {
  std::cerr << "Error: could not create screen copy for fading.\n"
    "SDL error: " << SDL_GetError() << std::endl;
  return;
  }
SDL_BlitSurface(sdl_screen, NULL, screen_copy, NULL) ;

black_surface = SDL_CreateRGBSurface (sdl_screen->flags|SDL_SRCALPHA,
  sdl_screen->w, sdl_screen->h, sdl_screen->format->BitsPerPixel,
  sdl_screen->format->Rmask, sdl_screen->format->Gmask, sdl_screen->format->Bmask,
  sdl_screen->format->Amask);
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

  SDL_BlitSurface(screen_copy, NULL, sdl_screen, NULL);
  SDL_BlitSurface(black_surface, NULL, sdl_screen, NULL);

  SDL_Flip(sdl_screen);
  SDL_Delay(int(LOOP_DELAY));

  alpha -= alpha_inc;
  }

fill_rect(Rect(0,0,w,h), Color(0,0,0,255));
update();
}
