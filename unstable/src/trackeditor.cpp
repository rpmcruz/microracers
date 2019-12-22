/***************************************************************************
                  trackeditor.cpp  -  built'in track editor
                             -------------------
    begin                : June, 23 2004
    copyright            : (C) 2004 by Ricardo Cruz
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

#include <iostream>
#include<bits/stdc++.h>
#include <SDL.h>

#include "trackeditor.h"
#include "lib/screen.h"
#include "lib/file_access.h"
#include "lib/font.h"
#include "lib/surface.h"
#include "lib/ini_parser.h"
#include "lib/menu.h"
#include "objects.h"
#include "track.h"
#include "gameloop.h"
#include "resources.h"

#define ABS(x) (x >= 0 ? x : -x)

static SDL_Event event;

TrackEditor::TrackEditor(const std::string& track_name, const std::string& theme_name)
  : use_grid(true), zoom(1.0), selected_object(-1), hovered_object(NULL), level_changed(false)
  {
  /* Creating button groups */
  filename = track_name;
  theme = 0;
  load_level();
  if(!theme)  // in case the level exists and the theme is set
    theme = new ObjectsTheme(theme_name);

  tiles_board = new ButtonGroup(Point(Screen::w - 150, 100), Point(50,50), Point(3,6));
  tiles_board->add_button(new Button(datadir+"gui/rubber.png", "Eraser", SDLK_DELETE), -1, true);

  /* Add ordinary tiles. */
  for(ObjectsTheme::Map::iterator i = theme->objects.begin(); i != theme->objects.end(); i++)
    {
    if(i->first >= 0)
      {
      Button* button = new Button(i->second->surface, "", SDLKey(SDLK_a+i->first));
      tiles_board->add_button(button, i->first);
      }
    }
  /* Add special tiles to bottom -- otherwise, it would add them at top. */
  for(ObjectsTheme::Map::reverse_iterator i = theme->objects.rbegin(); i != theme->objects.rend(); i++)
    {
    if(i->first < 0)
      {
      Button* button = new Button(i->second->editor, "", SDLKey(SDLK_KP0-i->first-2));
      tiles_board->add_button(button, i->first);
      }
    }

  level_options = new ButtonGroup(Point(Screen::w-150, Screen::h-36), Point(36, 36), Point(3,1));
  level_options->add_button(new Button(datadir+"gui/test.png", "Test level", SDLK_F2), BT_TEST);
  level_options->add_button(new Button(datadir+"gui/grid.png", "Change grid", SDLK_F3), BT_CHANGE_GRID);
  level_options->add_pair_of_buttons(new Button(datadir+"gui/zoom_in.png", "Zoom In", SDLK_PLUS),
    BT_ZOOM_IN, new Button(datadir+"gui/zoom_out.png", "Zoom Out", SDLK_MINUS), BT_ZOOM_OUT);
}

TrackEditor::~TrackEditor()
  {
  unload_level();
  delete tiles_board;
  delete level_options;
  }

void TrackEditor::unload_level()
  {
  if(theme)
    delete theme;
  for(Objects::iterator i = objects.begin(); i != objects.end(); i++)
    delete *i;
  objects.clear();
  }

void TrackEditor::run()
  {
  SDL_ShowCursor(SDL_ENABLE);

  run_flag:

  done = false;
  while(!done)
    {
    events();
    action();
    draw();
    Screen::update();
    }

  if(save_level() == -1)
    goto run_flag;
  }

