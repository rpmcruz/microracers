/***************************************************************************
        primitives.cpp  -  my own expensive and bad-looking rotate&scale funcs
                             -------------------
    begin                : Wed Oct 26 2005
    copyright            : (C) 2005 by Ricardo Cruz
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

#include "primitives.h"
#include "sin_table.h"
#include "screen.h"

SDL_Surface* Primitives::rotate(SDL_Surface* input, int angle_deg)
  {
  bool use_colorkey = input->flags & SDL_SRCCOLORKEY;
  SDL_Surface* output;

  double sin_angle, cos_angle;
  angle_deg += 90;  // in computers graphics, degrees generally are used to start here

  sin_angle = sin(angle_deg);
  cos_angle = cos(angle_deg);

  int sw, sh, rw, rh, dx, dy;

  rw = input->w / 2;
  rh = input->h / 2;

  sw = (int)(input->w*ABS(sin_angle) + input->h*ABS(cos_angle))+2;
  sh = (int)(input->w*ABS(cos_angle) + input->h*ABS(sin_angle))+2;

  dx = sw - input->w;
  dy = sh - input->h;

  output = SDL_CreateRGBSurface(SDL_SWSURFACE, sw, sh,
                                input->format->BitsPerPixel,
                                input->format->Rmask, input->format->Gmask,
                                input->format->Bmask, input->format->Amask);

  // Check is surface has colorkey enabled
  Uint32 colorkey = 0;
  if(use_colorkey)
    colorkey = input->format->colorkey;
  SDL_FillRect(output, NULL, colorkey);

  SDL_LockSurface(output);
  SDL_LockSurface(input);

  /* Instead of calculating the destination pixel, we'll calculate the source pixel
     for every distination pixel -- this way we'll avoid precision errors that *would*
     happen. */
  int x, y, xi, yi, cx, cy;
  Uint32 pixel;
  for(y = -ABS(dy); y < sh+ABS(dy); y++)
    for(x = -ABS(dx); x < sw+ABS(dx); x++)
      {
      cx = rw - x;
      cy = rh - y;
      xi = (int)(rw - ((cx * sin_angle) + (cy * cos_angle)));
      yi = (int)((cx * cos_angle) - (cy * sin_angle) + rh);

      if(xi < 0 || yi < 0 || xi >= input->w || yi >= input->h)
        continue;
      pixel = get_pixel(input, xi, yi);

      if(pixel == colorkey)
        continue;
      put_pixel(output, x+(dx/2), y+(dy/2), pixel);
      }

  SDL_UnlockSurface(output);
  SDL_UnlockSurface(input);

  if(use_colorkey)
    SDL_SetColorKey(output, SDL_SRCCOLORKEY|SDL_RLEACCEL, colorkey);

  return output;
  }

SDL_Surface* Primitives::scale(SDL_Surface* input, float x_ratio, float y_ratio, bool anti_aliasing)
  {
  bool use_colorkey = input->flags & SDL_SRCCOLORKEY;
  SDL_Surface* output;

  int sw, sh;
  sw = (int)(input->w * x_ratio);
  sh = (int)(input->h * y_ratio);

  output = SDL_CreateRGBSurface(SDL_SWSURFACE, sw, sh,
                                input->format->BitsPerPixel, 
                                input->format->Rmask, input->format->Gmask,
                                input->format->Bmask, input->format->Amask);

  // Check is surface has colorkey enabled
  Uint32 colorkey = 0;
  if(use_colorkey)
    colorkey = input->format->colorkey;
  SDL_FillRect(output, NULL, colorkey);

  SDL_LockSurface(output);
  SDL_LockSurface(input);

  int x,y, x_,y_;
  Uint32 pixel;
  for(y = 0; y < sh; y++)
    for(x = 0; x < sw; x++)
      {
      x_ = (int)(x/x_ratio);
      y_ = (int)(y/y_ratio);

      /* It works, but results are not very satisfying. To be improved. */
      if(anti_aliasing)
        {
        SDL_PixelFormat* fmt = Screen::surface->format;
        Uint32 color[3], temp;
        color[0] = color[1] = color[2] = 0;
        for(int i = 0; i < 4; i++)
          {
          if(i == 0) pixel = get_pixel(input, x_-1, y_-1);
          else if(i == 1) pixel = get_pixel(input, x_+1, y_-1);
          else if(i == 2) pixel = get_pixel(input, x_-1, y_+1);
          else if(i == 3) pixel = get_pixel(input, x_+1, y_+1);
          temp = pixel & fmt->Rmask; temp = temp >> fmt->Rshift;
          temp = temp << fmt->Rloss; color[0] += temp;
          temp = pixel & fmt->Gmask; temp = temp >> fmt->Gshift;
          temp = temp << fmt->Gloss; color[1] += temp;
          temp = pixel & fmt->Bmask; temp = temp >> fmt->Bshift;
          temp = temp << fmt->Bloss; color[2] += temp;
          }
        color[0] /= 4; color[1] /= 4; color[2] /= 4;
        pixel = SDL_MapRGB(Screen::surface->format,
                           color[0], color[1], color[2]);
        }
      else
        pixel = get_pixel(input,x_,y_);

      put_pixel(output, x, y, pixel);
      }

  SDL_UnlockSurface(output);
  SDL_UnlockSurface(input);

  if(use_colorkey)
    SDL_SetColorKey(output, SDL_SRCCOLORKEY|SDL_RLEACCEL, colorkey);

  return output;
  }

