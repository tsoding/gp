PKGS=sdl2
CFLAGS=-Wall -Wold-style-definition -ggdb -std=c11 -pedantic `pkg-config --cflags sdl2`
LIBS=`pkg-config --libs sdl2`

gp: main.c style.h
	$(CC) $(CFLAGS) -o gp main.c $(LIBS)
