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
    fprintf(stream, "Usage: ./gp_trainer [OPTIONS]\n");
    fprintf(stream, "    -g <NUMBER>    Amount of generations (default: 69)\n");
    fprintf(stream, "    -o <FILEPATH>  Output file path (default: output.bin)\n");
    fprintf(stream, "    -gp            Enable epic gnuplot reporting (requires gnuplot, duh)\n");
    fprintf(stream, "    -h             Print this help\n");
}

int main(int argc, char *argv[])
{
    shift(&argc, &argv);        // skip program name

    // TODO(#4): pause/resume mechanisms for gp_trainer
    // - Stop and dump current state by ^C
    // - Continue training from a provided dumped state
    // - Maybe log each generations during the training

    int generations_count = 69;
    const char *output_filepath = "output.bin";
    int gnuplot = 0;

    while (argc > 0) {
        const char *flag = shift(&argc, &argv);

        if (strcmp(flag, "-g") == 0) {
            if (argc == 0) {
                usage(stderr);
                fprintf(stderr, "ERROR: no value provided for flag `%s`\n", flag);
                exit(1);
            }

            const char *value = shift(&argc, &argv);
            generations_count = atoi(value);
        } else if (strcmp(flag, "-o") == 0) {
            if (argc == 0) {
                usage(stderr);
                fprintf(stderr, "ERROR: no value provided for flag `%s`\n", flag);
                exit(1);
            }

            const char *value = shift(&argc, &argv);
            output_filepath = value;
        } else if (strcmp(flag, "-gp") == 0) {
            gnuplot = 1;
        } else if (strcmp(flag, "-h") == 0) {
            usage(stdout);
            exit(0);
        } else {
            usage(stderr);
            fprintf(stderr, "ERROR: unknown flag `%s`\n", flag);
            exit(1);
        }
    }

    FILE* log = NULL;

    if (gnuplot) {
        log = fopen("log.dat", "w");
        log_header(log);
    }

    int current = 0;
    init_game(&games[current]);
    for (int i = 0; i < generations_count; ++i) {
        printf("Generation %d... ", i);
        fflush(stdout);
        int lifetime = 0;
        clock_t begin = clock();
        while (!is_everyone_dead(&games[current])) {
            step_game(&games[current]);
            lifetime++;
        }
        printf("%fs (%d max lifetime)\n", (float)(clock() - begin) / (float) CLOCKS_PER_SEC, lifetime);
        fflush(stdout);

        if (gnuplot) {
            log_generation(log, i, &games[current]);
            /* we need to fflush the log file in order to use the live update */
            fflush(log);
            log_live_update();
        }

        int next = 1 - current;
        make_next_generation(&games[current], &games[next]);
        current = next;
    }

    if (gnuplot) {
        log_close_pipe();
        fclose(log);
    }

    dump_game(output_filepath, &games[current]);

    return 0;
}
