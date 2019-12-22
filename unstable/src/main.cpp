/***************************************************************************
                          main.cpp  -  main file
                             -------------------
    begin                : Sex Ago 30 11:06:32 WEST 2002
    copyright            : (C) 2002 by Ricardo Cruz
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
#include "trackeditor.h"

// includes AI controlled
#define TOTAL_PLAYERS 2

// globals:
bool debug_mode = false;

int main(int argc, char *argv[])
{
int video_flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
Point resolution(800, 600);
int bpp = 32;

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
		std::cout << "Microracers V" VERSION ", written by Ricardo Cruz"
    " <rpmcruz@clix.pt>\n"
		"\nUsage: " << argv[0] << " [OPTIONS]\n"
		"\t--fullscreen\tFull screen video mode\n"
    "\t--resolution X Y\tSpecify a XxY resolution\n"
    "\t--bpp X\tSpecify a bpp (bits per pixel)\n"
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
  else if (!strcmp(argv[i], "--resolution"))
    {
    if(argc < i+2)
      {
      std::cout << "Error: you didn't specify a resolution or at least you didn't"
                   "in a manner that I understand.\n";
      return 1;
      }
    resolution.x = atoi(argv[++i]);
    resolution.y = atoi(argv[++i]);
    // maybe we should see if the resolution is sane or not
    }
  else if (!strcmp(argv[i], "--bpp"))
    {
    if(argc < i+1)
      {
      std::cout << "Error: you didn't specify a bpp or at least you didn't"
                   "in a manner that I understand.\n";
      return 1;
      }
    bpp = atoi(argv[++i]);
    if(bpp != 8 && bpp != 15 && bpp != 16 && bpp != 24 && bpp != 32)
      {
      std::cout << "Specified bpp (" << bpp << ")doesn't look sane.\n"
                   "Tested values are 8, 15, 16, 24 and 32.\n";
      return 1;
      }
    }
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
  std::cerr << "Error: No data dir was found. Bailing out.\n";
  return 1;
  }
std::cout << "Using data dir '" << datadir << "', and home dir '" << homedir << "'.\n";

// initialize the SDL library
if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
	std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
	return 1;
	}

// initialize the display
if(Screen::open(resolution.x, resolution.y, bpp, video_flags) != 0)
  return 1;

SDL_WM_SetCaption("Microracers " VERSION, "Microracers");
SDL_Surface* icon = IMG_Load(std::string(datadir + "icon.xpm").c_str());
if(!icon)
  std::cerr << "Warning: could not load icon.\n";
SDL_WM_SetIcon(icon, NULL);
SDL_FreeSurface(icon);

load_resources();

if(!debug_mode)  // don't waste my time
  splash();

// feed random number generator
srand(SDL_GetTicks());

Race* race = new Race();

Menu::set_current(main_menu);

int players = 1;  // number of human players
bool done = false;
while(!done)
	{
	int ret = Menu::current->show(menu_background);
  if(Menu::current == main_menu)
    {
    switch(ret)
      {
      case MN_START:
//        Menu::set_current(players_nb_menu);
        Menu::set_current(tracks_menu);
        break;
      case MN_CREDITS:
        Menu::set_current(credits_menu);
        break;
      case MN_CONTROLS:
        Menu::set_current(controls_menu);
        break;
      case MN_TRACKEDITOR:
        {
        ret = tracks_editor_menu->show(menu_background);
        if(ret == MN_QUIT)
          break;
        if(ret == 2 /* input entry -> new track */ &&
           themes_editor_menu->show(menu_background) == MN_QUIT)
          break;
        TrackEditor trackeditor(tracks_editor_menu->get_selected_entry_text(),
                                themes_editor_menu->get_selected_entry_text());
        trackeditor.run();
        }
        Menu::set_current(main_menu);
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
        {
        Menu loading;
        loading.add_entry("Now Loading", ENTRY_TITLE);
        loading.add_entry("Please wait...", ENTRY_TEXT1);
        loading.show(menu_background);
        }

      // setting up AI cars
      for(i = players; i < TOTAL_PLAYERS; i++)
        race->add_vehicle(datadir + "vehicles/" + vehicles_menu->random_entry_text());

      // setting up the track
      race->setup_track(tracks_menu->get_selected_entry_text());

      Screen::fadeout(250);

      race->start(2);		// play until X laps (0 is forever)

      race->reset();  // opposite to the setups
      }
    Menu::set_current(main_menu);
    }
  }

free_resources();

delete race;

Screen::fadeout(200);
Screen::close();

// clean up on exit
SDL_Quit();

std::cout << "\nThank you for playing Microracers!\n"
             "We have kept you addictive for "
             << SDL_GetTicks() / 60000 << " minutes.\n\n";
return 0;
}
