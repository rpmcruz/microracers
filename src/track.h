/***************************************************************************
                          track.h  -  responsable for the map
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

#ifndef TRACK_H
#define TRACK_H

/**
  *@author Ricardo Cruz
  */

#include <vector>

#include "globals.h"
#include "lib/point.h"

class Surface;

// order matters to check importance in Vehicle
// (lower are more important)
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
  TOTAL_TT
  };

class Track
{
public: 
	Track(const std::string& data_path);
	~Track();

	void draw(const Point& scroll, const Rect& screen_area);

  void set_scale(float x_scale, float y_scale);

	TerrainType get_terrain_type(const Point& point);

	Point get_player_pos(unsigned int nb)
    { if(nb > players_pos.size()) return players_pos[0];
      return players_pos[nb]; }
	int get_players_angle()
    { return players_angle; }

  const Point& get_size()
    { return size; }

	Point get_checkpoint(int nb)
    { if(nb >= total_checkpoints) nb -= total_checkpoints;
      if(nb < 0) nb += total_checkpoints;
      return checkpoints[nb]; }
	int get_checkpoint_angle(int nb)
    { if(nb >= total_checkpoints) nb -= total_checkpoints;
      if(nb < 0) nb += total_checkpoints;
      return checkpoints_angle[nb]; }
	int get_total_checkpoints()
    { return total_checkpoints; }
	int get_checkpoint_range()
    { return checkpoint_range; }
	int get_checkpoint_miss_range()
    { return checkpoint_miss_range; }

private:
	Surface *track_surface, *track_terrain_surface;

	std::vector <Point> checkpoints;
	std::vector <int> checkpoints_angle;
	int total_checkpoints;
	int checkpoint_range, checkpoint_miss_range;  // area that checkpoint covers

	std::vector <Point> players_pos;
  int players_angle;

	Point size;
  Point scale;  // use this for drawing
};

#endif
