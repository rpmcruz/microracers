/***************************************************************************
                          menu.cpp  -  description
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

#include <iostream>

#include "menu.h"
#include "surface.h"
#include "font.h"
#include "screen.h"
#include "file_access.h"

#define X_POS 105
#define Y_POS 205
#define ENTRIES_Y_SPACE 30

#define SEQ_TIME 1000

Surface* menu_cursor;
Font *title_font, *entry_font, *text_font;
Color title_color, entry_selected_color, entry_unselected_color,
      text1_color, text2_color;
Menu* Menu::current = 0;
std::vector<Menu*> Menu::last_menus;

void load_menu_resources()
{
title_font = new Font(datadir + "ikart.ttf", 28);
entry_font = new Font(datadir + "ikart.ttf", 22);
text_font = new Font(datadir + "bluehigh.ttf", 16);

menu_cursor = new Surface(datadir + "menucursor.png", Color(255, 0, 255));

title_color =            Color(100, 100, 240);
entry_selected_color =   Color(255, 0, 0);
entry_unselected_color = Color(120, 0, 0);
text1_color =            Color(150, 160, 230);
text2_color =            Color(150, 210, 220);
}

void free_menu_resources()
{
delete title_font;
delete entry_font;
delete text_font;
delete menu_cursor;
}

MenuEntry::MenuEntry(const std::string& text_, int entry_type, int id_)
  : text(text_), type(entry_type), id(id_)
{
}

MenuEntry::~MenuEntry()
{
}

Font* MenuEntry::get_font()
{
switch(type)
  {
  case ENTRY_TITLE:
    return title_font;
  case ENTRY_ENTRY:
    return entry_font;
  case ENTRY_TEXT1:
  case ENTRY_TEXT2:
    return text_font;
  }
return NULL;  // avoids warnings
}

Color& MenuEntry::get_color(bool selected)
{
switch(type)
  {
  case ENTRY_TITLE:
    return title_color;
  case ENTRY_ENTRY:
    if(selected)
      return entry_selected_color;
    else
      return entry_unselected_color;
  case ENTRY_TEXT1:
    return text1_color;
  case ENTRY_TEXT2:
    return text2_color;
  }
return title_color;  // avoids warnings
}

int MenuEntry::get_width()
{
return get_font()->get_text_width(text);
}

Menu::Menu()
  : selected_entry(-1)
  { }

Menu::~Menu()
  { }

void Menu::add_entry(const std::string& text, int entry_type, int id)
{
if(selected_entry == -1 && entry_type == ENTRY_ENTRY)
  selected_entry = entries.size();

MenuEntry menu_entry(text, entry_type, id);
entries.push_back(menu_entry);
}

void Menu::add_entries(const std::set<std::string>& text)
  {
  selected_entry = entries.size();
  int i = 0;
  for(std::set<std::string>::iterator it = text.begin();
      it != text.end(); it++, i++)
    add_entry(*it, ENTRY_ENTRY, i);
  }

const std::string& Menu::get_selected_entry_text()
{
return entries[selected_entry].text;
}

int Menu::get_selected_entry_id()
{
return entries[selected_entry].id;
}

void Menu::set_selected_entry(const std::string& entry_text)
  {
  for(unsigned int i = 0; i < entries.size(); i++)
    if(entries[i].text == entry_text)
      {
      selected_entry = i;
      break;
      }
  }

const std::string& Menu::random_entry_text()
{
while(true)
  {
  MenuEntry& entry = entries[rand()%entries.size()];
  if(entry.type != ENTRY_ENTRY || entry.id == -1)
    continue;
  return entry.text;
  }
return entries[0].text;
}

int Menu::show(Surface* background)
{
// do a checking on the entries
//selected_entry = -1;
last_selected_entry = 0;
int entries_nb = 0;

for(int i = 0; i < (signed)entries.size(); i++)
  if(entries[i].type == ENTRY_ENTRY)
    entries_nb++;

sequence_type = START_SEQ;
sequence_timer.start(SEQ_TIME);
if (!entries_nb)  // no entries
  sequence_type = NO_SEQ;

clear_events();

SDL_ShowCursor(SDL_ENABLE);

done = false;
while(!done)
	{
  /* Drawing. */
	if (selected_entry != last_selected_entry || sequence_type != NO_SEQ)
		{  // entry changed
    Point pos;
    pos.x = X_POS;
		last_selected_entry = selected_entry;

    if(background)
		  background->draw(Point(0,0));

		for (int i = 0; i < (signed)entries.size(); i++)
			{			// write text
      if(entries[i].type == ENTRY_SPACE)
        continue;

      Font* font = entries[i].get_font();
      Color color = entries[i].get_color(i == selected_entry &&
                                         sequence_type != END_SEQ);

      if(sequence_type != NO_SEQ)
        {  // do an animation when menu is first displayed
        if(sequence_type == START_SEQ)
          {
          if(i % 2 == 0)
            pos.x = ((float)(X_POS-0)/sequence_timer.get_total_time()) *
                    sequence_timer.get_gone();
          else
            pos.x = screen->w-((float)(screen->w-X_POS-0) /
                    sequence_timer.get_total_time())*sequence_timer.get_gone();
          color.alpha = 255 * sequence_timer.get_gone() /
                        sequence_timer.get_total_time();
          }
        else if(sequence_type == END_SEQ)
          {
          if(i % 2 == 0)
            pos.x = ((float)(X_POS-0)/sequence_timer.get_total_time()) *
                    sequence_timer.get_left();
          else
            pos.x = screen->w-((float)(screen->w-X_POS-0) /
                    sequence_timer.get_total_time())*sequence_timer.get_left();
          color.alpha = 255 * sequence_timer.get_left() /
                        sequence_timer.get_total_time();
          }
        }
      pos.y = Y_POS + (i*ENTRIES_Y_SPACE);

      font->draw_text(entries[i].text, color, pos);

      if (i == selected_entry && sequence_type != END_SEQ)
        // put the cursor before the selected entry
        menu_cursor->draw(pos - Point(35,2));
      }
    screen->update();
    }

  /* Check events. */
  if(sequence_type != END_SEQ)
    {
    check_events();

    switch(action)
      {
      case NO_ACTION:
        break;
      case UP_ACTION:
        do
          {
          selected_entry--;
          if(selected_entry < 0)
            selected_entry = entries.size()-1;
          } while(entries[selected_entry].type != ENTRY_ENTRY);
        break;
      case DOWN_ACTION:
        do
          {
          selected_entry++;
          if(selected_entry >= (signed)entries.size())
            selected_entry = 0;
          } while(entries[selected_entry].type != ENTRY_ENTRY);
        break;
      case ESCAPE_ACTION:
        selected_entry = -1;
      case ENTER_ACTION:
        if(sequence_type == START_SEQ)
          {
          int take = sequence_timer.get_left();
          sequence_timer.start(SEQ_TIME);
          sequence_timer.take_time(take);
          }
        else
          sequence_timer.start(SEQ_TIME);
        sequence_type = END_SEQ;
        break;
      }
    }

  /* Check if finished. */
  if(sequence_type != NO_SEQ && !sequence_timer.check())
    {
    if(sequence_type == END_SEQ)
      done = true;
    if(sequence_type == UPDATE_SEQ)
      sequence_type = NO_SEQ;
    else
      sequence_type = UPDATE_SEQ;
    }
  if(!entries_nb)
    done = true;
  }

