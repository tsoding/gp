PKGS=sdl2
CFLAGS=-Wall -ggdb -std=c11 -pedantic `pkg-config --cflags sdl2`
LIBS=`pkg-config --libs sdl2`

gp: main.c
	$(CC) $(CFLAGS) -o gp main.c $(LIBS)
