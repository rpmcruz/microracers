/***************************************************************************
                          surface.cpp  -  to be included in all cpp files
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
SDL_Surface* converted = SDL_ConvertSurface(sdl_surface, screen->get_screen()->format,
                                            SDL_SWSURFACE);
if(converted == NULL)
  {
  std::cerr << "Warning: Couldn't convert surface of " << filename << std::endl
            << "Possible reason: " << SDL_GetError() << std::endl;
  return;
  }
else
  {
  SDL_FreeSurface(sdl_surface);
  sdl_surface = converted;
  }

w = sdl_surface->w;
h = sdl_surface->h;
}

void Surface::draw(Point point)
{
if(point.x + w < 0 || point.x > screen->w ||
   point.y + h < 0 || point.y > screen->h)
  return;   // drawing out of screen range

SDL_Rect r = point.sdl_rect();
r.x += (int)offset.x;
r.y += (int)offset.y;

SDL_BlitSurface(sdl_surface, NULL, screen->get_screen(), &r);
}

void Surface::draw_part(Point point, Rect img_area)
{
if(img_area.x > w || img_area.x + img_area.w > w ||
   img_area.y > h || img_area.y + img_area.h > h)
  std::cerr << "Warning: part of image to draw bigger than the image itself.\n";

if(point.x + img_area.w < 0 || point.x > screen->w ||
   point.y + img_area.h < 0 || point.y > screen->h)
  return;   // drawing out of screen range

SDL_Rect src, dst;
src = img_area.sdl_rect();
dst = point.sdl_rect();
dst.x += (int)offset.x;
dst.y += (int)offset.y;
SDL_BlitSurface(sdl_surface, &src, screen->get_screen(), &dst);
}

void Surface::draw_frame(Point point, int frame, int total_frames)
{
draw_part(point, Rect(frame * (w / total_frames), 0, w/total_frames, h));
}

void Surface::set_colorkey(Color color)
{
if(SDL_SetColorKey(sdl_surface, SDL_SRCCOLORKEY | SDL_RLEACCEL,
                color.map_rgb(sdl_surface)) == -1)
  std::cerr << "Error: could not set colorkey.\n"
               "Possible reason: " << SDL_GetError() << std::endl;
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

void Surface::stretch(float x_scale, float y_scale)
{
SDL_Surface* old_surface = sdl_surface;
sdl_surface = Primitives::scale(old_surface, x_scale, y_scale);
SDL_FreeSurface(old_surface);

w = sdl_surface->w;
h = sdl_surface->h;
}

void Surface::set_transparent(int alpha)
{
SDL_SetAlpha(sdl_surface, SDL_SRCALPHA | SDL_RLEACCEL, alpha);
//SDL_SetAlpha(sdl_surface, 0, 128);
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

void Surface::replace_color(Color src_color, Color dst_color)
{
Uint32 src = src_color.map_rgb(sdl_surface);
Uint32 dst = dst_color.map_rgb(sdl_surface);

// don't forget to lock the surface, before this
for (int x = 0; x < w; x++)
	for (int y = 0; y < h; y++)
		{
		if (get_pixel(x, y) == src)
			{
			SDL_Rect r;
			r.x = x;
			r.y = y;
			r.w = 1;
			r.h = 1;
			SDL_FillRect(sdl_surface, &r, dst);
			}
		}
}

void Surface::paste_color(Color color)
{
Uint32 clr = color.map_rgb(sdl_surface);

// don't forget to lock the surface, before this
for (int x = 0; x < w; x++)
	for (int y = 0; y < h; y++)
		{
		if (get_pixel(x, y) != sdl_surface->format->colorkey)
			{
			SDL_Rect r;
			r.x = x;
			r.y = y;
			r.w = 1;
			r.h = 1;
			SDL_FillRect(sdl_surface, &r, clr);
			}
		}
}
