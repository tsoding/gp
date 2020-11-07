#ifndef GP_VISUAL_H_
#define GP_VISUAL_H_

#include "./gp_game.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define CELL_WIDTH ((float) SCREEN_WIDTH / BOARD_WIDTH)
#define CELL_HEIGHT ((float) SCREEN_HEIGHT / BOARD_HEIGHT)
#define AGENT_PADDING (fminf(CELL_WIDTH, CELL_HEIGHT) / 5.0f)
#define FOOD_PADDING (AGENT_PADDING)

// Style
#define BACKGROUND_COLOR    0x353535FF
#define GRID_COLOR          0x748CABFF
#define WALL_COLOR          (GRID_COLOR)
#define AGENT_ALIVE_COLOR   0xDA2C38FF
#define AGENT_DEAD_COLOR    0x909090FF
#define FOOD_COLOR          0x87C38FFF

float agents_dirs[DIR_COUNT][6] = {
    // DIR_RIGHT
    {0.0, 0.0, 1.0, 0.5, 0.0, 1.0},
    // DIR_UP
    {0.0, 1.0, 0.5, 0.0, 1.0, 1.0},
    // DIR_LEFT
    {1.0, 0.0, 1.0, 1.0, 0.0, 0.5},
    // DIR_DOWN
    {0.0, 0.0, 1.0, 0.0, 0.5, 1.0},
};

int scc(int code);
void *scp(void *ptr);

#define HEX_COLOR(hex)                      \
    ((hex) >> (3 * 8)) & 0xFF,              \
    ((hex) >> (2 * 8)) & 0xFF,              \
    ((hex) >> (1 * 8)) & 0xFF,              \
    ((hex) >> (0 * 8)) & 0xFF

void render_board_grid(SDL_Renderer *renderer);
void render_agent(SDL_Renderer *renderer, Agent agent);
void render_game(SDL_Renderer *renderer, const Game *game);

#endif  // GP_VISUAL_H_
