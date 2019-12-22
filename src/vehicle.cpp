/***************************************************************************
                          vehicle.cpp  -  controls the Vehicle
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
#include <math.h>

#include "lib/ini_parser.h"
#include "lib/surface.h"
#include "lib/file_access.h"
#include "resources.h"
#include "gameloop.h"
#include "vehicle.h"
#include "track.h"

// animation
#define FRAMES_DELAY 2000

#define ABS(x)  (x >= 0 ? x : -x)
#define SIGN(x) (x >= 0 ? 1 : -1)

#define EXPLOSION_FRAMES_NB 3

// speed consq of each terrain type
// TODO: a few of these values were not tuned yet.
const float TerrainTypeConsq[TOTAL_TT] = {
  1.00,  // NORMAL_TT
  4.00,  // SLOW1_TT
  8.00,  // SLOW2_TT
  12.00,  // SLOW3_TT
  0.80,  // FAST1_TT
  0.60,  // FAST2_TT
  0.40,  // FAST3_TT
  2.0,   // JUMP1_TT  // jump values are used for the Z velocity
  4.0,   // JUMP2_TT
  6.0,   // JUMP3_TT
  1.0,   // BOUNCE_TT  // used for the bounce impulse
  1.0    // DEAD_TT
  };

static int color_replacement[TOTAL_COLORS][3] = {
  {  0,   -1,  0 },  // GREEN
  { -1,   -1,  0 },  // YELLOW
  {  0,    0, -1 },  // BLUE
  {  0,   -1, -1 },  // CYAN
  { -1,    0, -1 },  // VIOLET
  { -1,   -1, -1 },  // GRAY
  { -1, -127,  0 }   // BROWN
  };

void set_default_keys(Keymap& keymap, SDLKey leftk, SDLKey rightk,
                      SDLKey accelk, SDLKey breakk, SDLKey powerk)
{  // TODO: turn Keymap into a class
keymap.key[LEFT] = leftk;
keymap.key[RIGHT] = rightk;
keymap.key[ACCEL] = accelk;
keymap.key[BREAK] = breakk;
keymap.key[POWER] = powerk;
}

Keymap players_keymap[MAX_HUMAN_PLAYERS];

Vehicle::Vehicle(const std::string& path, int human_player_)
  : angle(0), pos_z(0), vel_z(0), accel_power(1),
    power_type(NO_POWER), frame_nb(0),
    dying(NOT_DEAD), terrain_type(NORMAL_TT), checkpoint_nb(0), lap_nb(0),
    finish_place(0), cur_ai_point(0), human_player(human_player_),
    scale(1,1)
{
// initializating variables
for (int i = 0; i < 5; i++)
	key_pressed[i] = false;

// this values depend on the type of the car
// should be given by the file:
INIParser parser(path + "vehicle.conf");

total_frames = parser.get_int("[SURFACE]", "framesnb=");
frame_timer.start(0);

char str[256];
for(int f = 0; f < total_frames; f++)
  {
  sprintf(str, "vehicle%d.png", f+1);
  surfaces[0].push_back(new Surface(
              path + str, Color(255,0,255)));
  }
shadows[0] = new Surface(surfaces[0][0]->get_sdl_surface());

// colorization and rotating will happen further on (handled by Race)

// since they will rotate, there is not such thing as the bigger side.
// calculate the average size.
size.x = size.y = (surfaces[0][0]->w + surfaces[0][0]->h) / 2;

accel_power = parser.get_float("[PROPERTIES]", "accel_power=");
tire_friction = parser.get_float("[PROPERTIES]", "tire_friction=");
air_friction = parser.get_float("[PROPERTIES]", "air_friction=");
rot_vel_power = parser.get_float("[PROPERTIES]", "rotating_power=");

std::string power_str = parser.get_string("[PROPERTIES]", "power=");
if(power_str == "jump")
  power_type = JUMP_POWER;
else if(power_str == "speedup")
  power_type = SPEEDUP_POWER;
else if(power_str == "fly")
  power_type = FLY_POWER;
else if(power_str != "none")
  std::cerr << "Warning: power= parameter unknown: " << power_str
            << "\tfile: " << (path + "vehicle.conf") << std::endl;
}

Vehicle::~Vehicle()
{
for(int a = 0; a < 360/SKIP_ANGLE_RATIO; a ++)
  {
  for(std::vector <Surface*>::iterator i = surfaces[a].begin();
    i != surfaces[a].end(); i++)
    {
    delete (*i);
    surfaces[a].erase(i);
    }
  }
for(int a = 0; a < 360/SKIP_ANGLE_RATIO; a ++)
  delete shadows[a];
delete icon;
}

void Vehicle::set_angle(int a)
  {
  angle = a;
  while(angle >= 360)
    angle -= 360;
  while(angle < 0)
    angle += 360;
  }

void Vehicle::set_input(SDLKey key, bool pressed)
{
if(human_player == COMPUTER_PLAYER || finish_place)
  return;

for(int i = 0; i < TOTAL_DIRECTIONS; i++)
  if(players_keymap[human_player].key[i] == key)
    key_pressed[i] = pressed;
}

void Vehicle::kill(int dead_type)
{
if(dying)  // if already dying, ignore
  return;
dying = dead_type;

vel.x = vel.y = 0;
pos_z = 0;
old_pos = pos;

if(dead_type == EXPLODE_DEAD)
  checkpoint_nb--;
pos = Race::current->track->get_checkpoint(checkpoint_nb - 1);
set_angle(Race::current->track->get_checkpoint_angle(checkpoint_nb - 1));

if(dead_type == MISSED_CHECKPNT_DEAD)
  dying_timer.start(2000);
else
  dying_timer.start(6000);
}

void Vehicle::check_track_pos(const Point& track_size)
{		// check if car is inside the track
if (pos.x < 0)
	pos.x = 0;
if (pos.x + size.x > track_size.x)
	pos.x = track_size.x - size.x;

if (pos.y < 0)
	pos.y = 0;
if (pos.y + size.y > track_size.y)
	pos.y = track_size.y - size.y;
}

Point Vehicle::center_screen_pos(const Rect& screen_area)
{
Point center;

Point track_size = Race::current->track->get_size() * scale;

if(dying)
  {  // smooth camera movement since the crash point
  center.x = old_pos.x - screen_area.w/2;
  center.y = old_pos.y - screen_area.h/2;

  center.x += ((pos.x-old_pos.x)/dying_timer.get_total_time()) *
             dying_timer.get_gone();
  center.y += ((pos.y-old_pos.y)/dying_timer.get_total_time()) *
             dying_timer.get_gone();
  }
else
  {
  center.x = pos.x - screen_area.w/2;
  center.y = pos.y - screen_area.h/2;
  }

center *= scale;

if (center.x < 0)
	center.x = 0;
else if (center.x > track_size.x - screen_area.w)
	center.x = track_size.x - screen_area.w;

if (center.y < 0)
	center.y = 0;
else if (center.y > track_size.y - screen_area.h)
	center.y = track_size.y - screen_area.h;

return center;
}

Point Vehicle::get_vehicle_reference_velocity() const
  {
  Point vel_vr;
  vel_vr.x = (cos_angle * vel.y) + (sin_angle * vel.x);
  vel_vr.y = (-sin_angle * vel.y) + (cos_angle * vel.x);
  return vel_vr;
  }

Point Vehicle::translate_to_world_reference(const Point& vel_vr) const
  {
  return Point((cos_angle * vel_vr.y) + (sin_angle * vel_vr.x),
               (-sin_angle * vel_vr.y) + (cos_angle * vel_vr.x));
  }

/* Physics update. */

