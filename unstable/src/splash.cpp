/***************************************************************************
                          splash.cpp  -  description
                             -------------------
    begin                : Sat Jul 26 2003
    copyright            : (C) 2003 by Ricardo Cruz
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

#define SMALL_DELAY  25
#define BIG_DELAY  5000

#include "lib/font.h"
#include "lib/screen.h"
#include "lib/file_access.h"
#include "resources.h"
#include "globals.h"

void splash()
  {
  SDL_ShowCursor(SDL_DISABLE);

  const Point text_pos = Point(Screen::w / 6, Screen::h / 6);
  int color = 10;
  int number = 5;

  const char* text = "Microracers - a 2d top-view vehicle racing game\n"
    "Copyright (C) 2002-2005 Ricardo Cruz\n\n"
    "This program is free software; you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation; either version 2 of the License, or\n"
    "(at your option) any later version.\n\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program; if not, write to the Free Software\n"
    "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n\n"
    "I hope you like my game and feel free to contact me!";

  SDL_Event event;

  while(color >= 10)
    {
    // if someone presses a button, breaks the splash screen
    while(SDL_PollEvent(&event))
      {
      if(event.key.type == SDL_KEYUP || event.key.type == SDL_MOUSEBUTTONUP)
        if(number > 0)
          number = - number;
      if(event.type == SDL_QUIT)  // exit right now!
        color = 0;
      }

    small_font->draw_text(text, Color(color, color, color), text_pos);

    Screen::update();

    color += number;

    if(color >= 255)
      {
      number = -number;
      color = 255;
      }

    if(color == 255)
//      SDL_Delay(BIG_DELAY);
      for(int i = 0; i < BIG_DELAY; i += SMALL_DELAY)
        {
        while(SDL_PollEvent(&event))
          if(event.key.type == SDL_KEYUP || event.key.type == SDL_MOUSEBUTTONUP ||
             event.type == SDL_QUIT)
          i = BIG_DELAY;
        SDL_Delay(SMALL_DELAY);
        }
    else
      SDL_Delay(SMALL_DELAY);
    }

  Screen::clear_screen(Color::black);
  Screen::update();
  }
