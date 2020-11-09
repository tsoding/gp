#include "./gp_game.h"
#include "./gp_visual.h"

Game game = {0};

int main(int argc, char *argv[])
{
    srand(time(0));

    init_game(&game);

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
                    step_game(&game);
                } break;

                case SDLK_r: {
                    init_game(&game);
                } break;

                case SDLK_n: {
                    print_best_agents(stdout, &game, 5);
                    // TODO: introduce action that manually generates a new generation
                    //   - Take the agents that survived longest
                    //   - Crossover their jeans
                    //   - Mutate
                    //   - Make a new generation
                } break;
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                Coord pos;
                pos.x = (int) floorf(event.button.x / CELL_WIDTH);
                pos.y = (int) floorf(event.button.y / CELL_HEIGHT);
                Agent *agent = agent_at(&game, pos);

                if (agent) {
                    print_agent(stdout, agent);
                }
            } break;
            }
        }

        SDL_SetRenderDrawColor(renderer, HEX_COLOR(BACKGROUND_COLOR));
        scc(SDL_RenderClear(renderer));

        render_game(renderer, &game);

        SDL_RenderPresent(renderer);
    }

    SDL_Quit();

    return 0;
}
