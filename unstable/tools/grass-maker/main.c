/***************************************************************************
                          main.c  -  Outputs a pixelized bitmap
                             -------------------
    begin                : Sat Nov 05 2005
    copyright            : (C) 2005 by Ricardo Cruz
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

/**
  Description: Useful to create textures for grass, road, sand and alike that
  you want to have an old-school drawing look.
  It is a generator of pixelized images. You can specify the red, green and
  blue range and also lock them with each other.

  The name "Grass Maker" was because it was originally intended for grass.

  As an example, to create sand, use something like:
  grass-maker sand.bmp 300 300 2 --red-range 200 255 --green-range 200 255 --same-color
  **/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <SDL.h>

#define GET_RANDOM(min, max) \
  (min == max) ? min : \
  (rand() % (max-min)) + min

#define TOTAL_BASE_ARGUMENTS 5

void print_help(char* invocation)
  {
  printf("Grass Maker usage:\n\n"
         "%s OUTPUT_FILENAME WIDTH HEIGHT PIXEL_SIZE [OPTIONS]\n\n"
         "Options may be:\n"
         "\t--same-color\tOnly one random number shall be calculated for all.\n"
         "\t--red-range MIN MAX\tThe range for the red component.\n"
         "\t--green-range MIN MAX\tThe range for the green component.\n"
         "\t--blue-range MIN MAX\tThe range for the blue component.\n"
         "\n", invocation);
  }

typedef enum { RED, GREEN, BLUE } Colors;

int main(int argc, char *argv[])
  {
  SDL_Surface* surface;
  char *bmp_filename = "output.bmp";
  int pixel_size;
  int width, height;
  int min_color[3], max_color[3];
  bool same_color = false;

  if(argc < TOTAL_BASE_ARGUMENTS)
    {
    print_help(argv[0]);
    return 1;
    }

  // parse the arguments
  bmp_filename = argv[1];
  width = atoi(argv[2]);
  height = atoi(argv[3]);
  pixel_size = atoi(argv[4]);

    {
    int i;

    // reset all to 0
    for(i = 0; i < 3; i++)
      {
      min_color[i] = 0;
      max_color[i] = 0;
      }

    for(i = TOTAL_BASE_ARGUMENTS; i < argc; i++)
      {
      if(!strcmp(argv[i], "--same-color"))
        same_color = true;
      else if(!strcmp(argv[i], "--red-range"))
        {
        min_color[RED] = atoi(argv[++i]);
        max_color[RED] = atoi(argv[++i]);
        }
      else if(!strcmp(argv[i], "--green-range"))
        {
        min_color[GREEN] = atoi(argv[++i]);
        max_color[GREEN] = atoi(argv[++i]);
        }
      else if(!strcmp(argv[i], "--blue-range"))
        {
        min_color[BLUE] = atoi(argv[++i]);
        max_color[BLUE] = atoi(argv[++i]);
        }
      else
        {
        printf("I don't understand the argument \"%s\".\nBailing out.\n", argv[i]);
        return 1;
        }
      }
    }
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
    printf("Error: Couldn't initialize SDL: %s.\n", SDL_GetError());
    return 1;
    }

  int ret = 0;

  /* loading. */
  surface =  SDL_CreateRGBSurface(SDL_SWSURFACE,
             width, height, 24, 0xff, 0xff00, 0xff0000, 0);
  if(!surface)
    {
    printf("Error: Couldn't allocate a surface to output the font.\n"
           "Possible reason: %s.\n", SDL_GetError());
    ret = 1;
    goto cleanups;
    }

  // feed random number generator
  srand(time(NULL));

//  SDL_FillRect(bitmap_font, NULL, SDL_MapRGB(bitmap_font->format, 0, 255, 0));

  /* actual work goes now. */
  int x,y,i,j;
  SDL_Rect rect;
  int color[3];
  Uint32 pixel;
  for(y = 0; y < surface->h; y += pixel_size)
    for(x = 0; x < surface->w; x += pixel_size)
      {
      rect.x = x; rect.y = y;
      rect.w = rect.h = pixel_size;
      for(i = 0; i < 3; i++)
        {
        if(same_color)
          for(j = 0; j < i; j++)
            if(min_color[j] == min_color[i] && max_color[j] == max_color[i])
              {
              color[i] = color[j];
              break;
              }
        if(!same_color || j == i)
          color[i] = GET_RANDOM(min_color[i], max_color[i]);
        }

      pixel = SDL_MapRGB(surface->format, color[RED], color[GREEN], color[BLUE]);

      SDL_FillRect(surface, &rect, pixel);
      }

  SDL_SaveBMP(surface, bmp_filename);

  /* cleanups. */
  cleanups:

  if(surface)
    SDL_FreeSurface(surface);
  SDL_Quit();

  return ret;
  }
