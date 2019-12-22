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

#include <iostream>
#include <stdlib.h>

#include "ini_parser.h"

INIParser::INIParser(const std::string& filename)
{
parse_file(filename);
}

INIParser::~INIParser()
{
}

void INIParser::parse_file(const std::string& filename)
{
FILE *file = fopen(filename.c_str(), "r");
if (file == NULL)
	{
	std::cerr << "Error: File " << filename << " couldn't be open.\n";
	return;
	}

char buffer[1024];
char *ptr;

std::string cur_group;

std::map <std::string, std::string> temp_map ;

while ((ptr = fgets(buffer, sizeof(buffer), file)) != NULL)
	{
	if (ptr[0] == '#')	// comment
		continue;

	if (ptr[0] == '[')	// group symbol: [
		{
    if(!cur_group.empty())
      {
      cur_group.erase(cur_group.find("\n"), 1);
      group.insert(std::make_pair(cur_group, temp_map));
      temp_map.clear();
      }

    cur_group = buffer;
//    group.insert(std::make_pair(cur_group, entry));
		}

	else
		{
    std::string value = buffer;
    std::string entry;

    std::string::size_type comma_pos = value.find(";");
    if(comma_pos == std::string::npos)
      continue;
    value.erase(comma_pos, value.size() - comma_pos);  // ;, spaces and break lines

    std::string::size_type equal_pos = value.find("=");
    if(equal_pos == std::string::npos)
      continue;

    entry = value;
    entry.erase(equal_pos+1, value.size() - equal_pos+1);
    value.erase(0, equal_pos+1);

    temp_map[entry] = value;
    }
	}

fclose(file);

if(!cur_group.empty())
  {
  cur_group.erase(cur_group.find("\n"), 1);
  group[cur_group] = temp_map;
  temp_map.clear();
  }

// these code prints the INI file. Usefull for debugging.
#if 0
std::cerr << "INI file goes here:\n";
for(Group::iterator g_it = group.begin(); g_it != group.end(); g_it++)
  {
  std::cerr << g_it->first << std::endl;

  for(Entry::iterator e_it = g_it->second.begin();
      e_it != g_it->second.end(); e_it++)
    std::cerr << e_it->first << e_it->second << std::endl;
  }
#endif
}

/* TODO: In the future we could get a reference, rather than return the value.
  this would make it possible to ommit entries in the INI file.
  Or instead, have an argument to set the default value, if entry not found. */

const std::string& INIParser::get_string(const std::string& group_, const std::string& entry_)
{
Group::iterator g_it = group.find(group_);
if(g_it == group.end())
  std::cerr << "Warning: group not found: " << group_ << std::endl;

Entry::iterator e_it = g_it->second.find(entry_);
if(e_it == g_it->second.end())
  {
  std::cerr << "Warning: entry not found: " << entry_ << std::endl;
  }
return e_it->second;
}

int INIParser::get_int(const std::string& group_, const std::string& entry_)
{
return atoi(get_string(group_, entry_).c_str());
}

double INIParser::get_float(const std::string& group_, const std::string& entry_)
{
return atof(get_string(group_, entry_).c_str());
}

bool INIParser::group_exist(const std::string& group_)
{
if(group.find(group_) == group.end())
  return false;
return true;
}

#if 0
/* Old code - Gets a single specific entry. */

char* Base::filemanager(const char *path, const char *group, const char *entry)
{
FILE *file = fopen(path, "r");
if (file == NULL)
	{
	std::cerr << "File: " << path << " couldn't be open\n";
	return NULL;
	}

char buffer[1024];
static char entryvalue[1024];
char *ptr;
char *entryptr;
bool groupFound = false;
bool entryFound = false;

while ((ptr = fgets(buffer, sizeof(buffer), file)) != NULL)
	{
	if (ptr[0] == '#')	// comment
		continue;

	if((groupFound == true) && ((entryptr = strstr(ptr, entry)) != NULL))
		{
		entryptr += strlen(entry);

		char *semicolonptr;		// will point to ;
		strcpy(entryvalue, entryptr);

		if ((semicolonptr = strchr(entryvalue, ';')) == NULL)
			std::cerr << "ERROR: ; is missing in line: " << ptr << std::endl;
		else
			*semicolonptr = '\0';
		entryFound = true;

		break;
		}
	if (ptr[0] == '[')	// group symbol: [
		{
		groupFound = false;
		if (strstr(ptr, group) != NULL)
			groupFound = true;
		}
	}
fclose(file);
if (entryFound == false)
	{
	std::cerr << "ERROR: the " << entry << " entry was not found on " << group << " group in the file " << path << " ." << std::endl;
	return NULL;
	}
return entryvalue;

// example:
//filemanager("/home/rick2/.kde/share/config/kppprc", "[Account1]", "Password=");
}
#endif
