PKGS=sdl2 SDL2_gfx
CFLAGS=-Wall -Wold-style-definition -ggdb -std=c11 -pedantic `pkg-config --cflags $(PKGS)`
LIBS=`pkg-config --libs $(PKGS)` -lm

gp: $(wildcard src/*)
	$(CC) $(CFLAGS) -o gp src/gp.c $(LIBS)

