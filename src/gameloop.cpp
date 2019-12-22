/***************************************************************************
                          gameloop.cpp  -  responsible for the game itself
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

#include <iostream>
#include <math.h>

#include "gameloop.h"
#include "lib/font.h"
#include "lib/timer.h"
#include "lib/screen.h"
#include "lib/file_access.h"
#include "lib/surface.h"
#include "globals.h"
#include "vehicle.h"
#include "track.h"
#include "camera.h"

// in millisecons
#define FIXED_RATE 10

// affects AI behavior:
#define AIANGLEDIFF 10
#define AIPOSDIFF 15

enum Sequence {
  START_SEQ,
  FINISH_SEQ,
  NO_SEQ
  };

Race* Race::current = 0;

Race::Race()
  : track(0)
{
lap_font = new Font(datadir + "bluehigh.ttf", 13);
panel = new Surface(datadir + "panel.png");
traffic_lights = new Surface(datadir + "lights.png", Color(255,0,255));
}

Race::~Race()
{
delete lap_font;
delete panel;
delete traffic_lights;
reset();
}

void Race::reset()
{
// remove track and vehicles
if(track)
  delete track;

for (Vehicles::iterator i = vehicles.begin(); i < vehicles.end(); i++)
  delete *i;
vehicles.clear();
total_human_players = 0;
for (Cameras::iterator i = cameras.begin(); i < cameras.end(); i++)
  delete *i;
cameras.clear();
}

void Race::add_vehicle(const std::string& path, bool human_player)
{
int player;
if(human_player)
  {
  player = total_human_players;
  total_human_players++;
  if(total_human_players > MAX_HUMAN_PLAYERS)
    std::cerr << "Warning: More human players chosen than possible.\n";
  }
else
  player = COMPUTER_PLAYER;

Vehicle* veh = new Vehicle(path + "/", player);
if(player != COMPUTER_PLAYER)
  cameras.push_back(new Camera(veh));

int color_to_replace;
if(TOTAL_COLORS > vehicles.size())
  {
  color_to_replace = rand() % (TOTAL_COLORS-vehicles.size());
  // check if this color has already been attributed
  // if so, change it, so that there are not repetited colors
  bool already_used;
  do
    {
    already_used = false;
    for(Vehicles::iterator i = vehicles.begin(); i < vehicles.end(); i++)
      if(color_to_replace == (*i)->get_replacement_color())
        {
        already_used = true;
        color_to_replace++;  // change to next color
        if(color_to_replace == TOTAL_COLORS)
          color_to_replace = 0;
         break;
        }
    } while(already_used);
  }
else
  color_to_replace = rand() % TOTAL_COLORS;
veh->replace_color(color_to_replace);
veh->cache_rotated_cars();

vehicles.push_back(veh);
}

void Race::setup_track(const std::string& path)
{
track = new Track(path + "/");
}

void Race::start(int max_lap)
{
// check if we can start
if(!vehicles.size())
  {
  std::cerr << "Error: no vehicle was added.\n";
  return;
  }
if(!track)
  {
  std::cerr << "Error: track was not initialized.\n";
  return;
  }
if(total_human_players < 1)
  std::cerr << "Warning: No human players were selected.\n";

// Calculate different screen areas for the different cameras
  {
  int w = screen->w-80, h = screen->h;
  if(total_human_players > 2)
    w /= 2;
  if(total_human_players != 1)
    h /= 2;

  for (Cameras::iterator i = cameras.begin(); i < cameras.end(); i++)
    {
    int x, y;
    int player = (*i)->get_player_nb() + 1;
    if(player == 1 || (player == 2 && total_human_players == 2) || player == 3)
      x = 0;
    else// if(player == 2 || player == 4)
      x = w;

    if(player == 1 || (player == 2 && total_human_players > 2))
      y = 0;
    else// if(player == 2 || player == 4)
      y = h;

    (*i)->set_screen_area(Rect(x,y,w,h));
    }
  }

// resize track and vehicles because of the split screens
  {
  Point scale(1.0,1.0);
  if(total_human_players >= 2)
    {
    scale.y = 0.5;
    if(total_human_players > 2)
      scale.x = 0.5;
    }
  for (Vehicles::iterator i = vehicles.begin(); i != vehicles.end(); i++)
    (*i)->set_scale(scale.x, scale.y);
  track->set_scale(scale.x, scale.y);
  }

for (unsigned int i = 0; i < vehicles.size(); i++)
  {
  vehicles[i]->set_pos(track->get_player_pos(i));
  vehicles[i]->set_angle(track->get_players_angle());
  }

int last_place = 1;

this->activate();

Timer frame_rate;
unsigned int timestep_accumulator = 0;
frame_rate.start(0);

Timer fps_timer;
fps_timer.start(1000);
int last_fps, frame_counter;
last_fps = frame_counter = 0;

debug_delay_fps = 0;

Timer sequence_timer; // used for traffic light sequence and a future finish seq
sequence_timer.start(3000);
Sequence sequence_type = START_SEQ;

clear_events();

game_paused = false;
done = false;
while (!done)
  {
  check_events();
  AI_movement();   // AI for computer cars

  // update vel, angle and pos, and check for collision
  if(sequence_type != START_SEQ)
    {
    /* Calculate the time step. */
    timestep_accumulator += frame_rate.get_gone();
    frame_rate.start(0);  // reset frame rate counter

    if(timestep_accumulator > 200)
      timestep_accumulator = 200;  // might have forced to sleep or something
    if(timestep_accumulator < FIXED_RATE)
      {
      SDL_Delay(FIXED_RATE - timestep_accumulator);
      timestep_accumulator = FIXED_RATE;
      }
    /* Now update physics. */
    for(; timestep_accumulator >= FIXED_RATE;
          timestep_accumulator -= FIXED_RATE)
      {
      for (Vehicles::iterator i = vehicles.begin(); i != vehicles.end(); i++)
        // update will take care of calling collision tests
        (*i)->update();
      }
    }
  else
    frame_rate.start(0);  // reset frame rate counter

  // checking for position on track...	it cannot be outside the trackG
  for (Vehicles::iterator i = vehicles.begin(); i != vehicles.end(); i++)
    (*i)->check_track_pos(track->get_size());

  // checking for checkpoints 
  for (Vehicles::iterator i = vehicles.begin(); i != vehicles.end(); i++)
    {
    Point pos;
    pos = (*i)->get_pos() + (*i)->get_size()/2;

    Point ref_point = track->get_checkpoint((*i)->get_current_checkpoint());
    int ref_range = track->get_checkpoint_range();

    if (pos.x >= ref_point.x - ref_range/2 &&
        pos.x <= ref_point.x + ref_range/2 &&
        pos.y >= ref_point.y - ref_range/2 &&
        pos.y <= ref_point.y + ref_range/2)
      (*i)->change_checkpoint((*i)->get_current_checkpoint()+1,
                              track->get_total_checkpoints());
    /* If the players get too far away from the checkpoint, we consider
       it to have lost it, so we will kill him to go to his current one. */
    else
      {
      ref_range = track->get_checkpoint_miss_range();
      if (!(pos.x >= ref_point.x - ref_range &&
            pos.x <= ref_point.x + ref_range &&
            pos.y >= ref_point.y - ref_range &&
            pos.y <= ref_point.y + ref_range))
        (*i)->kill(Vehicle::MISSED_CHECKPNT_DEAD);
      }
    }

  // checking for terraintypes
  for (Vehicles::iterator i = vehicles.begin(); i != vehicles.end(); i++)
    (*i)->check_terrain();  // vehicle will access Track themselves

  /* Drawing */
  // scrolling screen and drawing...
  // This should be done by the camera
  for (Cameras::iterator i = cameras.begin(); i < cameras.end(); i++)
    {
    (*i)->update_scrolling();

    if(sequence_type == START_SEQ && sequence_timer.get_gone() % 1000 < 400)
      (*i)->draw(true);  // blink players on startup of the race
    else
      (*i)->draw();
    }

  // draw camera boarders
  if(total_human_players > 1)
    {  // any camera is good enough to ask for the screen size
    Point size = cameras[0]->get_screen_size();
    if(total_human_players > 2)
      {
      if(total_human_players == 3)
        screen->fill_rect(Rect(size.x,size.y,(int)size.x,(int)size.y),Color(30,30,30));
      screen->fill_rect(Rect(size.x,0,1,screen->h),Color(0,0,0));
      }
    screen->fill_rect(Rect(0,size.y,screen->w-80,1),Color(0,0,0));
    }

  {  // check the current lap by seeing what car has made the most
  int lap_nb = 0;
  for (Vehicles::iterator i = vehicles.begin(); i != vehicles.end(); i++)
    if((*i)->get_lap_nb() > lap_nb)
      lap_nb = (*i)->get_lap_nb();
  draw_info_text(lap_nb, max_lap, last_fps);
  }

  if(sequence_type == START_SEQ && sequence_timer.check())
    traffic_lights->draw_frame(Point(screen->w/2-84, screen->h/2-45),
           sequence_timer.get_gone() * 6 / sequence_timer.get_total_time(), 6);

  screen->update();

  /* Check for winners. */
  if (max_lap > 0)
    {
    for (unsigned int i = 0; i < vehicles.size(); i++)
      if (vehicles[i]->get_lap_nb() == max_lap)
        {
        if(sequence_type != FINISH_SEQ)
          {  // do this only on the first car crossing the line
          sequence_type = FINISH_SEQ;
          sequence_timer.start(10000);
          std::cout << "Player " << i+1 << " is the WINNER!\n";
          }
        if(!vehicles[i]->has_finished())
          {  // record place
          vehicles[i]->set_finish_place(last_place);
          last_place++;
          if(sequence_timer.get_gone() > 5000)
            sequence_timer.start(5000);  // add some more time
          }
        }
    }

  /* Check sequences. */
  if(sequence_type == START_SEQ && !sequence_timer.check())
    sequence_type = NO_SEQ;
  if(sequence_type == FINISH_SEQ && !sequence_timer.check())
    done = true;

  /* FPS calculation. */
  if(debug_delay_fps)
    SDL_Delay(debug_delay_fps);

  // not a very accurate way to calculate fps, but good enough
  // for what I want
  frame_counter++;
  if(!fps_timer.check())
    {
    fps_timer.start(1000);
    last_fps = frame_counter;
    frame_counter = 0;
    }
  }