void TrackEditor::events()
  {
  while(SDL_PollEvent(&event))
    {
    // check for events in buttons
    if(tiles_board->event(event))
      {
      selected_object = tiles_board->selected_id();
      }
    else if(level_options->event(event))
      {
      switch(level_options->selected_id())
        {
        case BT_TEST:
          test_level();
          break;
        case BT_CHANGE_GRID:
          use_grid = !use_grid;  // TODO: add grid modes
          break;
        case BT_ZOOM_IN:
          zoom += 1.00;
          if(zoom > 3.0)
            zoom = 3.0;
          else
            {
            scroll *= 2;
            theme->scale(zoom);
            }
          break;
        case BT_ZOOM_OUT:
          zoom -= 1.00;
          if(zoom < 1.0)
            zoom = 1.0;
          else
            {
            scroll /= 2;
            theme->scale(zoom);
            }
        }
      level_options->set_unselected();
      continue;
      }
    else
      {
      switch(event.type)
        {
        case SDL_MOUSEMOTION:
          if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(SDL_BUTTON_RIGHT))
            {  // movement like in strategy games
            scroll.x += -1 * event.motion.xrel;
            scroll.y += -1 * event.motion.yrel;
            }
          hovered_object = get_object((int)((scroll.x+event.motion.x)*(1/zoom)),
                                      (int)((scroll.y+event.motion.y)*(1/zoom)));
          break;

        case SDL_MOUSEBUTTONDOWN:
          if(event.button.button == SDL_BUTTON_LEFT)
            {
            put_object(selected_object, (int)((scroll.x+event.motion.x)*(1/zoom)),
                                        (int)((scroll.y+event.motion.y)*(1/zoom)));
            }
          else if(event.button.button == SDL_BUTTON_MIDDLE)
            {
            selected_object = get_object_id((int)((scroll.x+event.motion.x)*(1/zoom)),
                                           (int)((scroll.y+event.motion.y)*(1/zoom)));
            tiles_board->set_selected(selected_object);
            }
          break;

        case SDL_KEYUP:   // key released
          if(event.key.keysym.sym == SDLK_ESCAPE)
            done = true;
          break;

        case SDL_KEYDOWN:   // key pressed
          switch(event.key.keysym.sym)
            {
            /* scrolling related events: */
            case SDLK_HOME:
              scroll.x = 0;
              break;
            case SDLK_LEFT:
              scroll.x -= 80;
              break;
            case SDLK_RIGHT:
              scroll.x += 80;
              break;
            case SDLK_UP:
              scroll.y -= 80;
              break;
            case SDLK_DOWN:
              scroll.y += 80;
              break;
            case SDLK_F1:
              show_help();
              break;
            default:
              break;
            }
          break;

        case SDL_QUIT:   // window closed
          done = true;
          break;

          default:
            break;
        }
      }
    }
}

void TrackEditor::action()
  {
  }

#define GRID_W ((int)(theme->max_object_size.x * zoom))
#define GRID_H ((int)(theme->max_object_size.y * zoom))

void TrackEditor::draw()
  {
  Screen::fill_rect(Rect(0,0,Screen::w,Screen::h), Color(150,150,150));

  // draw the track
  for(Objects::reverse_iterator i = objects.rbegin(); i != objects.rend(); i++)
    (*i)->draw(scroll - ((*i)->pos*(zoom-1)), true);

  if(use_grid)
    {
    for(int x = -scroll.x%GRID_W; x < Screen::w; x += GRID_W)
      Screen::fill_rect(Rect(x,0,1,Screen::h), Color(225,225,225));
    for(int y = -scroll.y%GRID_H; y < Screen::h; y += GRID_H)
      Screen::fill_rect(Rect(0,y,Screen::w,1), Color(225,225,225));
    }

  normal_font->draw_text("Track Editor", Color(255,255,255), Point(10, 5));
  small_font->draw_text(filename, Color(255,255,0), Point(Screen::w/2, 10), CENTER_ALLIGN);
  small_font->draw_text("F1 for help", Color(255,255,255), Point(5, Screen::h-15), LEFT_ALLIGN);

  if(!tiles_board->is_hover() && !tiles_board->is_hover())
    {
    // draw square around hovered object
    if(selected_object == -1 && hovered_object)
      Screen::draw_rect(Rect((int)(hovered_object->pos.x*zoom)-scroll.x,
                            (int)(hovered_object->pos.y*zoom)-scroll.y,
                            hovered_object->info->width(), hovered_object->info->height()),
                            Color(255,0,0));

    // draw selected object on cursor
    if(selected_object != -1)
      {
      ObjectInfo* obj = theme->get_object(selected_object);
      obj->draw_transparent(Point(event.motion.x-(obj->width()/2),
                                  event.motion.y-(obj->height()/2)), true);
      }
    else  // rubber
      tiles_board->get_button_img(0)->draw_transparent(Point(event.motion.x, event.motion.y));
    }

  tiles_board->draw();
  level_options->draw();
  }

