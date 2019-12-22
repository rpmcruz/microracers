/***************************************************************************
        bitmask.cpp  -  a mask of bits to be used for collision detection
                             -------------------
    begin                : Fri Nov 18 2005
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

#include "bitmask.h"

#include "surface.h"

Bitmask::Bitmask(Surface* surface)
  {
  array_size = surface->w*surface->h;
  array_w = surface->w; array_h = surface->h;

  array = new bool [array_size];
  surface->lock_surface();
  for(int y = 0; y < surface->h; y++)
    for(int x = 0; x < surface->w; x++)
      array[get_index(x,y)] = !surface->is_pixel_colorkey(x,y);
  surface->unlock_surface();

#if 0
std::cerr << "-----printing bitmask-----\n";
for(int y = 0; y < surface->h; y++)
  {
  for(int x = 0; x < surface->w; x++)
    std::cerr << ((array[get_index(x,y)]) ? "1" : "0");
  std::cerr << "\n";
  }
std::cerr << "----------/--/------------\n";
#endif
  }

Bitmask::~Bitmask()
  {
  delete [] array;
  }

int Bitmask::get_index(int x, int y)
  {
  int index = x + (y*array_w);
  if(index < 0)
    return -1;
  else if(index >= array_size)
    return -1;
  return index;
  }

bool Bitmask::has_bit(int x, int y)
  {
  if(x < 0)
    x = 0;
  else if(y < 0)
    y = 0;
  else if(x >= array_w)
    x = array_w-1;
  else if(y >= array_h)
    y = array_h-1;
//std::cerr << "has_bit(" << x << ", " << y << "): " << (array[get_index(x,y)] ? "1" : "0") << std::endl;
  return array[get_index(x,y)];
  }
