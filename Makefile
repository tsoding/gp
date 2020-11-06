PKGS=sdl2 SDL2_gfx
CFLAGS=-Wall -Wold-style-definition -ggdb -std=c11 -pedantic `pkg-config --cflags $(PKGS)`
LIBS=`pkg-config --libs $(PKGS)` -lm

gp: main.c style.h
	$(CC) $(CFLAGS) -o gp main.c $(LIBS)