Object* TrackEditor::get_object(int x, int y)
  {
  for(Objects::iterator i = objects.begin(); i != objects.end(); i++)
    if((*i)->in_region_of(Point(x, y)))
      return *i;
  return NULL;
  }

int TrackEditor::get_object_id(int x, int y)
  {
  Object* obj = get_object(x, y);
  if(obj == NULL)
    return -1;
  return theme->get_id(obj->info);
  }

void TrackEditor::put_object(int id, int x, int y)
  {
  if(id == -1)
    {
    hovered_object = NULL;
    remove_object(x, y);
    return;
    }

  ObjectInfo* obj = theme->get_object(id);

  x -= obj->size.x / 2;
  y -= obj->size.y / 2;

  if(use_grid)
    {  // adjust to grid
    int x_, y_;
    x_ = (x / GRID_W) * GRID_W;
    y_ = (y / GRID_H) * GRID_H;
    if(ABS((ABS((x_)) - x)) <= 5)
      x = x_;
    if(ABS((ABS((y_)) - y)) <= 5)
      y = y_;

    x_ = (((x+obj->size.x) / GRID_W)+1) * GRID_W;
    y_ = (((y+obj->size.y) / GRID_H)+1) * GRID_H;
    if(ABS((ABS((x_)) - x)) <= obj->size.x + 5)
      x = x_ - obj->size.x;
    if(ABS((ABS((y_)) - y)) <= obj->size.y + 5)
      y = y_ - obj->size.y;
    }

  if(obj->surface && !obj->surface->has_colorkey())  // if opaque, remove objects below
    remove_objects(Rect(x, y, obj->surface->w, obj->surface->h));

  objects.push_front(new Object(obj, x, y));
  level_changed = true;

  if(x < 0)
    move_objects(-x, 0);
  if(y < 0)
    move_objects(-y, 0);
  }

void TrackEditor::move_objects(int x, int y)
  {
std::cerr << "moving all objects " << x << "x" << y << std::endl;
  if(x == 0 && y == 0)
    return;
  for(Objects::iterator i = objects.begin(); i != objects.end(); i++)
{
//std::cerr << "old pos: " << (*i)->rect.x << " x " << (*i)->rect.y << std::endl;
    (*i)->pos += Point(x, y);
//std::cerr << "new pos: " << (*i)->rect.x << " x " << (*i)->rect.y << std::endl;
}
  scroll += Point(x, y);
  }

void TrackEditor::remove_object(const Objects::iterator& it)
  {
std::cerr << "removing object by getting iterator\n";
  Point p = (*it)->pos;

  level_changed = true;
  delete (*it);
  objects.erase(it);

  if(p.x <= 0 || p.y <= 0)
    {
    // if this is the first objects, move everyother to the right
    int min_x = INT_MAX, min_y = INT_MAX;
    for(Objects::iterator i = objects.begin(); i != objects.end(); i++)
      {
      min_x = std::min(min_x, (*i)->pos.x);
      min_y = std::min(min_y, (*i)->pos.y);
      if(min_x == 0 && min_y == 0)
        break;  // no need to check any further
      }
    move_objects(-min_x, -min_y);
    }
std::cerr << "object removed.\n";
  }

void TrackEditor::remove_object(int x, int y)
  {
  for(Objects::iterator i = objects.begin(); i != objects.end(); i++)
    if((*i)->in_region_of(Point(x, y)))
      {
      remove_object(i);
      break;
      }
  }

void TrackEditor::remove_objects(const Rect& rect)
  {
  bool object_removed = false;
  for(Objects::iterator i = objects.begin(); i != objects.end(); i++)
    if(Rect::is_inside((*i)->rect(), rect))
      {
      object_removed = true;
      remove_object(i);
      break;
      }
  /* FIXME: Hack because of my poor iterator skills. */
  if(object_removed)
    remove_objects(rect);
  }

