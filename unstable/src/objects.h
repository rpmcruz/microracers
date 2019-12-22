/***************************************************************************
             objects.h  -  a track is an assembly of these "objects"
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

#ifndef OBJECTS_H
#define OBJECTS_H

#include <map>

#include "lib/point.h"
#include "track.h"

class Surface;
class ObjectInfo;
class ObjectsTheme;
class Bitmask;

/** Concrete class that contain the actual objects of the track. */
class Object
  {
  public:
    Object()
      : info(NULL) { }
    Object(ObjectInfo* info, int x, int y);

    /* Loading funcs. */
    void set_id(ObjectsTheme* theme, int id);
    void set_x(int x)
      { pos.x = x; }
    void set_y(int y)
      { pos.y = y; }

    Rect rect();

    bool in_region_of(const Point& point);
    TerrainType get_terrain_type();

    void draw(const Point& scroll, bool editor_mode = false);

    ObjectInfo* info;

    Point pos;
  };

/** Abstract classes with the information on objects. */

struct ObjectInfo
  {
  ObjectInfo();
  ObjectInfo(Surface* surface, Surface* editor, TerrainType terrain_type);
  ~ObjectInfo();

  void set_image(const std::string& filename, bool use_colorkey);
  void set_terrain_type(const std::string& terrain_type);
  void set_pixel_collision();

  void draw_transparent(const Point& point, bool editor_mode = false);

  int width();
  int height();

  Surface* surface, *editor;
  Point size;
  int frames_nb;

  TerrainType terrain_type;
  Bitmask* bitmask;  // used in case pixel_collision is enabled
  };

/* In charge of loading a theme of objects into a map of ObjectInfo's. */
class ObjectsTheme
  {
  public:
    ObjectsTheme(const std::string& theme_name, float scale = 1.0);
    ~ObjectsTheme();

    ObjectInfo* get_object(int id);
    int get_id(const ObjectInfo* object);

    typedef std::map <int, ObjectInfo*> Map;
    Map objects;

    void scale(float scale);

    std::string name;  // name of the theme

    Point max_object_size;
  };

#endif /*OBJECTS_H*/
