/***************************************************************************
                          camera.h  -  stores scrolling info
                             -------------------
    begin                : Mon Nov 01 2004
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

#include "camera.h" 
#include "lib/screen.h"
#include "gameloop.h"
#include "vehicle.h"
#include "track.h"

Camera::Camera(Vehicle* follow_)
  : follow(follow_)
{
}

Camera::~Camera()
{
}

void Camera::set_screen_area(const Rect& size)
{
screen_area = size;
}

void Camera::update_scrolling()
{
scroll = follow->center_screen_pos(screen_area);
}

int Camera::get_player_nb()
{
return follow->get_human_player_nb();
}

void Camera::draw(bool dont_draw_humans)
{
/* Set a clip to the current camera, so that everything is drawn
   inside it. */
Screen::set_clip_area(screen_area);

Race* race = Race::current;

race->track->draw(scroll, screen_area);

// Draw Vehicles
// draw shadows first to not overlap other vehicles graphics
/* This is a hack so that players that are in a higher Z position
   (jumping) are drawn above the others. */
for (Race::Vehicles::iterator i = race->vehicles.begin();
     i != race->vehicles.end(); i++)
  {
  if(dont_draw_humans && (*i)->is_human_player())
    continue;  // blink human players at start
  (*i)->draw_shadow(scroll, screen_area);
  }
//  for (Vehicles::iterator i = vehicles.begin(); i != vehicles.end(); i++)
//    (*i)->draw_shadow(scroll, Rect(0,0,Screen::w-80,Screen::h));
//  std::stable_sort(vehicles.begin(), vehicles.end(), Vehicle::sort_on_jump);
  {  // Hack to make jumping cars to be drawn above the others
  int order[race->vehicles.size()];
  for (unsigned int i = 0; i < race->vehicles.size(); i++)
    order[i] = -1;
  for (unsigned int i = 0; i < race->vehicles.size(); i++)
    {
    int over_nb = 0;
    for (unsigned int j = 0; j < race->vehicles.size(); j++)
      {
      if(j == i)
        continue;
      if(Vehicle::sort_on_jump(race->vehicles[i], race->vehicles[j]))
        over_nb++;
      }
    for (unsigned int j = over_nb; j < race->vehicles.size(); j++)
      if(order[j] == -1)
        {
        order[j] = i;
        break;
        }
    }
  for (unsigned int i = 0; i < race->vehicles.size(); i++)
    {
    if(dont_draw_humans && race->vehicles[i]->is_human_player())
      continue;  // blink human players at start
    if(order[i] != -1)
      race->vehicles[order[i]]->draw(scroll, screen_area);
    }
  }

/* Disable clipping now */
Screen::unset_clip_area();
}