void TrackEditor::load_level()
  {
  std::string file;
  if(file_exists(homedir + "tracks/" + filename))
    file = homedir + "tracks/" + filename;
  else
    file = datadir + "tracks/" + filename;
std::cerr << "loading level: " << file << std::endl;

  // load track here:
  INIIterator i;
  Object* object;
  for(i.open(file); !i.eof(); i.next())
    {
    if(i.group() == "GLOBALS")
      {
      for(; i.on_entry(); i.next())
        if(i.entry() == "theme")
          {
          if(theme)
            std::cerr << "Warning: a theme had already been loaded.\n";
          else
            theme = new ObjectsTheme(i.value(), zoom);
          }
      }
    else if(i.group() == "place")
      {
      if(!theme)
        theme = new ObjectsTheme("default");

      object = new Object();
      for(; i.on_entry(); i.next())
        {
        if(i.entry() == "object")
          object->set_id(theme, atoi(i.value().c_str()));
        else if(i.entry() == "x")
          object->set_x(atoi(i.value().c_str()));
        else if(i.entry() == "y")
          object->set_y(atoi(i.value().c_str()));
        }
      objects.push_front(object);
      }
    }
  }

int TrackEditor::save_level()
  {
  if(!level_changed)
    return true;

  {
    Menu ask_save;
    ask_save.add_entry("Confirm Saving", ENTRY_TITLE);
    ask_save.add_entry("Save", ENTRY_ENTRY, 1);
    ask_save.add_entry("Dont save", ENTRY_ENTRY, 0);
    ask_save.add_entry("Cancel", ENTRY_ENTRY, -1);
    int ret = ask_save.show(NULL);
    if(ret != 1)
      return ret;
  }

  level_changed = false;
std::cerr << "saving level: " << homedir + "tracks/" + filename << std::endl;

  INIWriter writer(homedir + "tracks/" + filename);
  writer.write_comment("Created by Microracers " VERSION " built-in track editor");
  writer.write_group("GLOBALS");
  writer.write_entry("theme", theme->name);
    {
    int track_width = 0, track_height = 0;
    for(Objects::iterator i = objects.begin(); i != objects.end(); i++)
      {
      track_width = std::max(track_width, (*i)->pos.x + (*i)->info->size.x);
      track_height = std::max(track_height, (*i)->pos.y + (*i)->info->size.y);
      }
    writer.write_entry("track-width", track_width);
    writer.write_entry("track-height", track_height);
    }

  for(Objects::reverse_iterator i = objects.rbegin(); i != objects.rend(); i++)
    {
    writer.write_group("place");
    writer.write_entry("object", theme->get_id((*i)->info));
    writer.write_entry("x", (*i)->pos.x);
    writer.write_entry("y", (*i)->pos.y);
    }
  return 1;
  }

void TrackEditor::test_level()
  {
  if(save_level() != 1)
    return;

  Race race;
  race.reset();
  race.add_vehicle(datadir + "vehicles/racingcar", true);
  race.setup_track(filename);
  race.start(RACE_TEST);
  }

void TrackEditor::show_help()
  {
  bool use_grid_t = use_grid;
  use_grid = false;

  char str[1024];
  const char *text1[] = {
    "Welcome to Microracer's built-in track editor!\n"
    "In here, you may create new or modify existing tracks. This process should\n"
    "be easy and, hopefully, pleasant.\n\n"
    "To place an objects, first select the one you want from the big board in the right.\n"
    "If there are too many objects, you may scroll the board with the mouse wheel.\n"
    "Keys shortcuts are also available, and you can check them by pressing the right button\n"
    "on the button you want.\n\n"
    "After selecting an object, place it wherever you want with the left button. Middle button\n"
    "on an object to copy it, and right button to scroll through the editor.\n\n"
    "Editing actions are available in another board in the bottom of the screen that contain\n"
    "the following buttonts: test track, change grid, zoomming.\n\n"
    "Saving is done automatically. Tracks are dumped over \".microracers/tracks\" in your home\n"
    "directory."
    };

  const char **text[] = { text1 };

  bool done;
  for(unsigned int i = 0; i < sizeof(text) / sizeof(text[0]); i++)
    {
    draw();

    normal_font->draw_text("- Track Editor's Help -", Color(0,0,255),
                   Point(Screen::w/2, 60), CENTER_ALLIGN);
    small_font->draw_text(*text[i], Color(255,255,255), Point(20, 120), LEFT_ALLIGN);

    sprintf(str, "Press any key to continue - Page %d/%d", i+1,
            (int)(sizeof(text) / sizeof(text[0])));
    small_font->draw_text(str, Color(255,255,0), Point(Screen::w/2, Screen::h-60), CENTER_ALLIGN);

    Screen::update();

    done = false;
    while(SDL_PollEvent(&event)) ;    // clear events
    while(!done)
      {
      while(SDL_PollEvent(&event))
        if(event.type == SDL_MOUSEBUTTONDOWN ||
           event.type == SDL_KEYDOWN || event.type == SDL_QUIT)
          done = true;
      SDL_Delay(50);
      }
    }

  use_grid = use_grid_t;
  }

