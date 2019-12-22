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

#ifndef SCREEN_H
#define SCREEN_H

#include "point.h"
#include "color.h"

class Screen
{
public:
  Screen(int w, int h, int bpp, int flags);
  ~Screen();

  int set_video_mode(int w_, int h_, int bpp, int flags);
  /* Toggle from window to fullscreen mode. */
  void toggle_fullscreen();
  bool is_fullscreen();

  void update();
  void update_part(Rect& rect);

  void fill_rect(const Rect& rect, const Color& color);

  // more advanced stuff
  void set_clip_area(const Rect& rect);
  void unset_clip_area();

  int w, h;

  SDL_Surface* get_screen();  // avoid this call

  // fade effects
  void fadeout(int fade_time);

private:
  SDL_Surface* sdl_screen;
};

extern Screen* screen;

#endif
