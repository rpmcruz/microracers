CC = gcc
CFLAGS = -Wall `sdl-config --cflags`
LIBS  = `sdl-config --libs`
SRC = main.c
OBJ = $(SRC:.c=.o)
TARGET = grass-maker

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LIBS) -o $(TARGET) $(OBJ)

clean:
	rm -f $(TARGET) $(OBJ) core*