Uint32 Primitives::get_pixel(SDL_Surface* surface, int x, int y)
  {
  // NOTE: The surface must be locked before calling this!
  
  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    return 0;
  
  /** This code to get a pixel form a position was taken from SDLdoc
      (The SDL Documentation Project): */
  // Returns the pixel value at (x, y)
  int bpp = surface->format->BytesPerPixel;
  
  // here p is the address to the pixel we want to retrieve
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
  
  switch(bpp)
    {
    case 1:
      return *p;
    case 2:
      return *(Uint16 *)p;
    case 3:
      #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        return p[0] << 16 | p[1] << 8 | p[2];
      #else
        return p[0] | p[1] << 8 | p[2] << 16;
      #endif
    case 4:
      return *(Uint32 *)p;
    default:
      return 0;   // shouldn't happen, but avoids warnings
    }
  }

void Primitives::put_pixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
  {
  if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    return;

  /** This code to get a pixel form a position was taken from SDLdoc
    (The SDL Documentation Project): */
  // Puts a pixel value at (x, y)

  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to set */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp)
    {
    case 1:
      *p = pixel;
      break;
    case 2:
      *(Uint16 *)p = pixel;
      break;
    case 3:
      #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        {
        p[0] = (pixel >> 16) & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = pixel & 0xff;
        }
      #else
        {
        p[0] = pixel & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = (pixel >> 16) & 0xff;
        }
      #endif
      break;
    case 4:
      *(Uint32 *)p = pixel;
      break;
    }
  }

void Primitives::draw_hline(SDL_Surface* surface, int x, int y, int w, const Color& color)
  {
  fill_rect(surface, x, y, w, 1, color);
  }

void Primitives::draw_vline(SDL_Surface* surface, int x, int y, int h, const Color& color)
  {
  fill_rect(surface, x, y, 1, h, color);
  }

void Primitives::draw_rect(SDL_Surface* surface, int x, int y, int w, int h, const Color& c)
  {
  draw_hline(surface, x, y,   w, c);
  draw_hline(surface, x, y+h, w, c);

  draw_vline(surface, x,   y, h, c);
  draw_vline(surface, x+w, y, h, c);
  }

void Primitives::fill_rect(SDL_Surface* surface, int x, int y, int w, int h, const Color& color)
  {
  if(color.alpha == 255)
    {
    SDL_Rect r = { x, y, w, h };
    SDL_FillRect(surface, &r, color.map_rgb(surface));
    }
  else
    {
    SDL_Surface* temp = SDL_CreateRGBSurface(surface->flags, w, h,
              surface->format->BitsPerPixel,
              surface->format->Rmask, surface->format->Gmask,
              surface->format->Bmask, surface->format->Amask);

    SDL_Rect src = { 0, 0, w, h };
    SDL_FillRect(temp, &src, color.map_rgb(surface));
    SDL_SetAlpha(temp, SDL_SRCALPHA, color.alpha);

    src.x = x; src.y = y;
    src.w = w; src.h = h;
    SDL_BlitSurface(temp, NULL, surface, &src);

    SDL_FreeSurface(temp);
    }
  }

void Primitives::draw_horizontal_gradient(SDL_Surface* surface, const Rect& rect, const Color& top, const Color& bot)
  {
  // calculates the color for each line, based in the generic equation for functions:
  // y = mx + b
  Color clr;
  for(int x = 0; x < rect.w; x++)
    {
    clr = Color((((top.red-bot.red) * x) / (0-rect.w)) + top.red,
                (((top.green-bot.green) * x) / (0-rect.w)) + top.green,
                (((top.blue-bot.blue) * x) / (0-rect.w)) + top.blue);
    fill_rect(surface, rect.x+x, rect.y, 1, rect.h, clr);
    }
  }


void Primitives::replace_color(SDL_Surface* surface, const Color& src_color, const Color& dst_color)
  {
  // don't forget to lock the surface, before this
  Uint32 src = src_color.map_rgb(surface);
  Uint32 dst = dst_color.map_rgb(surface);
  for (int x = 0; x < surface->w; x++)
    for (int y = 0; y < surface->h; y++)
      if (get_pixel(surface, x, y) == src)
        Primitives::put_pixel(surface, x, y, dst);
  }

void Primitives::paste_color(SDL_Surface* surface, const Color& color)
  {
  // don't forget to lock the surface, before this
  Uint32 clr = color.map_rgb(surface);
  for (int x = 0; x < surface->w; x++)
    for (int y = 0; y < surface->h; y++)
      if (get_pixel(surface, x, y) != surface->format->colorkey)
        Primitives::put_pixel(surface, x, y, clr);
  }

SDL_Surface* Primitives::optimize_surface(SDL_Surface* surface)
  {
  SDL_Surface* converted = SDL_ConvertSurface(surface, Screen::surface->format, SDL_SWSURFACE);
  if(converted != NULL)
    {
    SDL_FreeSurface(surface);
    return converted;
    }
  return surface;
  }