screen->fadeout(500);

return;
}

void Race::draw_shadow_text(const std::string& text, const Point& point, Allignment allignment)
{
lap_font->draw_text(text, Color(0,0,0), point+Point(2,2), allignment);
lap_font->draw_text(text, Color(255,255,255), point, allignment);
}

void Race::draw_info_text(int lapnumber, int maxlaps, int fps)
{
panel->draw(Point(screen->w-80, 0));

char str[32];
sprintf(str, "lap: %d/%d", lapnumber, maxlaps);
lap_font->draw_text(str, Color(175, 175, 240), Point(screen->w - 65, 60));

// debug information:
if(fps != -1)
  {
  sprintf(str, "FPS: %d", fps);
  lap_font->draw_text(str, Color(255, 255, 255), Point(screen->w - 62, 5+450));
  }
if(debug_mode)
  {
  sprintf(str, "vel.x: %.2f", vehicles[0]->get_vel().x);
  lap_font->draw_text(str, Color(255, 255, 255), Point(screen->w - 72, 5+410));
  sprintf(str, "vel.y: %.2f", vehicles[0]->get_vel().y);
  lap_font->draw_text(str, Color(255, 255, 255), Point(screen->w - 72, 5+420));
  }
// draw players position on track
int pos;
for (Vehicles::iterator i = vehicles.begin(); i != vehicles.end(); i++)
  {
  pos = 0;  // relative position
  for (Vehicles::iterator j = vehicles.begin(); j < vehicles.end(); j++)
    {
    if (*i == *j)
      continue;

    if((*i)->get_lap_nb() < (*j)->get_lap_nb())
      pos++;
    else if((*i)->get_lap_nb() == (*j)->get_lap_nb())
      {
      if((*i)->get_current_checkpoint() < (*j)->get_current_checkpoint())
        pos++;
      else if((*i)->get_current_checkpoint() == (*j)->get_current_checkpoint())
        {
        Point next_checkpoint =
          track->get_checkpoint((*i)->get_current_checkpoint());
        Point i_dist = (*i)->get_pos() - next_checkpoint;
        Point j_dist = (*j)->get_pos() - next_checkpoint;
        if(i_dist > j_dist)
          pos++;
        }
      }
    }
  // according to the fact that the position number is odd or not,
  // allign icon to left or right
  if(pos % 2 == 0)
    (*i)->draw_icon(Point(screen->w-70, 90+20*pos));
  else
    (*i)->draw_icon(Point(screen->w-70+35, 90+20*pos));
  }
}

