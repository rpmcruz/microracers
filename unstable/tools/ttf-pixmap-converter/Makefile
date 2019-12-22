CC = gcc
CFLAGS = -Wall `sdl-config --cflags`
LIBS  = `sdl-config --libs` -lSDL_ttf
SRC = main.c
OBJ = $(SRC:.c=.o)
TARGET = ttf-pixmap-converter

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LIBS) -o $(TARGET) $(OBJ)

clean:
	rm -f $(TARGET) $(OBJ) core*
