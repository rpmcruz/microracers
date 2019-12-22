/***************************************************************************
         bitmask.h  -  a mask of bits to be used for collision detection
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

#ifndef BITMASK_H
#define BITMASK_H

class Surface;

class Bitmask
  {
  public:
    Bitmask(Surface* surface);
    ~Bitmask();

    bool has_bit(int x, int y);

  private:
    int get_index(int x, int y);

    bool* array;
    int array_size, array_w, array_h;
  };

#endif /*BITMASK_H*/