SDL_ShowCursor(SDL_DISABLE);

if(selected_entry == -1)
  return -1;
return entries[selected_entry].id;
}

void Menu::clear_events()
{
while(SDL_PollEvent(&event));
}

void Menu::check_events()
{
action = NO_ACTION;
while(SDL_PollEvent(&event) && action == NO_ACTION)
	{
	// testing SDL_KEYDOWN, SDL_KEYUP and SDL_QUIT events
	switch(event.type)
		{
		case SDL_KEYDOWN:	// key pressed
			switch(event.key.keysym.sym)
				{
				case SDLK_UP:
				case SDLK_RIGHT:
					action = UP_ACTION;
					break;
				case SDLK_DOWN:
				case SDLK_LEFT:
					action = DOWN_ACTION;
					break;
				case SDLK_RETURN:
				case SDLK_SPACE:
					action = ENTER_ACTION;
					break;
				default:
					break;
				}
			break;
		case SDL_KEYUP:	// key released
			switch(event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					action = ESCAPE_ACTION;
					break;
				default:
					break;
				}
			break;
		case SDL_QUIT:	// window closed
			action = ESCAPE_ACTION;
			break;
		case SDL_MOUSEMOTION:	// mouse moved
      {
      int entry = (event.motion.y-Y_POS)/ENTRIES_Y_SPACE;
      if(entry >= 0 && entry < (signed)entries.size())
        if(entries[entry].type == ENTRY_ENTRY)
          selected_entry = entry;
      }
      break;
		case SDL_MOUSEBUTTONUP:	// mouse button released
      {
      int entry = (event.motion.y-Y_POS)/ENTRIES_Y_SPACE;
      if(entry >= 0 && entry < (signed)entries.size())
        if(entries[entry].type == ENTRY_ENTRY)
          {
          selected_entry = entry;
          action = ENTER_ACTION;
          }
      }
      break;
		default:
			break;
		}
	}
}

void Menu::set_current(Menu* menu)
{
if(current)
  last_menus.push_back(current);
if(!menu)
  last_menus.clear();
current = menu;
}

void Menu::set_previous()
{
current = last_menus.front();
last_menus.erase(last_menus.begin());
}
