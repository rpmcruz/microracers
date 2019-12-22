/***************************************************************************
              screen.h  -  convinience layer to deal with the screen
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

#ifndef SCREEN_H
#define SCREEN_H

#include "point.h"
#include "color.h"

namespace Screen
  {
  int open(int w, int h, int bpp, int flags);
  void close();

  /* Toggle from window to fullscreen mode. */
  void toggle_fullscreen();
  bool is_fullscreen();

  void update();
  void update_part(Rect& rect);

  void draw_hline(int x, int y, int w, const Color& color);
  void draw_vline(int x, int y, int h, const Color& color);
  void draw_rect(const Rect& rect, const Color& color);
  void fill_rect(const Rect& rect, const Color& color);
  void clear_screen(const Color& color);
  void draw_horizontal_gradient(const Rect& rect, const Color& top, const Color& bottom);

  // more advanced stuff
  void set_clip_area(const Rect& rect);
  void unset_clip_area();

  extern int w, h;  // convinience variables to avoid accessing surface

  // fade effects
  void fadeout(int fade_time);

  extern SDL_Surface* surface;  // use this carefully
  };

#endif