/** GUI STUFF -- I kept those here since they are not used elsewhere
    but they might fit better in their own file. */

/** Button */

Button::Button(Surface* image, const std::string& info, SDLKey binding)
  : info(info), binding(binding), state(BT_NONE)
  {
  this->image = new Surface(image->get_sdl_surface());
  size = Point(image->w, image->h);
  }

Button::Button(const std::string& filename, const std::string& info, SDLKey binding)
  : info(info), binding(binding), state(BT_NONE)
  {
  image = new Surface(filename, Color(255,0,255));
  size = Point(image->w, image->h);
  }

Button::~Button()
  {
  if(image)
    delete image;
  }

void Button::resize(const Point& size)
  {
  this->size = size;
  image->stretch(size.x, size.y);
  }

void Button::draw(Point offset, bool selected)
  {
  if(selected)
    Screen::fill_rect(Rect(pos+offset, size), Color (200,240,220));
//  else
//    Screen::fill_rect(Rect(pos+offset, size), Color (200,200,220));

//  image->draw_part(pos, Rect(0,0,size.x,size.y));
  image->draw(pos + offset);

  if(state == BT_SHOW_INFO)
    {
    if(pos.x < 100 && pos.y > Screen::h - 20)
      offset += Point(size.x, - 10);
    else if(pos.x + offset.x < 100)
      offset += Point(size.x, 0);
    else 
      offset += Point(-30, -size.y/2);
    small_font->draw_text(info, Color(255,255,255), pos + offset, LEFT_ALLIGN);
    if(binding != 0)
      small_font->draw_text("(" + std::string(SDL_GetKeyName(binding)) + ")",
                      Color(255,255,255), pos + offset + Point(0,12), LEFT_ALLIGN);
    }
  }

bool Button::event(SDL_Event &event, int x_offset, int y_offset)
  {
  state = BT_NONE;

  switch(event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
      if(event.button.x > pos.x + x_offset && event.button.x < pos.x + x_offset + size.x &&
        event.button.y > pos.y + y_offset && event.button.y < pos.y + y_offset + size.y)
        {
        if(event.button.button == SDL_BUTTON_RIGHT)
          state = BT_SHOW_INFO;
        else if(event.button.button == SDL_BUTTON_LEFT)
          state = BT_SELECTED;
        }
      break;
    case SDL_KEYDOWN: // key pressed
      if(event.key.keysym.sym == binding)
        state = BT_SELECTED;
      break;
    default:
      break;
    }

  return state == BT_SELECTED;
  }

/** ButtonGroup */

#define TITLE_BAR 15

ButtonGroup::ButtonGroup(Point pos, Point buttons_size, Point buttons_layout)
  : pos(pos), buttons_size(buttons_size), buttons_layout(buttons_layout),
    buttons_pair_nb(0), scroll(0), button_selected(-1), moving(false), mouse_hover(false)
  { }

ButtonGroup::~ButtonGroup()
  {
  for(Buttons::iterator i = buttons.begin(); i != buttons.end(); ++i)
    delete *i;
  }

void ButtonGroup::add_button(Button* button, int id, bool select)
  {
  button->pos.x = ((buttons.size()-buttons_pair_nb) % (int)buttons_layout.x) * buttons_size.x;
  button->pos.y = ((int)((buttons.size()-buttons_pair_nb) / buttons_layout.x)) * buttons_size.y;
  button->resize(buttons_size);
  button->id = id;
  if(select)
    button_selected = id;

  buttons.push_back(button);
  }

