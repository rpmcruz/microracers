/***************************************************************************
            objects.cpp  -  a track is an assembly of these "objects"
                             -------------------
    begin                : Sun Nov 13 2005
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

#include <iostream>

#include "objects.h"
#include "lib/surface.h"
#include "lib/bitmask.h"
#include "lib/ini_parser.h"
#include "lib/file_access.h"

Object::Object(ObjectInfo* info, int x, int y)
  : info(info)
  {
  pos = Point(x, y);
  }

Rect Object::rect()
  { return Rect(pos.x, pos.y, info->width(), info->height()); }

void Object::set_id(ObjectsTheme* theme, int id)
  {
  info = theme->get_object(id);
  }

TerrainType Object::get_terrain_type()
  {
  return info->terrain_type;
  }

void Object::draw(const Point& scroll, bool editor_mode)
  {
  if(info->surface)
    info->surface->draw(pos - scroll);
  else if(editor_mode && info->editor)
    info->editor->draw_transparent(pos - scroll);
  }

bool Object::in_region_of(const Point& point)
  {
  if(Rect::test_collision(point, rect()))
    {
    if(!info->bitmask)  // if no pixel collision
      return true;
    // if pixel collision, check for collision in the point
    return info->bitmask->has_bit(point.x-pos.x, point.y-pos.y);
    }
  return false;
  }

ObjectInfo::ObjectInfo()
  : surface(NULL), editor(NULL), frames_nb(1), terrain_type(NORMAL_TT), bitmask(NULL)
  { }

ObjectInfo::ObjectInfo(Surface* surface, Surface* editor, TerrainType terrain_type)
  : surface(surface), editor(editor), terrain_type(terrain_type), bitmask(NULL)
  {
  if(surface)
    {
    size.x = surface->w;
    size.y = surface->h;
    }
  else if(editor)
    {
    size.x = editor->w;
    size.y = editor->h;
    }
  }

ObjectInfo::~ObjectInfo()
  {
  if(surface)
    delete surface;
  if(bitmask)
    delete bitmask;
  }

void ObjectInfo::set_image(const std::string& filename, bool set_colorkey)
  {
  if(set_colorkey)
    surface = new Surface(filename, Color(255,0,255));
  else
    surface = new Surface(filename);

  size.x = surface->w;
  size.y = surface->h;
  }

void ObjectInfo::set_pixel_collision()
  {
  bitmask = new Bitmask(surface);
  }

void ObjectInfo::set_terrain_type(const std::string& tt)
  {
  terrain_type = NORMAL_TT;
  if(tt == "normal") ;
  else if(tt == "slow1")
    terrain_type = SLOW1_TT;
  else if(tt == "slow2")
    terrain_type = SLOW2_TT;
  else if(tt == "slow3")
    terrain_type = SLOW3_TT;
  else if(tt == "bounce")
    terrain_type = BOUNCE_TT;
  else if(tt == "start")
    terrain_type = START_TT;
  else
    std::cerr << "Warning: terrain type unknown: " << tt << std::endl;
  }

void ObjectInfo::draw_transparent(const Point& point, bool editor_mode)
  {
  if(surface)
    surface->draw_transparent(point);
  else if(editor_mode && editor)
    editor->draw_transparent(point);
  }

int ObjectInfo::width()
  {
  if(surface)
    return surface->w;
  if(editor)
    return editor->w;
  return 0;
  }

int ObjectInfo::height()
  {
  if(surface)
    return surface->h;
  if(editor)
    return editor->h;
  return 0;
  }

ObjectsTheme::ObjectsTheme(const std::string& theme_name, float scale)
  {
  name = theme_name;

  std::string dirname = datadir + "themes/" + name + "/";

  INIIterator i;
  ObjectInfo* object;

  for(i.open(dirname + "objects.conf"); !i.eof(); i.next())
    {
    object = new ObjectInfo();
    objects[atoi(i.group().c_str())] = object;
    for(; i.on_entry(); i.next())
      {
      if(i.entry() == "image")
        object->set_image(dirname + i.value(), false);
      if(i.entry() == "image-with-colorkey")
        object->set_image(dirname + i.value(), true);
      else if(i.entry() == "terrain-type")
        object->set_terrain_type(i.value());
      else if(i.entry() == "pixel-collision")
        {
        if(i.value() == "true")
          object->set_pixel_collision();
        }
      }

    if(object->surface)
      {
      if(scale != 1.0)
        object->surface->scale(scale, scale);
      max_object_size.x = std::max(max_object_size.x, object->surface->w);
      max_object_size.y = std::max(max_object_size.y, object->surface->h);
      }
    }

  /* Adding special checkpoints objects. */
  Surface* arrow = new Surface(datadir+"gui/arrow.png", Color(255,0,255));
  for(int i = 0; i < 8; i ++)
    {
    Surface* surface;
    if(i == 0) surface = arrow;
    else surface = new Surface(arrow->get_sdl_surface(), i*45);
    object = new ObjectInfo(NULL, surface, (TerrainType)(CHECKPOINT_0_TT+i));
    objects[-i-2] = object;
    }
  }

ObjectsTheme::~ObjectsTheme()
  {
  for(Map::iterator i = objects.begin(); i != objects.end(); i++)
    delete i->second;
  objects.clear();
  }

ObjectInfo* ObjectsTheme::get_object(int id)
  {
  Map::iterator i = objects.find(id);
  if(i == objects.end())
    {
    std::cerr << "Warning: object (" << id << ") was not found on "
              << name << " theme.\n";
    return NULL;
    }
  return i->second;
  }

int ObjectsTheme::get_id(const ObjectInfo* object)
  {
  for(Map::iterator i = objects.begin(); i != objects.end(); i++)
    if(i->second == object)
      return i->first;
  return -1;
  }

void ObjectsTheme::scale(float scale)
  {
  for(Map::iterator i = objects.begin(); i != objects.end(); i++)
    if(i->second->surface)
      i->second->surface->stretch((int)(scale*i->second->size.x), (int)(scale*i->second->size.y));
  }
