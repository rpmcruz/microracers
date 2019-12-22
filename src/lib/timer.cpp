/***************************************************************************
                          timer.h  -  stores timing info
                             -------------------
    begin                : Mon Nov 01 2004
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

#include <SDL.h>

#include "timer.h"

static int pause_first_tick,
           pause_acumulative_time = 0;

void pause_timers()
{
pause_first_tick = SDL_GetTicks();
}

void resume_timers()
{
pause_acumulative_time += SDL_GetTicks() - pause_first_tick;
}

Timer::Timer()
  : first_tick(0), time(0)
{
}

Timer::~Timer()
{
}

void Timer::start(int ms)
{
time = ms;
first_tick = SDL_GetTicks() - pause_acumulative_time;
}

void Timer::stop()
{
time = -1;
}

bool Timer::check()
{
if(time == 0)
  return false;
return get_left() > 0;
}

int Timer::get_left()
{
return time - get_gone();
}

int Timer::get_gone()
{
return SDL_GetTicks() - (first_tick + pause_acumulative_time);
}

int Timer::get_total_time()
{
return time;
}
