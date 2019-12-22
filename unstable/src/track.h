/***************************************************************************
                          track.h  -  responsable for the map
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

#ifndef TRACK_H
#define TRACK_H

/**
  *@author Ricardo Cruz
  */

#include <vector>
#include <list>

#include "globals.h"
#include "lib/point.h"

class Surface;
class Object;
class ObjectsTheme;

// order matters to check importance in Vehicle
// (lower are less important)
enum TerrainType {
  NORMAL_TT,
  SLOW1_TT,
  SLOW2_TT,
  SLOW3_TT,
  FAST1_TT,
  FAST2_TT,
  FAST3_TT,
  JUMP1_TT,
  JUMP2_TT,
  JUMP3_TT,
  BOUNCE_TT,
  DEAD_TT,
  TOTAL_TT,
// special terrain types
  START_TT,
  CHECKPOINT_0_TT,
  CHECKPOINT_45_TT,
  CHECKPOINT_90_TT,
  CHECKPOINT_135_TT,
  CHECKPOINT_180_TT,
  CHECKPOINT_225_TT,
  CHECKPOINT_270_TT,
  CHECKPOINT_315_TT
  };

class Track
  {
  public: 
    Track(const std::string& data_path);
    ~Track();

    void draw(const Point& scroll, const Rect& screen_area);
  #if 0
    void set_scale(float x_scale, float y_scale);
  #endif
    TerrainType get_terrain_type(const Rect& rect);
    TerrainType get_terrain_type(const Point& point);

    Point get_player_pos(unsigned int nb)
      { if(nb > players_pos.size()) return players_pos[0];
        return players_pos[nb]; }
    int get_players_angle()
      { return players_angle; }

    const Point& get_size()
      { return size; }

    Point get_checkpoint(int nb)
      { if(nb >= get_total_checkpoints()) nb -= get_total_checkpoints();
        if(nb < 0) nb += get_total_checkpoints();
        return checkpoints[nb]; }
    int get_checkpoint_angle(int nb)
      { if(nb >= get_total_checkpoints()) nb -= get_total_checkpoints();
        if(nb < 0) nb += get_total_checkpoints();
        return checkpoints_angle[nb]; }

    int get_total_checkpoints()
      { return checkpoints.size(); }
    int get_checkpoint_range()
      { return checkpoint_range; }
    int get_checkpoint_miss_range()
      { return checkpoint_miss_range; }

  private:
    ObjectsTheme* theme;

    /* The vector keeps the different sectors, each is a rectangule with the
       size defined on max_object_size and that keeps a list of objects from
       that positions.
       This avoids iterating through a huge list of objects. */
    typedef std::list <Object*> Objects;
    typedef std::vector <Objects> Sector;
    Sector sector;
    /* Use this function to get the index of the sector for a given point.
       It returns -1 if it is out of range. */
    int sector_index_of(const Point& p) const;

    std::vector <Point> checkpoints;
    std::vector <int> checkpoints_angle;

    int checkpoint_range, checkpoint_miss_range;  // area that checkpoint covers

    std::vector <Point> players_pos;
    int players_angle;
  #if 0
    Point scale;  // use this for drawing
  #endif
    Point size;
  };

#endif
