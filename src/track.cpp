/***************************************************************************
                          track.cpp  -  responsable for the map
                             -------------------
    begin                : Sun Oct 31 2004
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

#include <iostream>
#include <stdlib.h>

#define TERRAIN_TYPE_SURFACE_RATIO 1

#include "track.h"
#include "lib/ini_parser.h"
#include "lib/surface.h"
#include "lib/color.h"
#include "lib/file_access.h"
#include "globals.h"

const Color terrain_types_color[TOTAL_TT] = {
  Color(255, 255, 255),  // NORMAL_TT
  Color(255, 200, 255),  // SLOW1_TT
  Color(255, 140, 255),  // SLOW2_TT
  Color(255,   0, 255),  // SLOW3_TT
  Color(200, 255, 255),  // FAST1_TT
  Color(140, 255, 255),  // FAST2_TT
  Color(  0, 255, 255),  // FAST3_TT
  Color(255, 255, 200),  // JUMP1_TT
  Color(255, 255, 140),  // JUMP2_TT
  Color(255, 255,   0),  // JUMP3_TT
  Color(140, 140, 140),  // BOUNCE_TT
  Color(255,   0,   0)   // DEAD_TT
  };

Track::Track(const std::string& path)
  : scale(1,1)
{
INIParser parser(path + "track.conf");

track_surface = new Surface(path + "track.png");
size.x = track_surface->w;
size.y = track_surface->h;

track_terrain_surface = new Surface(path + "track-terraintypes.png");

checkpoint_range = parser.get_int("[CHECKPNTRECT]", "range=");
checkpoint_miss_range = parser.get_int("[CHECKPNTRECT]", "miss_range=");

int max_points = parser.get_int("[MAX_POINTS]", "pointsnb=");
total_checkpoints = max_points;

for (int group = 1; group <= max_points; group++)
	{
	char str_tmp[24];
	sprintf(str_tmp, "[%d_POINT]", group);

  Point point;
	point.x = parser.get_int(str_tmp, "x=");
	point.y = parser.get_int(str_tmp, "y=");

  checkpoints.push_back(point);
	checkpoints_angle.push_back(parser.get_int(str_tmp, "angle=") - 90);
	}

for (int i = 1; ; i++)
  {
  char str_tmp[24];
  sprintf(str_tmp, "[%d_CAR]", i);
  if(!parser.group_exist(str_tmp))
    {
    if(i == 1)
      std::cerr << "Error: Not even one vehicle position was defined on track.\n"
                   "There will be a crash.\n";
    break;
    }
  Point pos;
  pos.x = parser.get_int(str_tmp, "x=");
  pos.y = parser.get_int(str_tmp, "y=");
  players_pos.push_back(pos);
  }

players_angle = parser.get_int("[CARS]", "angle=") - 90;

// TODO: should friction be added to track?

/* it is only used to see a color from a pixel,
   so can be locked for ever */
track_terrain_surface->lock_surface();
}

Track::~Track()
{
delete track_surface;
delete track_terrain_surface;
}

TerrainType Track::get_terrain_type(const Point& point)
{
if (point.x < 0 || point.y < 0 ||
    point.x >= size.x || point.y >= size.y)
{
	return (TerrainType) SLOW3_TT;
}

Uint32 color = track_terrain_surface->get_pixel(
  (int)point.x / TERRAIN_TYPE_SURFACE_RATIO,
  (int)point.y / TERRAIN_TYPE_SURFACE_RATIO);

for(int i = 0; i < TOTAL_TT; i++)
  {
  if(color ==
    terrain_types_color[i].map_rgb(track_terrain_surface->get_sdl_surface()))
    return (TerrainType) i;
  }

return (TerrainType) NORMAL_TT;	// if nothing was found
}

void Track::set_scale(float x_scale, float y_scale)
{
scale = Point(x_scale, y_scale);
if(scale == Point(1.0,1.0))
  return;
track_surface->stretch(x_scale, y_scale);
}

void Track::draw(const Point& scroll, const Rect& screen_area)
{
if (scroll.x > size.x*scale.x + screen_area.w)
  std::cerr << "Warning: X scrolling is bigger than track.\n";
if (scroll.y > size.y*scale.y + screen_area.h)
  std::cerr << "Warning: Y scrolling is bigger than track.\n";

track_surface->draw_part(Point(screen_area.x,screen_area.y), Rect(scroll.x, scroll.y, screen_area.w, screen_area.h));
}

#if 0
/* This code was used before to just draw a part of screen for efficiency.
   Though, this can't be hardly done and was already causing too many bugs
   because we have to make sure that for every little thing was drawn a
   part of track above. */
void Track::draw_part(const Rect& rect, const Point& scroll, const Rect& screen_area)
{
if(rect.x > scroll.x + screen_area.x + screen_area.w ||
   rect.y > scroll.y + screen_area.y + screen_area.h ||
   rect.x + rect.w < scroll.x + screen_area.x ||
   rect.y + rect.h < scroll.y + screen_area.y)
  return;

track_surface->draw_part(Point(rect.x-scroll.x, rect.y-scroll.y), rect);
}
#endif
