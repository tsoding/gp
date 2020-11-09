#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include "./gp_game.c"
#include "./gp_visual.c"

Game game = {0};

const char *shift(int *argc, char ***argv)
{
    assert(*argc > 0);
    const char *result = **argv;
    *argc -= 1;
    *argv += 1;
    return result;
}

void usage(FILE *stream)
{
    printf("Usage: ./gp_inspector <input.bin>");
}

int main(int argc, char *argv[])
{
    shift(&argc, &argv);        // skip program name

    if (argc == 0) {
        usage(stderr);
        fprintf(stderr, "ERROR: no input file is provided");
        exit(1);
    }
    const char *input_filepath = shift(&argc, &argv);

    load_game(input_filepath, &game);

    static_assert(AGENTS_COUNT > 0, "We need to have at least one agent to print anything");
    print_chromo(stdout, &game.agents[0].chromo);

    return 0;
}
