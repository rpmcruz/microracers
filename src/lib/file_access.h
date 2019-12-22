/***************************************************************************
                          file_access.h  -  handles path, etc
                             -------------------
    begin                : Mon Nov 01 2004
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

#include <set>
#include <string>

#ifndef FILE_ACCESS_H
#define FILE_ACCESS_H

extern std::string datadir; 

/* Guesses the data directory.
   DATA_PREFIX should be specified in Makefile to be used to
   get the directory, in case it is installed in the system. */

bool find_datadir(const std::string& invocation);

// get all directories from a folder
std::set<std::string> subdirs(const std::string& path);

#endif
