/***************************************************************************
                          vehicle.h  -  controls the cars
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

#ifndef VEHICLE_H
#define VEHICLE_H

/**
  *@author Ricardo Cruz
  */

#include <string>
#include <vector>

#include "lib/point.h"
#include "lib/color.h"
#include "lib/timer.h"
#include "globals.h"

// we need this for the Axis enum -- would be nice to remove the include...
#include "gameloop.h"

#define SKIP_ANGLE_RATIO 4

// colors available for replacement
enum {
  RED,
  GREEN,
  YELLOW,
  BLUE,
  CYAN,
  VIOLET,
  GRAY,
  BROWN,
  TOTAL_COLORS,
  SHADOW
  };

#define COMPUTER_PLAYER -1
#define MAX_HUMAN_PLAYERS 4

// directions
enum Direction {
  LEFT,
  RIGHT,
  ACCEL,
  BREAK,
  POWER,
  TOTAL_DIRECTIONS
  };

// players keys. Can be easily changed.
struct Keymap  {
  SDLKey key[TOTAL_DIRECTIONS];
  };
extern Keymap players_keymap[MAX_HUMAN_PLAYERS];
void set_default_keys(Keymap& keymap, SDLKey leftk, SDLKey rightk,
                      SDLKey accelk, SDLKey breakk, SDLKey powerk);

class Surface;

class Vehicle
{
public: 
	Vehicle(const std::string& data_path, int human_player);
	~Vehicle();

	void set_pos(const Point& new_pos)
    { pos = new_pos; }
	const Point& get_pos()
    { return pos; }
	const Point& get_old_pos()
    { return old_pos; }
	const Point& get_vel()
    { return vel; }
	void set_angle(int a);
	int get_angle()
    { return (int)angle; }

	const Point& get_size()
    { return size; }
  Rect get_rect()
    { return Rect(pos.x, pos.y, (int)size.x, (int)size.y); }

	void set_input(int dir, bool state)  // used by AI
    { key_pressed[dir] = state; }

	void set_input(SDLKey key, bool pressed);
  void stop_movement()
    { for(int i = 0; i < 4; i++) key_pressed[i] = false; }

  void jump(float vel_)
    { vel_z += vel_; }
  bool jumping()
    { return pos_z != 0; }
  float get_pos_z()
    { return pos_z; }

  enum DyingMode {
    NOT_DEAD = 0,
    EXPLODE_DEAD,
    MISSED_CHECKPNT_DEAD,
    FALLING_DEAD  // TODO
    };
  void kill(int dead_type);

	void check_track_pos(const Point& track_size);
  Point center_screen_pos(const Rect& screen_area);

	void handle_collision(Vehicle* collided, Axis axis);

	void update();
  void update_frame();

	void draw_shadow(const Point& scroll, const Rect& screen_area);
	void draw(const Point& scroll, const Rect& screen_area);
	void draw_icon(const Point& point);

	int get_lap_nb()
    { return lap_nb; }
	int get_current_checkpoint()
    { return checkpoint_nb; }
	int get_current_AI_point()
    { return cur_ai_point; }
  bool has_finished()
    { return finish_place; }
  void set_finish_place(int place)
    { stop_movement(); finish_place = place; }

	void change_checkpoint(int checkpoint, int checkpoints_nb);
	void change_AI_point(int point_nb, int max_points);

  void check_terrain();

  void cache_rotated_cars();
  void set_scale(float x_scale, float y_scale);
  void replace_color(int color_to_replace);
  int get_replacement_color()
    { return color; }

  static bool sort_on_jump (const Vehicle* v1, const Vehicle* v2)
    { return v1->pos_z > v2->pos_z; }

  bool is_human_player()
    { if(human_player == COMPUTER_PLAYER) return false; return true; }
  int get_human_player_nb()
    { return human_player; }

private:
  Point get_vehicle_reference_velocity() const;
  Point translate_to_world_reference(const Point& vel_vr) const;

  /* Position variables */
  Point pos, old_pos;
  float angle;  // in degrees
  double sin_angle, cos_angle;  // cache from last update()

  /* Velocity variables */
  Point vel;
  float rot_vel_power;

  /* Jumping position and velocity */
  float pos_z, old_pos_z, vel_z;

  /* Vehicle properties */
  float accel_power;
  float tire_friction, air_friction;

  // The input state
  bool key_pressed[TOTAL_DIRECTIONS];
  enum Powers {
    JUMP_POWER,
    SPEEDUP_POWER,
    FLY_POWER,
    NO_POWER
    };
  int power_type;

  // vector for frames, array for angles
	std::vector <Surface*> surfaces [360/SKIP_ANGLE_RATIO];
  // vehicles' shadows (needed for jumps visual feedback). They don't need frames.
	Surface* shadows  [360/SKIP_ANGLE_RATIO];
  Surface* icon;  // the surface drawn on the position tab

	Point size;

	int frame_nb;
	int total_frames;
  Timer frame_timer;

  Timer dying_timer;
  int dying;

	int terrain_type;
	int checkpoint_nb;
	int lap_nb;
  int finish_place;  // 0 if still on race

	int cur_ai_point;

  int color;

  int human_player;

  Point scale;  // use this for drawing
};

#endif
