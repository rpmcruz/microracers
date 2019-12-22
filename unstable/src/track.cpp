/***************************************************************************
                          track.cpp  -  responsable for the map
                             -------------------
    begin                : Sun Oct 31 2004
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

#include <iostream>
#include <stdlib.h>

#include "track.h"
#include "lib/ini_parser.h"
#include "lib/surface.h"
#include "lib/color.h"
#include "lib/file_access.h"
#include "objects.h"
#include "globals.h"

#define CHECKPOINT_RANGE      300
#define CHECKPOINT_MISS_RANGE 900

Track::Track(const std::string& filename)
  : checkpoint_range(CHECKPOINT_RANGE), checkpoint_miss_range(CHECKPOINT_MISS_RANGE),
    players_angle(0)
#if 0
  : scale(1,1)
#endif
  {
  /* Load objects. */
  theme = NULL;

  std::string file;
  if(file_exists(homedir + "tracks/" + filename))
    file = homedir + "tracks/" + filename;
  else
    file = datadir + "tracks/" + filename;

  // load track here:
  INIIterator i;
  Object* object;

  for(i.open(file); !i.eof(); i.next())
    {
    if(i.group() == "GLOBALS")
      {
      for(; i.on_entry(); i.next())
        {
        if(i.entry() == "theme")
          {
          if(theme)
            std::cerr << "Warning: theme has already been specified at track file: "
                      << filename << std::endl;
          else
            theme = new ObjectsTheme(i.value());
          }
        else if(i.entry() == "track-width")
          size.x = atoi(i.value().c_str());
        else if(i.entry() == "track-height")
          size.y = atoi(i.value().c_str());
        else
          std::cerr << "Warning: unknown GLOBALS attribute: "
                    << i.entry() << std::endl;
        }
      }
    else if(i.group() == "place")
      {
      if(theme == NULL)
        {
        std::cerr << "Error: theme not specified at track file: "
                  << filename << std::endl;
        break;
        }
      if(sector.empty())
        {
        if(!size.x || !size.y)
          std::cerr << "Warning: Width and/or height has not been set.\n";
        sector.resize((size.x/theme->max_object_size.x) *
                      (size.y/theme->max_object_size.y));
        }

      object = new Object();
      for(; i.on_entry(); i.next())
        {
        if(i.entry() == "object")
          object->set_id(theme, atoi(i.value().c_str()));
        else if(i.entry() == "x")
          object->set_x(atoi(i.value().c_str()));
        else if(i.entry() == "y")
          object->set_y(atoi(i.value().c_str()));
        else
          std::cerr << "Warning: unknown object attribute: "
                    << i.entry() << std::endl;
        }

      /* Putting image into its respective sector. */
      sector[sector_index_of(object->pos)].push_back(object);
//std::cerr << "putting object " << object->pos().x << ", " << object->pos().y << " at " << sector_index_of(object->pos()) << std::endl;
      /* Check for special object proprieties and store them. */
      if(object->get_terrain_type() > TOTAL_TT)
        {
        TerrainType tt = object->get_terrain_type();
        if(tt == START_TT)
          players_pos.push_back(object->pos + Point(object->info->width()/2, object->info->height()));
        else if(tt >= CHECKPOINT_0_TT && tt <= CHECKPOINT_315_TT)
          {
          checkpoints.push_back(object->pos);
          checkpoints_angle.push_back((tt - CHECKPOINT_0_TT) * 45);
          }
        }
      }
    else
      std::cerr << "Warning: Unknown group: " << i.group() << " at "
                << filename << std::endl;
    }
  }

Track::~Track()
  {
  delete theme;
  for(Sector::iterator i = sector.begin(); i != sector.end(); i++)
    for(Objects::iterator j = (*i).begin(); j != (*i).end(); j++)
      delete *j;
  }

TerrainType Track::get_terrain_type(const Point& point)
  {
  TerrainType tt = NORMAL_TT, t;
  int index = sector_index_of(point);
  if(index == -1)
    return SLOW3_TT;
  for(Objects::iterator i = sector[index].begin(); i != sector[index].end(); i++)
    if((*i)->in_region_of(point))
      {
      t = (*i)->get_terrain_type();
      if(t != -1 && t > tt)
        tt = t;
      }
  return tt;
  }

TerrainType Track::get_terrain_type(const Rect& rect)
  {
  /* Let's store the edges to iterate them then. */
  int xpos[2] = { rect.x+2, rect.x + rect.w-4 };
  int ypos[2] = { rect.y+2, rect.y + rect.h-4 };

  TerrainType tt = NORMAL_TT, t;
  for (int x = 0; x < 2; x++)
    for (int y = 0; y < 2; y++)
      {
      t = get_terrain_type(Point(xpos[x],ypos[y]));
      if(t != -1 && t > tt)
        {   // check what's the worse terrain type -- that wins
        tt = t;
        }
      }
  return tt;
  }

#if 0
void Track::set_scale(float x_scale, float y_scale)
{
scale = Point(x_scale, y_scale);
if(scale == Point(1.0,1.0))
  return;
track_surface->scale(x_scale, y_scale);
}
#endif

void Track::draw(const Point& scroll, const Rect& screen_area)
  {
  for(int x = screen_area.w; x > -theme->max_object_size.x;
       x -= theme->max_object_size.x)
    for(int y = screen_area.h; y > -theme->max_object_size.y;
         y -= theme->max_object_size.y)
      {
      int index = sector_index_of(Point(x,y)+scroll);
      if(index == -1)
        continue;
      for(Objects::iterator i = sector[index].begin(); i != sector[index].end(); i++)
        {
        (*i)->draw(scroll);
        }
      }
  }

int Track::sector_index_of(const Point& p) const
  {
  int i = (p.x / theme->max_object_size.x) +
          ((p.y / theme->max_object_size.y) * (size.x/theme->max_object_size.x));
  if(i < 0 || i >= (int)sector.size())
    return -1;
  return i;
  }