void Race::AI_movement()
{
for(Vehicles::iterator i = vehicles.begin(); i != vehicles.end(); i++)
  {
  if((*i)->is_human_player() || (*i)->has_finished())
    continue;

  // move according to referencial points
  float angleCar = (*i)->get_angle() + 90;

  while (angleCar >= 360)
    angleCar -= 360;
  while (angleCar < 0)
    angleCar += 360;

  float angleCarSimetric = angleCar + 180;

  while (angleCarSimetric >= 360)
    angleCarSimetric -=360;
  while (angleCarSimetric < 0)
    angleCarSimetric +=360;

  Point pos = (*i)->get_pos();
  pos.x += (*i)->get_size().x/2;
  pos.y += (*i)->get_size().y/2;

  Point ref_point = track->get_checkpoint((*i)->get_current_AI_point());

  (*i)->set_input(ACCEL, false);		// reset
  (*i)->set_input(LEFT, false);
  (*i)->set_input(RIGHT, false);

  float deltaX = ref_point.x - pos.x;
  float deltaY = pos.y - ref_point.y;

  int changeToAngle = (int)(((atan(deltaY/deltaX))/M_PI)*180);

  if(deltaX < 0)
    changeToAngle = 180 + changeToAngle;

  while(changeToAngle >= 360)
    changeToAngle -= 360;
  while(changeToAngle < 0)
    changeToAngle += 360;

  bool b_directionChanged = false;

  if(abs((int)(changeToAngle - angleCar)) <= AIANGLEDIFF || abs((int)(changeToAngle - angleCar)) >= 360 - AIANGLEDIFF)
    {
    (*i)->set_input(ACCEL, true);
    b_directionChanged = true;
    }

  if (b_directionChanged == false)
    {
    // see if point is before angleCar and after angleCarSimetric
    for (int left = (int)angleCar; left != angleCarSimetric; left++)
      {
      if (left < 0)
        left += 360;
      if (left >= 360)
        left -= 360;

      if (left == changeToAngle)
        {
        (*i)->set_input(LEFT, true);
        b_directionChanged = true;
        break;
        }
      }
    }

  if (b_directionChanged == false)
    {
    // see if point is after angleCar and before angleCarSimetric
    for (int right = (int)angleCar; right != angleCarSimetric; right--)
      {
      if (right < 0)
        right += 360;
      if (right >= 360)
        right -= 360;

      if (right == changeToAngle)
        {
        (*i)->set_input(RIGHT, true);
        b_directionChanged = true;
        break;
        }
      }
    }

  if (b_directionChanged == false)
    (*i)->set_input(RIGHT, true);

  if (abs(int(pos.x - ref_point.x)) <= AIPOSDIFF &&
      abs(int(pos.y - ref_point.y)) <= AIPOSDIFF)
    (*i)->change_AI_point((*i)->get_current_AI_point() + 1,
                          track->get_total_checkpoints());
  }
}

