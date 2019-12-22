/***************************************************************************
                          surface.cpp  -  to be included in all cpp files
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

#include <SDL_image.h>
#include <iostream>
#include <math.h>

#include "surface.h"
#include "primitives.h"
#include "screen.h"

Surface::Surface(const std::string& filename)
{
load_image(filename);
}

Surface::Surface(const std::string& filename, const Color& color)
{
load_image(filename);
set_colorkey(color);
}

Surface::Surface(SDL_Surface* sur)
{
sdl_surface = SDL_CreateRGBSurface(sur->flags,
             sur->w, sur->h, sur->format->BitsPerPixel,
             sur->format->Rmask, sur->format->Gmask,
             sur->format->Bmask, sur->format->Amask);
if(!sdl_surface)
  std::cerr << "Warning: could not create SDL_surface to copy Surface.\n";

SDL_FillRect(sdl_surface, NULL, sur->format->colorkey);
SDL_BlitSurface(sur, NULL, sdl_surface, NULL);
SDL_SetColorKey(sdl_surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, sur->format->colorkey);

w = sdl_surface->w;
h = sdl_surface->h;
}

Surface::Surface(SDL_Surface* sur, int angle)
{
sdl_surface = SDL_CreateRGBSurface(sur->flags,
             sur->w, sur->h, sur->format->BitsPerPixel,
             sur->format->Rmask, sur->format->Gmask,
             sur->format->Bmask, sur->format->Amask);

if(!sdl_surface)
  std::cerr << "Warning: could not create SDL_Surface to copy Surface.\n";

SDL_FillRect(sdl_surface, NULL, sur->format->colorkey);
SDL_BlitSurface(sur, NULL, sdl_surface, NULL);
SDL_SetColorKey(sdl_surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, sur->format->colorkey);

rotate(angle);

// set offset, so that user doesn't notice change in size
offset.x = (sur->w-w) / 2;
offset.y = (sur->h-h) / 2;
}

Surface::~Surface()
{
SDL_FreeSurface(sdl_surface);
}

void Surface::load_image(const std::string& filename)
{
sdl_surface = IMG_Load(filename.c_str());
if(sdl_surface == NULL)
  {
  std::cerr << "Error: Couldn't load image " << filename << std::endl
            << "Possible reason: " << IMG_GetError() << std::endl;
  return;
  }

sdl_surface = Primitives::optimize_surface(sdl_surface);

w = sdl_surface->w;
h = sdl_surface->h;
}

void Surface::draw(Point point)
{
if(point.x + w < 0 || point.x > Screen::w ||
   point.y + h < 0 || point.y > Screen::h)
  return;   // drawing out of screen range

SDL_Rect r = point.sdl_rect();
r.x += (int)offset.x;
r.y += (int)offset.y;

SDL_BlitSurface(sdl_surface, NULL, Screen::surface, &r);
}

void Surface::draw_part(Point point, Rect img_area)
{
if(img_area.x > w || img_area.x + img_area.w > w ||
   img_area.y > h || img_area.y + img_area.h > h)
  std::cerr << "Warning: part of image to draw bigger than the image itself.\n";

if(point.x + img_area.w < 0 || point.x > Screen::w ||
   point.y + img_area.h < 0 || point.y > Screen::h)
  return;   // drawing out of screen range

SDL_Rect src, dst;
src = img_area.sdl_rect();
dst = point.sdl_rect();
dst.x += (int)offset.x;
dst.y += (int)offset.y;
SDL_BlitSurface(sdl_surface, &src, Screen::surface, &dst);
}

void Surface::draw_transparent(Point point)
  {
  if(point.x + w < 0 || point.x > Screen::w ||
     point.y + h < 0 || point.y > Screen::h)
    return;   // drawing out of screen range

  Surface t(sdl_surface);
  t.set_transparent(128);
  t.draw(point);
  }

void Surface::draw_frame(Point point, int frame, int total_frames)
{
draw_part(point, Rect(frame * (w / total_frames), 0, w/total_frames, h));
}

void Surface::set_colorkey(const Color& color)
{
if(SDL_SetColorKey(sdl_surface, SDL_SRCCOLORKEY | SDL_RLEACCEL,
                color.map_rgb(sdl_surface)) == -1)
  std::cerr << "Error: could not set colorkey.\n"
               "Possible reason: " << SDL_GetError() << std::endl;
}

bool Surface::has_colorkey()
  {
  return sdl_surface->flags & SDL_SRCCOLORKEY;
  }

void Surface::rotate(int angle)
{
if(angle == 0)
  return;

SDL_Surface* old_surface = sdl_surface;
sdl_surface = Primitives::rotate(old_surface, angle);
SDL_FreeSurface(old_surface);

w = sdl_surface->w;
h = sdl_surface->h;
}

void Surface::stretch(int w, int h)
{
if(this->w == w && this->h == h)  // sanity check
  return;
scale((float)w / this->w, (float)h / this->h);
}

void Surface::scale(float x_scale, float y_scale)
{
if(x_scale == 1 && y_scale == 1)  // sanity check
  return;

SDL_Surface* old_surface = sdl_surface;
sdl_surface = Primitives::scale(old_surface, x_scale, y_scale);
SDL_FreeSurface(old_surface);

w = sdl_surface->w;
h = sdl_surface->h;
}

void Surface::set_transparent(int alpha)
  {
  SDL_SetAlpha(sdl_surface, SDL_SRCALPHA | SDL_RLEACCEL, alpha);
  }

void Surface::fill_rect(const Rect& rect, const Color& color)
  {
  Primitives::fill_rect(sdl_surface, rect.x, rect.y, rect.w, rect.h, color);
  }

void Surface::lock_surface()
{
SDL_LockSurface(sdl_surface);
}

void Surface::unlock_surface()
{
SDL_UnlockSurface(sdl_surface);
}

Uint32 Surface::get_pixel(int x, int y)
  {
  return Primitives::get_pixel(sdl_surface, x, y);
  }

void Surface::replace_color(const Color& src_color, const Color& dst_color)
  { Primitives::replace_color(sdl_surface, src_color, dst_color); }

void Surface::paste_color(const Color& color)
  { Primitives::paste_color(sdl_surface, color); }

bool Surface::is_pixel_colorkey(int x, int y)
  {
  return get_pixel(x, y) == sdl_surface->format->colorkey;
  }
