#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include "./gp_game.c"
#include "./gp_visual.c"

Game games[2] = {0};

void usage(FILE *stream)
{
    fprintf(stream, "./gp_simulator [input.bin]\n");
}

const char *shift(int *argc, char ***argv)
{
    assert(*argc > 0);
    const char *result = **argv;
    *argc -= 1;
    *argv += 1;
    return result;
}

int main(int argc, char *argv[])
{
    int current = 0;
    srand(time(0));

    shift(&argc, &argv);        // skip program name

    if (argc == 0) {
        printf("WARNING: No input file is provided. Generating a new random state...\n");
        init_game(&games[current]);
    } else {
        const char *input_filepath = shift(&argc, &argv);
        printf("Loading the state from %s...\n", input_filepath);
        load_game(input_filepath, &games[current]);
    }

    scc(SDL_Init(SDL_INIT_VIDEO));

    SDL_Window *const window = scp(SDL_CreateWindow(
        "Hunger Games?",
        0, 0,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_RESIZABLE));

    SDL_Renderer *const renderer = scp(SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED));

    scc(SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT));

    int quit = 0;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
                quit = 1;
            } break;

            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                case SDLK_SPACE: {
                    step_game(&games[current]);
                } break;

                case SDLK_r: {
                    init_game(&games[current]);
                } break;

                case SDLK_n: {
                    int next = 1 - current;
                    make_next_generation(&games[current], &games[next]);
                    current = next;
                } break;
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                Coord pos;
                pos.x = (int) floorf(event.button.x / CELL_WIDTH);
                pos.y = (int) floorf(event.button.y / CELL_HEIGHT);
                Agent *agent = agent_at(&games[current], pos);

                if (agent) {
                    print_agent(stdout, agent);
                }
            } break;
            }
        }

        SDL_SetRenderDrawColor(renderer, HEX_COLOR(BACKGROUND_COLOR));
        scc(SDL_RenderClear(renderer));

        render_game(renderer, &games[current]);

        SDL_RenderPresent(renderer);
    }

    SDL_Quit();

    return 0;
}
