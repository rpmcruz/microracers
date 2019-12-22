/***************************************************************************
                          ini_parser.h  -  to be included in all cpp files
                             -------------------
    begin                : ¦un Oct 30 2004
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

#ifndef INI_PARSER_H
#define INI_PARSER_H

#include <vector>
#include <map>
#include <string>

/* This is a INI-like (windows, kde, etc) files parser.
   It is kinda hacky and it is space sensative, but it works. :) */

class INIParser
{
public:
  INIParser(const std::string& filename);
  ~INIParser();

  void parse_file(const std::string& filename);

  const std::string& get_string(const std::string& group_, const std::string& entry_);
  int get_int(const std::string& group_, const std::string& entry_);
  double get_float(const std::string& group_, const std::string& entry_);

  /* Check if group exists. */
  bool group_exist(const std::string& group_);

private:
  typedef std::map <std::string, std::string> Entry;
  typedef std::map <std::string, Entry > Group;

  Group group;
};

#endif
