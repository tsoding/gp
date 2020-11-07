#include "./gp_visual.h"

int scc(int code)
{
    if (code < 0) {
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
        exit(1);
    }

    return code;
}

void *scp(void *ptr)
{
    if (ptr == NULL) {
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
        exit(1);
    }

    return ptr;
}

void render_board_grid(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, HEX_COLOR(GRID_COLOR));

    for (int x = 1; x < BOARD_WIDTH; ++x) {
        scc(SDL_RenderDrawLine(
                renderer,
                x * CELL_WIDTH,
                0,
                x * CELL_WIDTH,
                SCREEN_HEIGHT));
    }

    for (int y = 1; y < BOARD_HEIGHT; ++y) {
        scc(SDL_RenderDrawLine(
                renderer,
                0,
                y * CELL_HEIGHT,
                SCREEN_WIDTH,
                y * CELL_HEIGHT));
    }
}

void render_agent(SDL_Renderer *renderer, Agent agent)
{
    float x1 = agents_dirs[agent.dir][0] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos.x * CELL_WIDTH  + AGENT_PADDING;
    float y1 = agents_dirs[agent.dir][1] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos.y * CELL_HEIGHT + AGENT_PADDING;
    float x2 = agents_dirs[agent.dir][2] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos.x * CELL_WIDTH  + AGENT_PADDING;
    float y2 = agents_dirs[agent.dir][3] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos.y * CELL_HEIGHT + AGENT_PADDING;
    float x3 = agents_dirs[agent.dir][4] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos.x * CELL_WIDTH  + AGENT_PADDING;
    float y3 = agents_dirs[agent.dir][5] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos.y * CELL_HEIGHT + AGENT_PADDING;

    Uint32 color = agent.health <= 0 ? AGENT_DEAD_COLOR : AGENT_ALIVE_COLOR;

    filledTrigonRGBA(renderer, x1, y1, x2, y2, x3, y3, HEX_COLOR(color));
    aatrigonRGBA(renderer, x1, y1, x2, y2, x3, y3, HEX_COLOR(color));
}

void render_game(SDL_Renderer *renderer, const Game *game)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        render_agent(renderer, game->agents[i]);
    }

    for (size_t i = 0; i < FOODS_COUNT; ++i) {
        if (!game->foods[i].eaten) {
            filledCircleRGBA(
                renderer,
                (int) floorf(game->foods[i].pos.x * CELL_WIDTH + CELL_WIDTH * 0.5f),
                (int) floorf(game->foods[i].pos.y * CELL_HEIGHT + CELL_HEIGHT * 0.5f),
                (int) floorf(fminf(CELL_WIDTH, CELL_HEIGHT) * 0.5f - FOOD_PADDING),
                HEX_COLOR(FOOD_COLOR));
        }
    }

    for (size_t i = 0; i < WALLS_COUNT; ++i) {
        SDL_Rect rect = {
            (int) floorf(game->walls[i].pos.x * CELL_WIDTH),
            (int) floorf(game->walls[i].pos.y * CELL_HEIGHT),
            (int) floorf(CELL_WIDTH),
            (int) floorf(CELL_HEIGHT),
        };

        SDL_SetRenderDrawColor(renderer, HEX_COLOR(WALL_COLOR));

        SDL_RenderFillRect(renderer, &rect);
    }
}
