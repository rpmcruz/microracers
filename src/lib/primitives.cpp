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

#include <math.h>

#define ABS(x) (x >= 0 ? x : -x)

SDL_Surface* Primitives::rotate(SDL_Surface* input, int angle_deg)
  {
  bool use_colorkey = input->flags & SDL_SRCCOLORKEY;
  SDL_Surface* output;

  double sin_angle, cos_angle;
  angle_deg += 90;  // in computers graphics, degrees generally are used to start here
  /* The reason why we do this is not to optmize, but because
     sin() and cos() may not give precise numbers. */
  if(angle_deg == 0 || angle_deg == 360)
    {
    cos_angle = 1;
    sin_angle = 0;
    }
  else if(angle_deg == 90)
    {
    cos_angle = 0;
    sin_angle = 1;
    }
  else if(angle_deg == 180)
    {
    cos_angle = -1;
    sin_angle = 0;
    }
  else if(angle_deg == 270)
    {
    cos_angle = 0;
    sin_angle = -1;
    }
  else
    {
    double angle_rad = (angle_deg * M_PI) / 180.0;
    sin_angle = sin(angle_rad);
    cos_angle = cos(angle_rad);
    }

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

      if(use_colorkey && pixel == colorkey)
        continue;
      put_pixel(output, x+(dx/2), y+(dy/2), pixel);
      }

  SDL_UnlockSurface(output);
  SDL_UnlockSurface(input);

  if(use_colorkey)
    SDL_SetColorKey(output, SDL_SRCCOLORKEY|SDL_RLEACCEL, colorkey);

  return output;
  }

SDL_Surface* Primitives::scale(SDL_Surface* input, float x_ratio, float y_ratio)
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

  int x,y;
  Uint32 pixel;
  for(y = 0; y < sh; y++)
    for(x = 0; x < sw; x++)
      {
      pixel = get_pixel(input, (int)(x/x_ratio), (int)(y/y_ratio));
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