void Vehicle::update()
{
if(dying)
  {  // if dying, no need to update this stuff
  if(!dying_timer.check())
    dying = NOT_DEAD;
  else
    return;
  }

/* Rotation. */

if(key_pressed[POWER] && power_type == SPEEDUP_POWER)
  ;  // when speeding up, don't let player rotate car (it gotta have a cons)
else
  {
  if (key_pressed[LEFT])
    angle += rot_vel_power;
  if (key_pressed[RIGHT])
    angle -= rot_vel_power;
  }
// arranging angle
while(angle >= 360)
	angle -= 360;
while(angle < 0)
	angle += 360;

// X and Y axis movement

/* Power key. */
if(key_pressed[POWER])
  {    // use the power!
  if(power_type == JUMP_POWER)
    {
    if(pos_z == 0)  // if not already jumping
      vel_z += 4.0;
    key_pressed[POWER] = false;
    }
  }

  {
  float angle_rad = (angle*M_PI)/180;
  sin_angle = - sin(angle_rad);
  cos_angle = - cos(angle_rad);
  }

// translate world reference variables to vehicle reference
Point vel_vr, accel_vr;
vel_vr = get_vehicle_reference_velocity();

if((key_pressed[ACCEL] || key_pressed[BREAK]) && pos_z == 0)
  {
  float extra_power = 1.0;
  if(key_pressed[POWER] && power_type == SPEEDUP_POWER)
    extra_power = 2.0;

  if(key_pressed[BREAK])  // reversing
    extra_power *= - 0.5;

  accel_vr.x = accel_power * extra_power;
//  vel.x += accel * sin_angle;
//  vel.y += accel * cos_angle;
  }

/* Now, air friction and track friction. */
if(pos_z == 0)
  {  // if it is landed
  float friction = tire_friction;
  if(terrain_type >= NORMAL_TT && terrain_type <= FAST3_TT)
    friction *= TerrainTypeConsq[terrain_type];

  accel_vr.x -= (friction * vel_vr.x) + (air_friction * vel_vr.x * ABS(vel_vr.x));
  accel_vr.y -= (friction * vel_vr.y) + (air_friction * vel_vr.y * ABS(vel_vr.y));
  }

vel += translate_to_world_reference(accel_vr);// * scale;

if(pos_z != 0)
  vel_z -= 0.20;

// updating positions
old_pos = pos;
old_pos_z = pos_z;

pos.x += vel.x;
Race::current->check_collision(this, X_AXIS);
pos.y += vel.y;
Race::current->check_collision(this, Y_AXIS);

pos_z += vel_z;

if((int)pos_z <= 0)
  pos_z = vel_z = 0;

// update frames
update_frame();
}

