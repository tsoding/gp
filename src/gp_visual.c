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

void render_agent(SDL_Renderer *renderer, Agent agent)
{
    float x1 = agents_dirs[agent.dir][0] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos.x * CELL_WIDTH  + AGENT_PADDING;
    float y1 = agents_dirs[agent.dir][1] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos.y * CELL_HEIGHT + AGENT_PADDING;
    float x2 = agents_dirs[agent.dir][2] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos.x * CELL_WIDTH  + AGENT_PADDING;
    float y2 = agents_dirs[agent.dir][3] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos.y * CELL_HEIGHT + AGENT_PADDING;
    float x3 = agents_dirs[agent.dir][4] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos.x * CELL_WIDTH  + AGENT_PADDING;
    float y3 = agents_dirs[agent.dir][5] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos.y * CELL_HEIGHT + AGENT_PADDING;

    filledTrigonRGBA(renderer, x1, y1, x2, y2, x3, y3, HEX_COLOR(AGENT_COLOR));
    aatrigonRGBA(renderer, x1, y1, x2, y2, x3, y3, HEX_COLOR(AGENT_COLOR));
}

void render_game(SDL_Renderer *renderer, const Game *game)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        if (game->agents[i].health > 0) {
            render_agent(renderer, game->agents[i]);
        }
    }

    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            if (game->foods[y][x]) {
                filledCircleRGBA(
                    renderer,
                    (int) floorf(x * CELL_WIDTH + CELL_WIDTH * 0.5f),
                    (int) floorf(y * CELL_HEIGHT + CELL_HEIGHT * 0.5f),
                    (int) floorf(fminf(CELL_WIDTH, CELL_HEIGHT) * 0.5f - FOOD_PADDING),
                    HEX_COLOR(FOOD_COLOR));
            }
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
