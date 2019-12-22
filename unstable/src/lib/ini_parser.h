/***************************************************************************
                        ini_parser.h  -  parser of INI-like files
                             -------------------
    begin                : Sun Oct 30 2004
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

#ifndef INI_PARSER_H
#define INI_PARSER_H

#include <vector>
#include <map>
#include <string>
#include <fstream>

/* INI is the file syntax used by Windows and KDE. It makes sense for us
   to use it, since we don't need more complexity than that. */

/* This is an iterator that is used by INIParser, but can also be used
   from without for stuff like iterating lists.
   Should be destroyed as soon as not needed to free the file. */

class INIIterator
  {
  public:
    void open(const std::string& filename);

    bool on_entry()
      { return state == ON_ENTRY_STATE; }

    const std::string& group()
      { return group_str; }
    const std::string& entry()
      { return entry_str; }
    const std::string& value()
      { return value_str; }
    std::string group_str, entry_str, value_str;

    void next();
    bool eof()
      { return state == END_OF_FILE_STATE; }

  private:
    std::ifstream file_stream;

    /* The various iterating states (by order). */
    enum IteratingState {
      ON_ENTRY_STATE,
      END_OF_GROUP_STATE,
      END_OF_FILE_STATE
      };
    IteratingState state;
  };

/* Convinience class that loads the file into memory and then has
   calls to get the values of a given group and entry. Should be
   used for the basic stuff. */

class INIParser
  {
  public:
    INIParser(const std::string& filename);

    const std::string& get_string(const std::string& group, const std::string& entry,
                                  const std::string& default_);
    int get_int(const std::string& group, const std::string& entry,
                int default_);
    double get_float(const std::string& group, const std::string& entry,
                     float default_);

    /* Check if group exists. */
    bool group_exists(const std::string& group);

  private:
    typedef std::map <std::string, std::string> Group;
    typedef std::map <std::string, Group> File;

    File file;
  };

/* Convinience class to write a INI file in an easy manner.
   Should be destroyed as soon as not needed to free the file. */

class INIWriter
  {
  public:
    INIWriter(const std::string& filename);

    void write_group(const std::string& group);
    void write_entry(const std::string& entry, const std::string& value);
    void write_entry(const std::string& entry, int value);
    void write_comment(const std::string& comment);

  private:
    std::ofstream file_stream;
  };

#endif
