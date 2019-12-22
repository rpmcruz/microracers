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

  void set_colorkey(const Color& color);
  bool has_colorkey();

  void draw(Point point);
  void draw_part(Point screen_point, Rect image_area);
  void draw_transparent(Point point); // ordinary draw() with some transparency

  /* Convinience drawing functions */
  // emulate a Sprite behavior. Will call draw_part().
  void draw_frame(Point point, int frame, int total_frames);

  void set_offset(const Point& new_offset)
    { offset = new_offset; }
  void add_offset(const Point& new_offset)
    { offset += new_offset; }

  /* Graphical effects. */
  void rotate(int angle);
  void scale(float x_scale, float y_scale);
  void stretch(int w, int h);
  void set_transparent(int alpha);
  void fill_rect(const Rect& rect, const Color& color);

  // Special pixel operations
  void lock_surface();
  void unlock_surface();
  /* DON'T YOU FORGET TO LOCK SURFACE BEFORE CALLING THESE: */
  Uint32 get_pixel(int x, int y);
  void replace_color(const Color& src_color, const Color& dst_color);
  void paste_color(const Color& color);
  bool is_pixel_colorkey(int x, int y);

  SDL_Surface* get_sdl_surface()  // avoid this call
    { return sdl_surface; }

  int w, h;

private:
  void load_image(const std::string& filename);

  SDL_Surface* sdl_surface;

  // used to draw the image a bit far way to make it looking well placed.
  // one of the uses is for rotated images
  Point offset;
};

#endif
