/***************************************************************************
                    trackeditor.h  -  built'in track editor
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

#ifndef TRACKEDITOR_H
#define TRACKEDITOR_H

#include <SDL.h>
#include <string>
#include <list>
#include <vector>

#include "lib/point.h"

class ButtonGroup;
class Surface;
class ObjectsTheme;
class Object;
class Font;

enum {
  BT_TEST,
  BT_CHANGE_GRID,
  BT_ZOOM_IN, BT_ZOOM_OUT
  };

class TrackEditor
  {
  public:
    TrackEditor(const std::string& filename, const std::string& theme_name);
    ~TrackEditor();

    void run();

  private:
    void events();
    void action();
    void draw();

    void load_level();
    void unload_level();
    int save_level();
    void test_level();

    void show_help();

    ObjectsTheme* theme;
    typedef std::list <Object*> Objects;
    Objects objects;
    std::string filename, themename;

    Object* get_object(int x, int y);
    int get_object_id(int x, int y);
    void put_object(int id, int x, int y);
    void move_objects(int x, int y);
    void remove_object(int x, int y);
    void remove_object(const Objects::iterator& it);
    void remove_objects(const Rect& rect);

    bool done;
    bool use_grid;

    Point scroll;
    float zoom;

    ButtonGroup *tiles_board, *level_options;
    int selected_object;
    Object* hovered_object;

    bool level_changed;
  };

/** GUI stuff. */

struct Button
  {
  Button(const std::string& image, const std::string& info, SDLKey binding);
  Button(Surface* image, const std::string& info, SDLKey binding);
  ~Button();

  void draw(Point pos, bool selected);
  bool event(SDL_Event& event, int x_offset = 0, int y_offset = 0);

  void resize(const Point& size);

  Point pos, size;
  Surface* image;

  std::string info;
  SDLKey binding;

  int id;
  enum State {
    BT_NONE,
    BT_SELECTED,
    BT_SHOW_INFO
    };
  State state;
  };

class ButtonGroup
  {
  public:
    ButtonGroup(Point pos, Point size, Point buttons_layout);
    ~ButtonGroup();

    void draw();
    bool event(SDL_Event& event);

    void add_button(Button* button, int id, bool select = false);
    void add_pair_of_buttons(Button* button1, int id1, Button* button2, int id2);

    Surface* get_button_img(int n);

    int selected_id();
    void set_unselected();
    void set_selected(int id);

    bool is_hover()
      { return mouse_hover; }

  private:
    Point pos, buttons_size, buttons_layout;
    int buttons_pair_nb;
    int scroll;

    typedef std::vector <Button*> Buttons;
    Buttons buttons;

    int button_selected;

    bool moving, mouse_hover;
  };

#endif /*TRACKEDITOR_H*/
