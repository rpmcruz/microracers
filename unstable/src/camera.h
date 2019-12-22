/***************************************************************************
                          camera.h  -  stores scrolling info
                             -------------------
    begin                : Mon Nov 01 2004
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

#ifndef CAMERA_H
#define CAMERA_H

#include "lib/point.h"

class Vehicle;

/* This class follows a player and is reponsible to keep track of its
   screen scrolling, as well, as drawing its screen part to the screen.
   */

class Camera
{
public:
  Camera(Vehicle* follow);
  ~Camera();

  // this call must be made to initilizate it!
  void set_screen_area(const Rect& size);

  void update_scrolling();

  // dont_draw_humans is just a flag used for a work around
  // to make human players blinking at startup
  void draw(bool dont_draw_humans = false);

  // returns the player's human number
  int get_player_nb();
  const Point& get_scrolling()
    { return scroll; }
  Point get_screen_size()
    { return Point(screen_area.w, screen_area.h); }

private:
  Point scroll, old_scroll;
  Rect screen_area;
  Vehicle* follow;
};

#endif
