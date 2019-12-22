/***************************************************************************
                      ini_parser.cpp  -  parser of INI-like files
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

#include <iostream>
#include <stdlib.h>

#include "ini_parser.h"

void INIIterator::open(const std::string& filename)
  {
  file_stream.open(filename.c_str());
  if(!file_stream)
    {
    std::cerr << "Warning: file '" << filename << "' could not be open.\n";
    state = END_OF_FILE_STATE;
    }
  else
    next();
  }

void INIIterator::next()
  {
  /* End of file found. */
  if(file_stream.eof())
    {
    state = END_OF_FILE_STATE;
    return;
    }
  /* Group found -- tell user that next next() will start reading it. */
  // group_str.empty() is just to see if the file has just been opened or not
  if(file_stream.peek() == '[' && (state != END_OF_GROUP_STATE && !group_str.empty()))
    {
    state = END_OF_GROUP_STATE;
    return;
    }

  char str[1024];
  file_stream.getline(str, 1024);

  /* Group found. */
  if(*str == '[')
    {
    int i;
    char* group;

    group = str+1;  // cut the first '['
    for(i = 1; str[i] != '\0' && str[i] != ']'; i++) ;  // cut the second ']'
    if(str[i] == '\0')
      {  // this ain't a well-formed group!
      std::cerr << "Warning: mal-formed INI group line: " << str << std::endl;
      next();
      return;
      }
    str[i] = '\0';

    group_str = group;

    next();  // run the function again to get an entry
    }
  /* Comment or no line. */
  else if(*str == '\0' || *str == '#')
    {
    next();
    }
  /* Space at start -- complain. */
  else if(*str == ' ')
    {
    std::cerr << "Warning: INI Parser doesn't like whitespaces that much."
                 "He says: There is no way I'll be parsing line: " << str << std::endl;
    next();
    }
  /* If file just opened, but no group found. */
  else if(group_str.empty())
    {
    std::cerr << "Warning: Found entry with no given group: " << str << std::endl;
    next();
    }
  /* Entry found. */
  else
    {
    /* Let's make a pointer to the entry and the group, and terminate
       the string where necessary. */
    char *entry, *value;
    int i;

    entry = str;
    for(i = 0; str[i] != '=' && str[i] != '\0'; i++) ;
    if(str[i] == '\0')
      {  // this ain't an entry!
      std::cerr << "Warning: mal-formed INI entry line: " << str << std::endl;
      next();
      return;
      }

    if(str[i-1] == ' ')  // tolerate a space before '='
      str[i-1] = '\0';
    else
      str[i] = '\0';

    i++;
    if(str[i] == ' ')
      i++;  // tolerates a space after '='
    value = str+i;

    for(; str[i] != '\0' && str[i] != ';'; i++) ;
    if(str[i] == ';')
      str[i] = '\0';

    state = ON_ENTRY_STATE;
    entry_str = entry;
    value_str = value;
    }
  }

INIParser::INIParser(const std::string& filename)
  {
  INIIterator i;
  Group temp_map;

  // iterate groups
  for(i.open(filename); !i.eof(); i.next())
    {
    // iterate entries
    for(; i.on_entry(); i.next())
      temp_map[i.entry()] = i.value();

    file[i.group()] = temp_map;
    temp_map.clear();
    }

  // This code prints the INI file: (Usefull for debugging.)
  #if 0
  std::cerr << "INI file goes here:\n";
  for(File::iterator i = file.begin(); i != file.end(); i++)
    {
    std::cerr << "[" << i->first << "]" << std::endl;
    for(Group::iterator j = i->second.begin(); j != i->second.end(); j++)
      std::cerr << j->first << " = " << j->second << ";" << std::endl;
    }
  std::cerr << "-------/-/---------\n";
  #endif
  }

/* TODO: In the future we could get a reference, rather than return the value.
  this would make it possible to ommit entries in the INI file.
  Or instead, have an argument to set the default value, if entry not found. */

const std::string& INIParser::get_string(const std::string& group, const std::string& entry, const std::string& default_)
{
File::iterator i = file.find(group);
if(i == file.end())
  return default_;  // group not found

Group::iterator j = i->second.find(entry);
if(j == i->second.end())
  return default_;  // entry not found
return j->second;
}

int INIParser::get_int(const std::string& group, const std::string& entry, int default_)
{
std::string ret = get_string(group, entry, "");
if(ret[0] == ' ')
  return default_;
return atoi(ret.c_str());
}

double INIParser::get_float(const std::string& group, const std::string& entry, float default_)
{
std::string ret = get_string(group, entry, "");
if(ret[0] == ' ')
  return default_;
return atof(ret.c_str());
}

bool INIParser::group_exists(const std::string& group)
{
if(file.find(group) == file.end())
  return false;
return true;
}

INIWriter::INIWriter(const std::string& filename)
  {
  file_stream.open(filename.c_str());
  if(!file_stream)
    std::cerr << "Warning: could not open file for writting: " << filename << std::endl;
  }

void INIWriter::write_group(const std::string& group)
  {
  if(!file_stream) return;
  file_stream << std::endl << "[" << group << "]" << std::endl;
  }

void INIWriter::write_entry(const std::string& entry, const std::string& value)
  {
  if(!file_stream) return;
  file_stream << entry << " = " << value << ";" << std::endl;
  }

void INIWriter::write_entry(const std::string& entry, int value)
  {
  char str[16];
  sprintf(str, "%d", value);
  write_entry(entry, str);
  }

void INIWriter::write_comment(const std::string& comment)
  {
  if(!file_stream) return;
  file_stream << "# " << comment << std::endl;
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
