/***************************************************************************
                          resources.h  -  stuff used by more than one thingf
                             -------------------
    begin                : Tue Nov 16 2004
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

#ifndef RESOURCES_H
#define RESOURCES_H

class Surface;

void load_resources();
void free_resources();

extern Surface* explosion;
extern Surface* menu_background;

enum {
  MN_QUIT = -1,
  MN_START,
  MN_CONTROLS,
  MN_CREDITS,
  MN_MAX
  };

class Menu;

extern Menu *main_menu, *controls_menu, *credits_menu,
            *vehicles_menu, *tracks_menu, *players_nb_menu;

void load_menus();
void free_menus();

#endif
