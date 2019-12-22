/***************************************************************************
                          menu.h  -  decription
                             -------------------
    begin                : Ter Abr 1 2003
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

#ifndef MENU_H
#define MENU_H

/**Controls menu: font, input...
  *@author Ricardo Cruz
  */

#include <string>
#include <vector>
#include <set>

#include "point.h"
#include "color.h"
#include "timer.h"

class Font;
class Surface;

enum EntryType {
  ENTRY_TITLE,
  ENTRY_ENTRY,
  ENTRY_TEXT1,
  ENTRY_TEXT2,
  ENTRY_SPACE
  };

class Menu;

void load_menu_resources();
void free_menu_resources();

class MenuEntry
{
public:
  MenuEntry(const std::string& text, int entry_type, int id = -1);
  ~MenuEntry();

  Font* get_font();
  Color& get_color(bool selected);

  int get_width();

private:
  friend class Menu;

  std::string text;
  int type;
  int id;
};

class Menu
{
public:
  Menu();
  ~Menu();

  void add_entry(const std::string& text, int entry_type, int id = -1);
  void add_entries(const std::set<std::string>& text);

  const std::string& get_selected_entry_text();
  int get_selected_entry_id();

  void set_selected_entry(const std::string& entry_text);

  const std::string& random_entry_text();

  // draws menu and returns id of selected entry
  // -1 to repeate
  // pass NULL to background to not draw it
	int show(Surface* background);

  static void set_current(Menu* menu);
  static void set_previous();
  static Menu* current;

private:
	static std::vector<Menu*> last_menus;

  void clear_events();
	void check_events();
  SDL_Event event;

	bool done;

  enum {
    NO_ACTION,
    UP_ACTION,
    DOWN_ACTION,
    ENTER_ACTION,
    ESCAPE_ACTION
    };
  int action;

	int selected_entry, last_selected_entry;
	std::vector <MenuEntry> entries;

  enum {
    NO_SEQ,
    START_SEQ,
    END_SEQ,
    UPDATE_SEQ // used for a hack
    };
  int sequence_type;
  Timer sequence_timer;
};

#endif
