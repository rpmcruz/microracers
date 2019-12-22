/***************************************************************************
                          resources.cpp  -  stuff used by more than one thing
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

#include "resources.h"
#include "lib/surface.h"
#include "lib/file_access.h"
#include "lib/menu.h"
#include "vehicle.h"

#include <iostream>

Surface* explosion;
Surface* menu_background;

Menu *main_menu, *controls_menu, *credits_menu, *vehicles_menu, *tracks_menu, *players_nb_menu;

void load_menus();
void free_menus();

void load_resources()
{
explosion = new Surface(datadir + "explosion.png", Color(255,0,255));
menu_background = new Surface(datadir + "menu.png");

load_menu_resources();
load_menus();

// setting default keys
set_default_keys(players_keymap[0], SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN, SDLK_RSHIFT);
set_default_keys(players_keymap[1], SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_f);
set_default_keys(players_keymap[2], SDLK_KP4, SDLK_KP6, SDLK_KP8, SDLK_KP2, SDLK_KP0);
set_default_keys(players_keymap[3], SDLK_h, SDLK_k, SDLK_u, SDLK_j, SDLK_l);
}

void free_resources()
{
delete explosion;
delete menu_background;

free_menu_resources();
free_menus();
}

void load_menus()
{  // Creating menu entries
main_menu = new Menu();
main_menu->add_entry("Start", ENTRY_ENTRY, MN_START);
main_menu->add_entry("Controls", ENTRY_ENTRY, MN_CONTROLS);
main_menu->add_entry("Credits", ENTRY_ENTRY, MN_CREDITS);
main_menu->add_entry("Quit", ENTRY_ENTRY, MN_QUIT);

credits_menu = new Menu();
credits_menu->add_entry("Credits", ENTRY_TITLE);
credits_menu->add_entry("Ricardo Cruz", ENTRY_TEXT1);
credits_menu->add_entry("  Programmer and Project Leader", ENTRY_TEXT2);
credits_menu->add_entry("Marinho Tobolla", ENTRY_TEXT1);
credits_menu->add_entry("  Game artist and web designer", ENTRY_TEXT2);
credits_menu->add_entry("Ok", ENTRY_ENTRY, MN_QUIT);

vehicles_menu = new Menu();
vehicles_menu->add_entry("Choose a Vehicle", ENTRY_TITLE);
vehicles_menu->add_entries(subdirs(datadir + "vehicles/"));
vehicles_menu->set_selected_entry("racingcar");
vehicles_menu->add_entry("", ENTRY_SPACE, MN_QUIT);
vehicles_menu->add_entry("Go Back", ENTRY_ENTRY, MN_QUIT);

tracks_menu = new Menu();
tracks_menu->add_entry("Choose a Track", ENTRY_TITLE);
tracks_menu->add_entries(subdirs(datadir + "tracks/"));
tracks_menu->set_selected_entry("blackboard");
tracks_menu->add_entry("", ENTRY_SPACE, MN_QUIT);
tracks_menu->add_entry("Go Back", ENTRY_ENTRY, MN_QUIT);

players_nb_menu = new Menu();
players_nb_menu->add_entry("Human Players", ENTRY_TITLE);
players_nb_menu->add_entry("One", ENTRY_ENTRY, 1);
players_nb_menu->add_entry("Two", ENTRY_ENTRY, 2);
players_nb_menu->add_entry("Three", ENTRY_ENTRY, 3);
players_nb_menu->add_entry("Four", ENTRY_ENTRY, 4);
players_nb_menu->add_entry("", ENTRY_SPACE, MN_QUIT);
players_nb_menu->add_entry("Go Back", ENTRY_ENTRY, MN_QUIT);

controls_menu = new Menu();
controls_menu->add_entry("Controls", ENTRY_TITLE);
controls_menu->add_entry("Keys not yet configurable -- sorry", ENTRY_TEXT1);
controls_menu->add_entry("Controls include direction and power if available", ENTRY_TEXT1);
controls_menu->add_entry("First player - Arrow keys and right shift", ENTRY_TEXT2);
controls_menu->add_entry("Second player - W S A D and F", ENTRY_TEXT2);
controls_menu->add_entry("Third player - Keypad 8 2 4 6 and 0", ENTRY_TEXT2);
controls_menu->add_entry("Forth player - U J H K and L", ENTRY_TEXT2);
controls_menu->add_entry("Ok", ENTRY_ENTRY, MN_QUIT);
}

void free_menus()
{
delete main_menu;
delete controls_menu;
delete credits_menu;
delete vehicles_menu;
delete tracks_menu;
delete players_nb_menu;
}