bool Race::test_collision(const Rect& rect1, const Rect& rect2)
{
// very simple box collision detection
return !(rect1.x + rect1.w < rect2.x || rect2.x + rect2.w < rect1.x ||
         rect1.y + rect1.h < rect2.y || rect2.y + rect2.h < rect1.y);
}

void Race::check_collision(Vehicle* vehicle, Axis axis)
  {
  if(vehicle->jumping())
    return;  // if in air, don't check collisions

  for(Vehicles::iterator i = vehicles.begin(); i < vehicles.end(); i++)
    {
    if (vehicle == *i || (*i)->jumping())
      continue;

    if (test_collision(vehicle->get_rect(), (*i)->get_rect()))
      vehicle->handle_collision(*i, axis);
    }
  }

void Race::clear_events()
{
while(SDL_PollEvent(&event));
}

void Race::check_events()
{
/* Poll for events. SDL_PollEvent() returns 0 when there are no  */
/* more events on the event queue, our while loop will exit when */
/* that occurs.                                                  */
while(SDL_PollEvent(&event))
  {
  // testing SDL_KEYDOWN, SDL_KEYUP and SDL_QUIT events
  switch(event.type)
    {
    case SDL_KEYDOWN:	// key pressed
      {
      SDLKey key = event.key.keysym.sym;
      for(Vehicles::iterator i = vehicles.begin(); i != vehicles.end(); i++)
        (*i)->set_input(key, true);

      if(key == SDLK_RETURN && event.key.keysym.mod & KMOD_ALT)
        screen->toggle_fullscreen();

      switch(key)
        {
        case SDLK_p:
          game_paused = !game_paused;
          if(!game_paused)
            resume_timers();
          else
            {
            pause_timers();
            screen->fill_rect(Rect(0,0,screen->w,screen->h),
                              Color(190,190,230,128));
            lap_font->draw_text("Game Paused", Color(255, 255, 255),
                                Point(screen->w/2, screen->h/2-10),
                                CENTER_ALLIGN);
            lap_font->draw_text("P to resume", Color(175, 175, 240),
                                Point(screen->w/2, screen->h/2+10),
                                CENTER_ALLIGN);
            if(debug_mode)
              lap_font->draw_text("Debug mode", Color(255,0,0),
                                  Point(screen->w/2, 100),
                                  CENTER_ALLIGN);
            screen->update();
            while(game_paused && !done)
              {
              SDL_Delay(250);  // dont waste all cpu on this
              check_events();
              }
            }
          break;
        case SDLK_PLUS:
          if(debug_mode)
            debug_delay_fps += 5;
        case SDLK_MINUS:
          if(debug_mode)
            if(debug_delay_fps > 0)
              debug_delay_fps -= 5;
        case SDLK_d:
          if(debug_mode)
            vehicles[0]->kill(Vehicle::EXPLODE_DEAD);
        default:
          break;
        }
      }
      break;
    case SDL_KEYUP:	// key released
      {
      SDLKey key = event.key.keysym.sym;
      for(Vehicles::iterator i = vehicles.begin(); i != vehicles.end(); i++)
        (*i)->set_input(key, false);

      if(key == SDLK_ESCAPE)
        if(!game_paused)
          done = true;
      }
      break;
    case SDL_QUIT:	// window closed
      done = true;
      break;
    default:
      break;
    }
  }
}
