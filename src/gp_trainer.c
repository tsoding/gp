#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include "./gp_game.c"
#include "./gp_visual.c"
#include "./gp_logging.c"

#define MAX_GENERATIONS 10

Game games[2] = {0};

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
    fprintf(stream, "Usage: ./gp_trainer <generations-count> <output.bin>\n");
}

int main(int argc, char *argv[])
{
    shift(&argc, &argv);        // skip program name

    // TODO(#4): pause/resume mechanisms for gp_trainer
    // - Stop and dump current state by ^C
    // - Continue training from a provided dumped state
    // - Maybe log each generations during the training

    if (argc == 0) {
        usage(stderr);
        fprintf(stderr, "ERROR: number of generations is not provided\n");
        exit(1);
    }
    int generations_count = atoi(shift(&argc, &argv));

    if (argc == 0) {
        usage(stderr);
        fprintf(stderr, "ERROR: output filepath is not provided\n");
        exit(1);
    }
    const char *output_filepath = shift(&argc, &argv);

    FILE* log = fopen("log.dat", "w");
    log_header(log);

    int current = 0;
    init_game(&games[current]);
    for (int i = 0; i < generations_count; ++i) {
        printf("Generation %d... ", i);
        fflush(stdout);

        clock_t begin = clock();
        while (!is_everyone_dead(&games[current])) {
            step_game(&games[current]);
        }
        printf("%fs\n", (float)(clock() - begin) / (float) CLOCKS_PER_SEC);
        fflush(stdout);

        log_generation(log, i, &games[current]);
        /* we need to fflush the log file in order to use the live update */
        fflush(log);

        int next = 1 - current;
        make_next_generation(&games[current], &games[next]);
        current = next;
    }

    log_close_pipe();
    fclose(log);

    dump_game(output_filepath, &games[current]);

    return 0;
}
