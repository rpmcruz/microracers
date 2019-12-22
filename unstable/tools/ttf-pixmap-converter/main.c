#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

#define DIV_EXCESS(a,b) ((a%b==0) ? (a/b) : ((a/b)+1))
#define PERCENTAGE(a,b) (a*(b/100.0))

TTF_Font *ttf_font, *ttf_font_smallcaps;
SDL_Surface* bitmap_font;

void print_char(char glyph, int x, int y, int w, int h)
  {
  if(glyph == ' ')
    return;

  TTF_Font* font;
  if(ttf_font_smallcaps && glyph >= 'a' && glyph <= 'z')
    font = ttf_font_smallcaps;
  else
    font = ttf_font;

  SDL_Color color = { 0, 0, 0 };
  SDL_Surface* temp = TTF_RenderGlyph_Solid(font, glyph, color);
  if(!temp)
    {
    fprintf(stderr, "Warning: Couldn't render character %c out of the TTF.\nPossible reason: %s\n",
            glyph, TTF_GetError());
    return;
    }

  int min_x, max_x, min_y, max_y, advance;
  TTF_GlyphMetrics(font, glyph, &min_x, &max_x, &min_y, &max_y, &advance);

//  int font_ascent  = TTF_FontAscent(ttf_font);
  int font_descent = TTF_FontDescent(font);

//printf("glyph: %c - min_x: %2d - max_x: %2d - min_y: %2d - max_y: %2d - advance: %2d\n", glyph, min_x, max_x, min_y, max_y, advance);

  SDL_Rect dst = { x, y + h-max_y+font_descent-1, 0, 0 };
  SDL_BlitSurface(temp, NULL, bitmap_font, &dst);

  SDL_FreeSurface(temp);

  if(min_x+max_x < w)
    {
    dst.x += max_x; dst.y = y;
    dst.w = 1; dst.h = h;
    SDL_FillRect(bitmap_font, &dst, SDL_MapRGB(bitmap_font->format, 255,255,255));
    }
  }

void print_help(char* invocation)
  {
  printf("TTF2Pixmap-Converter usage:\n\n"
         "%s TTF-font-filename [OPTIONS]\n\n"
         "Options:\n"
         "\t--output filename\tThe name of the output'ed BMP file.\n"
         "\t--coloums-nb NUMBER\tThe number of coloums you want for the bitmap output table.\n"
         "\t--glyph-size NUMBER\tThe wanted size for each character.\n"
         "\t--text STRING\tThe characters you want to be rendered.\n"
         "\t--simulate-smallcaps\tFor fonts that display small caps as big caps, try to simulate "
         "small caps.\n"
         "\t--help\tPrints this information.\n"
         "\n", invocation);
  }

int main(int argc, char *argv[])
  {
  char *ttf_filename, *bmp_filename = "output.bmp";
  int glyph_size = 20, coloums_nb = 16;
  char* text = " !\"#$%&'()*+,-./"
               "0123456789:;<=>?"
               "@ABCDEFGHIJKLMNO"
               "PQRSTUVWXYZ[\\]^_"
               "`abcdefghijklmno"
               "pqrstuvwxyz{|}~ ";
  bool simulate_smallcaps = false;
  int text_len;

  if(argc < 2 || !strcmp(argv[1], "--help"))
    {
    printf("Not enough arguments given. You must at least provide a TTF font.\n");
    print_help(argv[0]);
    return 1;
    }
  if(!strcmp(argv[1], "--help"))
    {
    print_help(argv[0]);
    return 1;
    }

  ttf_filename = argv[1];
  // parse the arguments
    {
    int i;
    for(i = 2; i < argc; i++)
      {
      if(!strcmp(argv[i], "--output"))
        bmp_filename = argv[++i];
      else if(!strcmp(argv[i], "--coloums-nb"))
        coloums_nb = atoi(argv[++i]);
      else if(!strcmp(argv[i], "--glyph-size") ||
              !strcmp(argv[i], "--size"))
        glyph_size = atoi(argv[++i]);
      else if(!strcmp(argv[i], "--text"))
        text = argv[++i];
      else if(!strcmp(argv[i], "--simulate-smallcaps"))
        simulate_smallcaps = true;
      else if(!strcmp(argv[i], "--help"))
        {
        print_help(argv[0]);
        return 1;
        }
      else
        {
        printf("I don't understand the argument \"%s\".\nBailing out.\n", argv[i]);
        return 1;
        }
      }
    }

  // calculate size of text
  for(text_len = 0; text[text_len] != '\0'; text_len++) ;

  /* initializations. */
  if(TTF_Init() < 0)
    {
    fprintf(stderr, "Error: Couldn't initialize TTF: %s.\n", TTF_GetError());
    return 1;
    }

  int ret = 0;

  /* loading. */
  ttf_font = ttf_font_smallcaps = NULL;
  bitmap_font = NULL;

  ttf_font = TTF_OpenFont(ttf_filename, glyph_size);
  if(ttf_font == NULL)
    {
    fprintf(stderr, "Error: Couldn't load font: %s.\nPossible reason: %s.\n",
            ttf_filename, TTF_GetError());
    ret = 1;
    goto cleanups;
    }
  if(simulate_smallcaps)
    ttf_font_smallcaps = TTF_OpenFont(ttf_filename, PERCENTAGE(glyph_size,80));

  bitmap_font = SDL_CreateRGBSurface(SDL_SWSURFACE,
             coloums_nb*glyph_size, DIV_EXCESS(text_len,coloums_nb)*glyph_size,
             24, 0xff, 0xff00, 0xff0000, 0);
  if(!bitmap_font)
    {
    fprintf(stderr, "Error: Couldn't allocate a surface to output the font.\nPossible reason: %s.\n",
            SDL_GetError());
    ret = 1;
    goto cleanups;
    }

  SDL_FillRect(bitmap_font, NULL, SDL_MapRGB(bitmap_font->format, 255, 0, 255));

  /* actual work goes now. */
  int i;
  for(i = 0; i < text_len; i++)
    {
    int x = i % coloums_nb * glyph_size;
    int y = i / coloums_nb * glyph_size;
//printf("printing char %c at %d,%d\n", text[i], x, y);
    print_char(text[i], x, y, glyph_size, glyph_size);
    }

  SDL_SaveBMP(bitmap_font, bmp_filename);

  /* cleanups. */
  cleanups:

  if(bitmap_font)
    SDL_FreeSurface(bitmap_font);
  if(ttf_font)
    TTF_CloseFont(ttf_font);
  if(ttf_font_smallcaps)
    TTF_CloseFont(ttf_font_smallcaps);

  TTF_Quit();

  return ret;
  }
