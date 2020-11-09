PKGS=sdl2 SDL2_gfx
CFLAGS=-Wall -Wold-style-definition -ggdb -std=c11 -pedantic `pkg-config --cflags $(PKGS)`
LIBS=`pkg-config --libs $(PKGS)` -lm
COMMON_SRC=src/gp_game.h src/gp_game.c src/gp_visual.h src/gp_visual.c

all: gp_trainer gp_simulator gp_inspector

gp_trainer: src/gp_trainer.c $(COMMON_SRC)
	$(CC) $(CFLAGS) -o gp_trainer src/gp_trainer.c $(LIBS)

gp_simulator: src/gp_simulator.c $(COMMON_SRC)
	$(CC) $(CFLAGS) -o gp_simulator src/gp_simulator.c $(LIBS)

gp_inspector: src/gp_inspector.c $(COMMON_SRC)
	$(CC) $(CFLAGS) -o gp_inspector src/gp_inspector.c $(LIBS)
