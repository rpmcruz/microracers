/***************************************************************************
                          gameloop.h  -  responsible for the game itself
                             -------------------
    begin                : Sat Aug 31 2002
    copyright            : (C) 2002 by Ricardo Cruz
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

#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <SDL.h>
#include <vector>

#include "lib/point.h"
#include "lib/font.h"

class Vehicle;
class Track;
class Surface;
class Camera;

enum Axis { X_AXIS, Y_AXIS };

class Race
{
public:
  Race();
  ~Race();

  void setup_track(const std::string& path);
  void add_vehicle(const std::string& path, bool human_player = false);

  /* max_lap is the lap which wins, if it is 0, game is forever */
  // don't forget to setup a track and add vehicles before calling this
  void start(int max_lap);

  // if you want to start a new race with other vehicles and track
  // call this!
  void reset();

  // draw text with a shadow effect
  void draw_shadow_text(const std::string& text, const Point& point,
                        Allignment allignment = LEFT_ALLIGN);

  /* This could be used by Vehicles or Track to get info about each
     other by making a call like: Track* t = Race::current->track; */
  static Race* current;
  void activate()
    { current = this; }

  /* Checks collision of given vehicle against other vehicles. If true,
     it will call handle_collision() from the given vehicle. */
  void check_collision(Vehicle* vehicle, Axis axis);

  typedef std::vector <Vehicle*> Vehicles;
  Vehicles vehicles;
  int total_human_players;

  typedef std::vector <Camera*> Cameras;
  Cameras cameras;

  Track* track;

private:
  // clear all events until now
  void clear_events();
  void check_events();
  SDL_Event event;

  // check if there is a collision on those boxes
  bool test_collision(const Rect& rect1, const Rect& rect2);

  // AI for computer cars
  void AI_movement();

  // draw player lap number
  void draw_info_text(int lapnumber, int maxlaps, int fps = -1);
  Font* lap_font;
  Surface *panel, *traffic_lights;

  bool done, game_paused;

  // debug variables:
  int debug_delay_fps;
};

#endif
