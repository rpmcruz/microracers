
# WARNING:
#   - Please use the root directory Makefile, not this one. -

#DATA_PREFIX=$(DATA_PREFIX)

# CXX and CFLAGS are workarounds to support C files
CC = g++
CXXFLAGS = -Wall -g `sdl-config --cflags` -DDATA_PREFIX=\"$(DATA_PREFIX)\"
LIBS  = -lm `sdl-config --libs` -lSDL_image -lSDL_ttf
SRC = camera.cpp gameloop.cpp main.cpp \
      splash.cpp track.cpp vehicle.cpp  resources.cpp \
      lib/font.cpp lib/ini_parser.cpp lib/menu.cpp \
      lib/point.cpp lib/file_access.cpp lib/screen.cpp \
      lib/surface.cpp lib/timer.cpp \
      lib/primitives.cpp
OBJ = $(SRC:.cpp=.o)
HDR = camera.h gameloop.h \
      splash.h track.h vehicle.h  resources.h \
      lib/font.h lib/ini_parser.h lib/menu.h \
      lib/point.h lib/file_access.h lib/screen.h \
      lib/surface.h lib/timer.h \
      globals.h lib/color.h \
      lib/primitives.h
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
