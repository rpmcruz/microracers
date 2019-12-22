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

#ifndef TIMER_H
#define TIMER_H 

void pause_timers();
void resume_timers();

class Timer
{
public:
  Timer();
  ~Timer();

  void start(int ms);  // 0 for forever
  void stop();

  /* True while time not reached. Else or when not started, false */
  bool check();

  int get_left();
  int get_gone();

  /* The time it was requested to time */
  int get_total_time();

  /* Take some time from the timer. */
  void take_time(int t)
    { first_tick -= t; }

private:
  int first_tick;
  int time;
};

#endif
