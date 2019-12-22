/***************************************************************************
                          file_access.cpp  -  handles path, etc
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

#include <iostream>

#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>
#include <dirent.h>

#include "file_access.h"

#ifndef DATA_PREFIX
  #define DATA_PREFIX "./data/"
    #warning DATA_PREFIX should have been declared in Makefile.
    #warning If installed on Linux, the game will not work properly!
#endif

std::string datadir; 

/* Check if directory exists */
bool dir_exists(const std::string& dirname)
{
struct stat filestat;
if (stat(dirname.c_str(), &filestat) == -1)
  return false;
if(S_ISDIR(filestat.st_mode))
  return true;
return false;
}

/* Does the given file exist and is it accessible? */
int file_exists(const std::string& filename)
{
  struct stat filestat;
  if (stat(filename.c_str(), &filestat) == -1)
    {
      return false;
    }
  else
    {
      if(S_ISREG(filestat.st_mode))
        return true;
      else
        return false;
    }
}

bool find_datadir(const std::string& invocation)
{
// if data dir has been already feed, don't check for it.
// It was probably given from one of the arguments or something.
if(!datadir.empty())
  {
  datadir += "/";
  if(!dir_exists(datadir))
    std::cout << "Warning: the data dir you feed was not found. Ignoring...\n";
  else
    return true;
  }

datadir = invocation;
std::string::size_type i = datadir.find_last_of('/');
if(i == std::string::npos)
  i = 0;
else
  i++;
datadir.erase(i, datadir.size()-i);

datadir += "data/";

if(!dir_exists(datadir) && DATA_PREFIX)
  {
  datadir = DATA_PREFIX;
  datadir += "/";

  if(!dir_exists(datadir))
    return false;
  }
return true;
}

/* Get all names of sub-directories in a certain directory. */
/* Returns the sub-directories found. */
/* Note: The user has to free the allocated space. */
std::set<std::string> subdirs(const std::string& path)
{
DIR *dirStructP;
struct dirent *direntp;

std::set <std::string> sdirs;

if((dirStructP = opendir(path.c_str())) != NULL)
  {
  while((direntp = readdir(dirStructP)) != NULL)
    {
    std::string filename;
    struct stat buf;

    filename = path + "/" + direntp->d_name;

    if(std::string(direntp->d_name) == "." ||
       std::string(direntp->d_name) == ".." ||
       std::string(direntp->d_name) == "CVS")
      continue;  // ignore . and .. directories

    if (stat(filename.c_str(), &buf) == 0 && S_ISDIR(buf.st_mode))
      {
      sdirs.insert(direntp->d_name);
      }
    }
  closedir(dirStructP);
  }

return sdirs;
}