void ButtonGroup::add_pair_of_buttons(Button* button1, int id1, Button* button2, int id2)
  {
  button1->pos.x = button2->pos.x = ((buttons.size()-buttons_pair_nb) % (int)buttons_layout.x)
                                  * buttons_size.x;
  button1->pos.y = button2->pos.y = ((int)((buttons.size()-buttons_pair_nb) / buttons_layout.x))
                                  * buttons_size.y;
  button1->size.x = button2->size.x = buttons_size.x;
  button1->size.y = button2->size.y = buttons_size.y / 2;
  button2->pos.y += buttons_size.y / 2;
  button1->id = id1;
  button2->id = id2;

  buttons_pair_nb++;
  buttons.push_back(button1);
  buttons.push_back(button2);
  }

void ButtonGroup::draw()
  {
  // semi-transparent background
  Screen::fill_rect(Rect(pos.x, pos.y, buttons_layout.x*buttons_size.x, buttons_layout.y*buttons_size.y),
                    Color (200,200,220, 128));
  // title bar:
  Screen::draw_horizontal_gradient(Rect(pos.x, pos.y-TITLE_BAR, buttons_layout.x*buttons_size.x, TITLE_BAR),
                                   Color(0,0,255), Color(255,255,255));

  for(Buttons::iterator i = buttons.begin(); i != buttons.end(); ++i)
    {
    if((*i)->pos.y < scroll*buttons_size.y ||
        (*i)->pos.y + (*i)->size.y > (scroll + buttons_layout.y) * buttons_size.y)
      continue;

    (*i)->draw(Point(pos.x, pos.y - buttons_size.y*scroll), (*i)->id == button_selected);
    }
  }

bool ButtonGroup::event(SDL_Event &event)
  {
  bool caught_event = false;

  switch(event.type)
    {
    case SDL_MOUSEMOTION:
      if(moving)
        {
        pos.x += event.motion.xrel;
        pos.y += event.motion.yrel;
        caught_event = true;
        }
      mouse_hover = Rect::test_collision(Point(event.button.x, event.button.y), Rect(pos.x, pos.y,
                buttons_size.x*buttons_layout.x, buttons_size.y*buttons_layout.y));
      break;
    case SDL_MOUSEBUTTONDOWN:
      if(event.button.button == SDL_BUTTON_LEFT &&
              event.button.y >= pos.y - TITLE_BAR && event.button.y < pos.y &&
              event.button.x >= pos.x && event.button.x <= pos.x + (buttons_layout.x*buttons_size.x))
        {
        moving = true;
        caught_event = true;
        }

      else if(!Rect::test_collision(Point(event.button.x, event.button.y), Rect(pos.x, pos.y,
                                    buttons_size.x*buttons_layout.x, buttons_size.y*buttons_layout.y)))
        return false;

      if(event.button.button == SDL_BUTTON_WHEELUP)
        {
        scroll--;
        if(scroll < 0)
          scroll = 0;
        caught_event = true;
        }
      else if(event.button.button == SDL_BUTTON_WHEELDOWN)
        {
        scroll++;
        if(scroll > (int)((buttons.size()-buttons_pair_nb)/buttons_layout.x) - (int)buttons_layout.y +
                ((int)(buttons.size()-buttons_pair_nb)%(int)buttons_layout.x != 0 ? 1 : 0))
          scroll = (int)((buttons.size()-buttons_pair_nb)/buttons_layout.x) - (int)buttons_layout.y +
                ((int)(buttons.size()-buttons_pair_nb)%(int)buttons_layout.x != 0 ? 1 : 0);
        caught_event = true;
        }
      break;
    case SDL_MOUSEBUTTONUP:
      if(moving)
        caught_event = true;
      moving = false;
      break;
    default:
      break;
    }

  if(caught_event)
    return true;

  for(Buttons::iterator i = buttons.begin(); i != buttons.end(); ++i)
    {
    if((*i)->pos.y < scroll*buttons_size.y ||
        (*i)->pos.y + (*i)->size.y > (scroll + buttons_layout.y) * buttons_size.y)
      continue;

    if((*i)->event(event, pos.x, pos.y - scroll*(int)buttons_size.y))
      {
      button_selected = (*i)->id;
      caught_event = true;
      break;
      }
    }

  return caught_event;
  }

Surface* ButtonGroup::get_button_img(int n)
  { return buttons[n]->image; }

int ButtonGroup::selected_id()
  { return button_selected; }
void ButtonGroup::set_unselected()
  { button_selected = -1; }
void ButtonGroup::set_selected(int id)
  { button_selected = id; }
