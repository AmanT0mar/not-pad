CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb `sdl2-config --cflags sdl2`
LIBS=`sdl2-config --libs sdl2` -lm -lSDL2_ttf
 np: main.c
	$(CC) $(CFLAGS) -o np main.c buffer.c $(LIBS) 
