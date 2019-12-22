/***************************************************************************
                          surface.h  -  to be included in all cpp files
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

#ifndef SURFACE_H
#define SURFACE_H

#include <SDL.h>
#include <string>

#include "point.h"
#include "color.h"

class Surface
{
public:
  Surface(const std::string& filename);
  Surface(const std::string& filename, const Color& color);
  Surface(SDL_Surface* sur);  // sur will not be edited (most likely...)
  Surface(SDL_Surface* sur, int angle);  // also rotate it
  ~Surface();

  void load_image(const std::string& filename);

  void draw(Point point);
  void draw_part(Point screen_point, Rect image_area);

  /* Convinience drawing functions */
  // emulate a Sprite behavior. Will call draw_part().
  void draw_frame(Point point, int frame, int total_frames);

  void set_offset(const Point& new_offset)
    { offset = new_offset; }
  void add_offset(const Point& new_offset)
    { offset += new_offset; }

  void set_colorkey(Color color);

  /* Graphical effects. */
  void rotate(int angle);
  void stretch(float x_scale, float y_scale);
  void set_transparent(int alpha);

  // Special pixel operations
  void lock_surface();
  void unlock_surface();
  Uint32 get_pixel(int x, int y);
  void replace_color(Color src_color, Color dst_color);
  void paste_color(Color color);

  SDL_Surface* get_sdl_surface()  // avoid this call
    { return sdl_surface; }

  int w, h;

private:
  SDL_Surface* sdl_surface;

  // used to draw the image a bit far way to make it looking well placed.
  // one of the uses is for rotated images
  Point offset;
};

#endif
