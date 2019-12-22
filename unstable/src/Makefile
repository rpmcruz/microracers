
# WARNING:
#   - Please use the root directory Makefile, not this one. -

#DATA_PREFIX=$(DATA_PREFIX)

CC = g++
CXXFLAGS = -Wall -g `sdl-config --cflags` -DDATA_PREFIX=\"$(DATA_PREFIX)\"
LIBS  = `sdl-config --libs` -lSDL_image
SRC = track.cpp objects.cpp vehicle.cpp resources.cpp \
      gameloop.cpp trackeditor.cpp main.cpp camera.cpp splash.cpp  \
      lib/font.cpp lib/ini_parser.cpp lib/menu.cpp \
      lib/surface.cpp lib/screen.cpp lib/primitives.cpp  \
      lib/file_access.cpp lib/timer.cpp \
      lib/sin_table.cpp lib/point.cpp lib/bitmask.cpp
OBJ = $(SRC:.cpp=.o)
HDR = track.h objects.h vehicle.h resources.h \
      gameloop.h trackeditor.h camera.h splash.h  \
      lib/font.h lib/ini_parser.h lib/menu.h \
      lib/surface.h lib/screen.h lib/primitives.h \
      lib/file_access.h lib/timer.h \
      lib/sin_table.h lib/point.h \
      globals.h lib/color.h lib/bitmask.h
TARGET = ../microracers

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o $(TARGET)

# this hack is to force a major compiling each time a header file
# is modified.
# FIXME: only the necessary code should be compiled, not all.
$(OBJ): $(HDR)

clean:
	rm -f $(TARGET) $(OBJ) core*