void Vehicle::update_frame()
{
if (frame_timer.get_gone() > (float)FRAMES_DELAY / vel.norm())
	{    // is time to change frame?
	if (vel.x >= 0 && vel.y >= 0)    // going forward
		{
		frame_nb++;
		if (frame_nb >= total_frames)
			frame_nb = 0;
		}
	else   // going backward
		{
		frame_nb--;
		if (frame_nb == -1)
			frame_nb = total_frames - 1;
		}
	frame_timer.start(0);
	}
}

//#define ANGLE_IMPACT 3
void Vehicle::handle_collision(Vehicle* collided, Axis axis)
{
// make a point to this, to just to make things more readable
Vehicle* collider = this;

/* Flyers never collide, unless to each other. */
if((collider->power_type == FLY_POWER) xor (collided->power_type == FLY_POWER))
  return;

// if one just ended a jump, make him jumping again
if(collider->old_pos_z != 0)
  {
  collider->jump(2.0);
  collided->vel = 0;
  return;
  }

/* Split vehicles. */
if(axis == X_AXIS)  // X impact
  {
  if(collider->pos.x > collider->old_pos.x)  // right side impact
    collider->pos.x = collided->pos.x - collider->size.x - 1;
  else/* if(collider->vel.x < 0)*/  // left side impact
    collider->pos.x = collided->pos.x + collided->size.x + 1;
  }
else // Y impact
  {
  if(collider->pos.y > collider->old_pos.y)  // down side collision
    collider->pos.y = collided->pos.y - collider->size.y - 1;
  else/* if(collider->vel.y < 0)*/  // up side collision
    collider->pos.y = collided->pos.y + collided->size.y + 1;
  }

/* Now the actual re-action code. */
collided->vel += collider->vel * 2.0;

/* If collider or collided is dying, kill the other as well! */
if(collider->dying == EXPLODE_DEAD)
  {
  collided->kill(EXPLODE_DEAD);
  return;
  }
if(collided->dying == EXPLODE_DEAD)
  {
  collider->kill(EXPLODE_DEAD);
  return;
  }
}

void Vehicle::check_terrain()
{
if((vel.x == 0 && vel.y == 0) || power_type == FLY_POWER)
  return;  // don't need to check if car is stopped

int xpos[2];
xpos[0] = (int)pos.x;
xpos[1] = (int)(pos.x + size.x);
int ypos[2];
ypos[0] = (int)pos.y;
ypos[1] = (int)(pos.y + size.y);

terrain_type = NORMAL_TT;

for (int x = 0; x < 2; x++)
  for (int y = 0; y < 2; y++)
    {
    TerrainType tt;
    tt = Race::current->track->get_terrain_type(Point(xpos[x],ypos[y]));
    if (tt != -1 && tt > terrain_type)
      {   // check what's the worse terrain type -- that wins
      terrain_type = tt;
      }
    }

/* Do some actions according to terrain type: */
if(pos_z == 0)  // if landed
  switch(terrain_type)
    {
    case BOUNCE_TT:
      pos = old_pos;  // hack
      vel.x -= 6.0 * TerrainTypeConsq[terrain_type] * SIGN(vel.x);
      vel.y -= 6.0 * TerrainTypeConsq[terrain_type] * SIGN(vel.y);
      break;
    case DEAD_TT:
      kill(EXPLODE_DEAD);
      break;
    case JUMP1_TT:
    case JUMP2_TT:
    case JUMP3_TT:
      jump(TerrainTypeConsq[terrain_type]);
      break;
    default:
      break;  // avoids warnings
    }
}

