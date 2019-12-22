/***************************************************************************
         rotate_scale.h  -  my own expensive and bad-looking rotate&scale funcs
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

#ifndef ROTATE_SCALE
#define ROTATE_SCALE

#include <SDL.h>

/* TODO: there are still some primitives code at surface.cpp -- might
   be a good idea to isolate it here too. */

namespace Primitives
  {
  SDL_Surface* rotate(SDL_Surface* input, int angle_deg);
  SDL_Surface* scale(SDL_Surface* input, float x_ratio, float y_ratio);

  Uint32 get_pixel(SDL_Surface* sdl_surface, int x, int y);
  void put_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
  };

#endif/*ROTATE_SCALE*/ 
