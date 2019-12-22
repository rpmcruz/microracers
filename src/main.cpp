/***************************************************************************
                          main.cpp  -  main file
                             -------------------
    begin                : Sex Ago 30 11:06:32 WEST 2002
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

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

#include "lib/screen.h"
#include "lib/menu.h"
#include "lib/file_access.h"
#include "globals.h"
#include "splash.h"
#include "gameloop.h"
#include "resources.h"

// Current version:
#define VERSION "0.2"

// includes AI controlled
#define TOTAL_PLAYERS 6

// globals:
bool debug_mode = false;

int main(int argc, char *argv[])
{
int video_flags = SDL_HWSURFACE | SDL_DOUBLEBUF;

int players = 1;  // temporary variable
// parse arguments
for (int i = 1; i < argc; i++)
	{
	if (!strcmp(argv[i], "--version") || !strcmp(argv[i], "-v"))
		{
		std::cout << "Microracers V" VERSION ", written by Ricardo Cruz <rick2@aeiou.pt>\n\n";
		return 0;
		}
	else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h"))
		{
		std::cout << "Microracers V" VERSION ", written by Ricardo Cruz <rick2@aeiou.pt>\n"
		"\nUsage: " << argv[0] << " [OPTIONS]\n"
		"\t--fullscreen\tFull screen video mode\n"
		"\t--prefix [DIR]\tForce a data dir\n"
		"\t--debug\tDebug mode (enables some testing keys)\n"
		"\t--version\tShows program's version\n"
		"\t--help\tShows this help :-)\n\n";
		return 0;
		}
	else if (!strcmp(argv[i], "--fullscreen"))
		{
		video_flags |= SDL_FULLSCREEN;
		}
	else if (!strcmp(argv[i], "--prefix"))
		{
    i++;
    if(argc <= i)
      {
      std::cerr << "Error: No file was feed to --prefix.\n";
      return 0;
      }
    else
		  datadir = argv[i];
		}
	else if (!strcmp(argv[i], "--debug"))
		{
		debug_mode = true;
		}
  /* Temporary options for internal use. */
	else if (!strcmp(argv[i], "--2players"))
		players = 2;
	else if (!strcmp(argv[i], "--3players"))
		players = 3;
	else if (!strcmp(argv[i], "--4players"))
		players = 4;
  else
    {
    std::cout << "Error: Unrecognized paramter: " << argv[i] << std::endl
              << "Type: " << argv[0] << " --help to see available paramters.\n";
    return 0;
    }
	}

// hack to make it possible to execute microracers from eveywhere
if(!find_datadir(argv[0]))
  {
  std::cout << "Using data dir: " << datadir << std::endl;
  std::cerr << "Error: No data dir was found. Bailing out.\n";
  return 1;
  }
std::cout << "Using data dir: " << datadir << std::endl;

// initialize the SDL library
if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
	std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
	return 1;
	}

// initialize the display
screen = new Screen(640, 480, 16, video_flags);

// initialize the TTF library
if(TTF_Init() < 0)
	{
	std::cerr << "Couldn't initialize TTF: " << TTF_GetError() << std::endl;
	SDL_Quit();
	return 1;
	}

SDL_WM_SetCaption("Microracers " VERSION, "Microracers");
SDL_Surface* icon = IMG_Load(std::string(datadir + "icon.xpm").c_str());
if(!icon)
  std::cerr << "Warning: could not load icon.\n";
SDL_WM_SetIcon(icon, NULL);
SDL_FreeSurface(icon);

// feed random number generator
srand(SDL_GetTicks());

if(!debug_mode)  // dont' waste my time
  splash();

Race* race = new Race();

load_resources();

Menu::set_current(main_menu);

bool done = false;
while(!done)
	{
	int ret = Menu::current->show(menu_background);
  if(Menu::current == main_menu)
    {
    switch(ret)
      {
      case MN_START:
        Menu::set_current(players_nb_menu);
        break;
      case MN_CREDITS:
        Menu::set_current(credits_menu);
        break;
      case MN_CONTROLS:
        Menu::set_current(controls_menu);
        break;
      case MN_QUIT:
        done = true;
        break;
      }
    }
  else if(ret == -1)
    {
    Menu::set_previous();
    }
  else if(Menu::current == players_nb_menu)
    {
    players = players_nb_menu->get_selected_entry_id();
    Menu::set_current(tracks_menu);
    }
  else if(Menu::current == tracks_menu)
    {
    if(ret != MN_QUIT)
      {
      race->reset();

      // adding vehicles
      int i;
      for(i = 0; i < players; i++)
        {
        if(vehicles_menu->show(menu_background) == -1)
          break;
        std::string vehicle_path = datadir + "vehicles/" +
                                 vehicles_menu->get_selected_entry_text();
        race->add_vehicle(vehicle_path, true);
        }
      if(i != players)
        {
        Menu::set_current(main_menu);
        continue;
        }

      /* Automatic stuff now. */
      // setting up AI cars
      for(i = players; i < TOTAL_PLAYERS; i++)
        race->add_vehicle(datadir + "vehicles/" + vehicles_menu->random_entry_text());

      {
        Menu loading;
        loading.add_entry("Now Loading", ENTRY_TITLE);
        loading.add_entry("Please wait...", ENTRY_TEXT1);
        loading.show(menu_background);
      }
      // setting up the track
      race->setup_track(datadir + "tracks/" + tracks_menu->get_selected_entry_text());

      screen->fadeout(250);

      race->start(2);		// play until X laps (0 is forever)
      }
    Menu::set_current(main_menu);
    }
  }

free_resources();

delete race;

screen->fadeout(200);
delete screen;

// clean up on exit
TTF_Quit();
SDL_Quit();

std::cout << "\nThank you for playing Microracers!\n"
             "We have kept you addictive for "
             << SDL_GetTicks() / 60000 << " minutes.\n\n";
return 0;
}