void Vehicle::draw_shadow(const Point& scroll, const Rect& screen_area)
{
if(dying)
  return;
shadows[(int)(angle/SKIP_ANGLE_RATIO)]->draw(pos*scale - scroll + Point(screen_area));
}

void Vehicle::draw(const Point& scroll, const Rect& screen_area)
{
if(dying == EXPLODE_DEAD && dying_timer.get_gone() < 3000)
  {
// FIXME: should use a draw_frame_inside().
  explosion->draw_frame((old_pos*scale) - scroll + Point(screen_area),
    dying_timer.get_gone() * EXPLOSION_FRAMES_NB / 3000, EXPLOSION_FRAMES_NB);
  return;
  }
surfaces[(int)(angle/SKIP_ANGLE_RATIO)][frame_nb]->draw(
   (pos*scale) - scroll + Point(screen_area) + (scale*(4*pos_z
   + (power_type == FLY_POWER ? 50 : 0))));

/* Draw some status info */
// might be better to draw in separated func
// FIXME: it doesn't care about drawing inside of screen

if(finish_place)
  {  // draw position text over vehicle
  char str[32];
  if(finish_place == 1)
    sprintf(str, "1st");
  else if(finish_place == 2)
    sprintf(str, "2nd");
  else if(finish_place == 3)
    sprintf(str, "3rd");
  else  // 4 and so on
    sprintf(str, "%dth", finish_place);

  Race::current->draw_shadow_text(str,
        (pos*scale) + Point(size.x/2,-10) - scroll + Point(screen_area),
        CENTER_ALLIGN);
  }
}

void Vehicle::draw_icon(const Point& point)
{
icon->draw(point);
}

void Vehicle::change_checkpoint(int checkpoint, int maxcheckpoints)
{
checkpoint_nb = checkpoint;

if (checkpoint_nb == maxcheckpoints)
	{
	checkpoint_nb = 0;
	lap_nb++;
	}
}
void Vehicle::change_AI_point(int point_nb, int max_points)
{
cur_ai_point = point_nb;

if (cur_ai_point == max_points)
	cur_ai_point = 0;
}

void Vehicle::cache_rotated_cars()
{
icon = new Surface(surfaces[0][0]->get_sdl_surface());

for(int a = 1; a < 360 / SKIP_ANGLE_RATIO; a++)
  {
  for(int f = 0; f < total_frames; f++)
    {
    surfaces[a].push_back(new Surface(surfaces[0][f]->get_sdl_surface(),
                                      a*SKIP_ANGLE_RATIO));
    }
  shadows[a] = new Surface(shadows[0]->get_sdl_surface(),
                           a*SKIP_ANGLE_RATIO);
  }

// set some stuff on shadows
for(int a = 0; a < 360 / SKIP_ANGLE_RATIO; a++)
  {
  shadows[a]->add_offset(Point(2,2));
  shadows[a]->set_transparent(128);
  }
}

void Vehicle::set_scale(float x_scale, float y_scale)
{
scale = Point(x_scale, y_scale);

if(scale == Point(1.0,1.0))
  return;

for(int a = 0; a < 360 / SKIP_ANGLE_RATIO; a++)
  {
  for(int f = 0; f < total_frames; f++)
    surfaces[a][f]->stretch(x_scale, y_scale);
  shadows[a]->stretch(x_scale, y_scale);
  }
}

void Vehicle::replace_color(int color_to_replace)
{
color = color_to_replace;
if (color_to_replace != RED)
  {
  for(int f = 0; f < total_frames; f++)
    {
    surfaces[0][f]->lock_surface();

    for (int pix = 0; pix < 255; pix++)
      {
      int color[3];
      color[0] = color_replacement[color_to_replace][0];	// red
      color[1] = color_replacement[color_to_replace][1];	// green
      color[2] = color_replacement[color_to_replace][2];	// blue

      for (int clr = 0; clr < 3; clr++)
        {
        if (color[clr] == -1)
          color[clr] = pix;
        if (color[clr] == -127)
          color[clr] = pix/2;
        }

      surfaces[0][f]->replace_color(Color(pix, 2, 2),
                      Color(color[0], color[1], color[2]));
      }
    surfaces[0][f]->unlock_surface();
    }
  }

// color the shadow
shadows[0]->lock_surface();
for (int pix = 0; pix < 255; pix++)
  shadows[0]->paste_color(Color(0,0,0));
shadows[0]->unlock_surface();
}
