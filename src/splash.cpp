/***************************************************************************
                          splash.cpp  -  description
                             -------------------
    begin                : Sáb Jul 26 2003
    copyright            : (C) 2003 by Ricardo Cruz
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

#define SMALL_DELAY 50
#define BIG_DELAY  4000

#include "lib/font.h"
#include "lib/screen.h"
#include "lib/file_access.h"
#include "globals.h"

#include <iostream>

void splash()
{
Font* font = new Font(datadir + "bluehigh.ttf", 12);

int color = 10;

char c_text[] = "Microracers - a 2d top-view car racing game\n"
	"Copyright (C) 2002-2004 Ricardo Cruz\n\n"
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

int number = 10;

Point text_pos;
text_pos.x = 100;

int i_lastTick, i_deltaTicks;

while(color >= 10)
	{
	i_lastTick = SDL_GetTicks();

	// if someone presses a button, breaks the splash screen
	while(SDL_PollEvent(&event))
    {
		if(event.key.type == SDL_KEYUP || event.key.type == SDL_MOUSEBUTTONUP)
			if(number > 0)                  
				number = -1 * number;
    if(event.type == SDL_QUIT)
      {
      color = 0;
      number = -10;
      }
    }

	text_pos.y = 100;

	unsigned int textI, tempI;

	for(textI = 0; textI < strlen(c_text); textI++)
		{
		char text[512];
    // doing some hacking to get line per line
		for(tempI = 0; c_text[textI] != '\0' && c_text[textI] != '\n'; textI++, tempI++)
      {
			text[tempI] = c_text[textI];
      }

		text[tempI] = '\0';

    font->draw_text(text, Color(color, color, color), text_pos);

		text_pos.y += 14;
		}
	screen->update();

	if(color + number > 255)
		{
		number = -number;
		color = 255;
		}
	else
		color += number;

  // calculating frame rate
	i_deltaTicks = SDL_GetTicks() - i_lastTick;
	if(i_deltaTicks > SMALL_DELAY && color != 255)
		continue;
	if(i_deltaTicks > BIG_DELAY && color == 255)
		continue;

	if(color == 255)
		SDL_Delay((int)(BIG_DELAY - i_deltaTicks));
	else
		SDL_Delay((int)(SMALL_DELAY - i_deltaTicks));
	}

delete font;
}
