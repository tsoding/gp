#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include "./style.h"

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 10

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define CELL_WIDTH ((float) SCREEN_WIDTH / BOARD_WIDTH)
#define CELL_HEIGHT ((float) SCREEN_HEIGHT / BOARD_HEIGHT)

#define AGENTS_COUNT 5
#define AGENT_PADDING (fminf(CELL_WIDTH, CELL_HEIGHT) / 5.0f)

#define FOODS_COUNT 5
#define FOOD_PADDING (AGENT_PADDING)

#define WALLS_COUNT 5

#define JEANS_COUNT 20

#define FOOD_HUNGER_RECOVERY 10
#define STEP_HUNGER_DAMAGE 5
#define HUNGER_MAX 100
#define HEALTH_MAX 100
#define ATTACK_DAMAGE 10

typedef struct {
    int x, y;
} Coord;

int coord_equals(Coord a, Coord b)
{
    return a.x == b.x && a.y == b.y;
}

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

#define HEX_COLOR(hex)                      \
    ((hex) >> (3 * 8)) & 0xFF,              \
    ((hex) >> (2 * 8)) & 0xFF,              \
    ((hex) >> (1 * 8)) & 0xFF,              \
    ((hex) >> (0 * 8)) & 0xFF

typedef enum {
    DIR_RIGHT = 0,
    DIR_UP,
    DIR_LEFT,
    DIR_DOWN,
} Dir;

float agents_dirs[4][6] = {
    // DIR_RIGHT
    {0.0, 0.0, 1.0, 0.5, 0.0, 1.0},
    // DIR_UP
    {0.0, 1.0, 0.5, 0.0, 1.0, 1.0},
    // DIR_LEFT
    {1.0, 0.0, 1.0, 1.0, 0.0, 0.5},
    // DIR_DOWN
    {0.0, 0.0, 1.0, 0.0, 0.5, 1.0},
};

Coord coord_dirs[4] = {
    // DIR_RIGHT
    {1, 0},
    // DIR_UP
    {0, -1},
    // DIR_LEFT
    {-1, 0},
    // DIR_DOWN
    {0, 1}
};

typedef int State;

typedef enum {
    ENV_NOTHING = 0,
    ENV_AGENT,
    ENV_FOOD,
    ENV_WALL,
} Env;

typedef enum {
    ACTION_NOP = 0,
    ACTION_STEP,
    ACTION_EAT,
    ACTION_ATTACK,
    ACTION_TURN_LEFT,
    ACTION_TURN_RIGHT,
} Action;

typedef struct {
    State state;
    Env env;
    Action action;
    State next_state;
} Gene;

typedef struct {
    size_t count;
    Gene jeans[JEANS_COUNT];
} Chromo;

typedef struct {
    Coord pos;
    Dir dir;
    int hunger;
    int health;
    State state;
} Agent;

typedef struct {
    int eaten;
    Coord pos;
} Food;

typedef struct {
    Coord pos;
} Wall;

typedef struct {
    Agent agents[AGENTS_COUNT];
    Chromo chromos[AGENTS_COUNT];
    Food foods[FOODS_COUNT];
    Wall walls[WALLS_COUNT];
} Game;

static_assert(AGENTS_COUNT + FOODS_COUNT + WALLS_COUNT <= BOARD_WIDTH * BOARD_HEIGHT,
              "Too many entities. Can't fit all of them on the board");

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

int random_int_range(int low, int high)
{
    return rand() % (high - low) + low;
}

Dir random_dir(void)
{
    return (Dir) random_int_range(0, 4);
}

Coord random_coord_on_board(void)
{
    Coord result;
    result.x = random_int_range(0, BOARD_WIDTH);
    result.y = random_int_range(0, BOARD_HEIGHT);
    return result;
}

int is_cell_empty(const Game *game, Coord pos)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        if (coord_equals(game->agents[i].pos, pos)) {
            return 0;
        }
    }

    for (size_t i = 0; i < FOODS_COUNT; ++i) {
        if (coord_equals(game->foods[i].pos, pos)) {
            return 0;
        }
    }

    for (size_t i = 0; i < WALLS_COUNT; ++i) {
        if (coord_equals(game->walls[i].pos, pos)) {
            return 0;
        }
    }

    return 1;
}

Coord random_empty_coord_on_board(const Game *game)
{
    Coord result = random_coord_on_board();
    while (!is_cell_empty(game, result)) {
        result = random_coord_on_board();
    }
    return result;
}

void render_agent(SDL_Renderer *renderer, Agent agent)
{
    float x1 = agents_dirs[agent.dir][0] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos.x * CELL_WIDTH  + AGENT_PADDING;
    float y1 = agents_dirs[agent.dir][1] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos.y * CELL_HEIGHT + AGENT_PADDING;
    float x2 = agents_dirs[agent.dir][2] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos.x * CELL_WIDTH  + AGENT_PADDING;
    float y2 = agents_dirs[agent.dir][3] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos.y * CELL_HEIGHT + AGENT_PADDING;
    float x3 = agents_dirs[agent.dir][4] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos.x * CELL_WIDTH  + AGENT_PADDING;
    float y3 = agents_dirs[agent.dir][5] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos.y * CELL_HEIGHT + AGENT_PADDING;

    filledTrigonColor(renderer, x1, y1, x2, y2, x3, y3, AGENT_COLOR);
    aatrigonColor(renderer, x1, y1, x2, y2, x3, y3, AGENT_COLOR);
}

void render_game(SDL_Renderer *renderer, const Game *game)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        render_agent(renderer, game->agents[i]);
    }

    // TODO: foods are not rendered
    for (size_t i = 0; i < FOODS_COUNT; ++i) {
        filledCircleRGBA(
            renderer,
            (int) floorf(game->foods[i].pos.x * CELL_WIDTH + CELL_WIDTH * 0.5f),
            (int) floorf(game->foods[i].pos.y * CELL_HEIGHT + CELL_HEIGHT * 0.5f),
            (int) floorf(fminf(CELL_WIDTH, CELL_HEIGHT) * 0.5f - FOOD_PADDING),
            HEX_COLOR(FOOD_COLOR));
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

void init_game(Game *game)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        game->agents[i].pos = random_empty_coord_on_board(game);
        game->agents[i].dir = random_dir();
        game->agents[i].hunger = HUNGER_MAX;
        game->agents[i].health = HEALTH_MAX;
        game->agents[i].dir = i % 4;
    }

    for (size_t i = 0; i < FOODS_COUNT; ++i) {
        game->foods[i].pos = random_empty_coord_on_board(game);
    }

    for (size_t i = 0; i < WALLS_COUNT; ++i) {
        game->walls[i].pos = random_empty_coord_on_board(game);
    }
}

int mod_int(int a, int b)
{
    return (a % b + b) % b;
}

Coord coord_infront_of_agent(const Agent *agent)
{
    Coord d = coord_dirs[agent->dir];
    Coord result = agent->pos;
    result.x = mod_int(result.x + d.x, BOARD_WIDTH);
    result.y = mod_int(result.y + d.y, BOARD_HEIGHT);
    return result;
}


void step_agent(Agent *agent)
{
    Coord d = coord_dirs[agent->dir];
    agent->pos.x = mod_int(agent->pos.x + d.x, BOARD_WIDTH);
    agent->pos.y = mod_int(agent->pos.y + d.y, BOARD_HEIGHT);
}

Food *food_infront_of_agent(Game *game, size_t agent_index)
{
    Coord infront = coord_infront_of_agent(&game->agents[agent_index]);

    for (size_t i = 0; i < FOODS_COUNT; ++i) {
        if (coord_equals(infront, game->foods[i].pos)) {
            return &game->foods[i];
        }
    }

    return NULL;
}

Agent *agent_infront_of_agent(Game *game, size_t agent_index)
{
    Coord infront = coord_infront_of_agent(&game->agents[agent_index]);

    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        if (i != agent_index && coord_equals(infront, game->agents[i].pos)) {
            return &game->agents[i];
        }
    }

    return NULL;
}

Wall *wall_infront_of_agent(Game *game, size_t agent_index)
{
    Coord infront = coord_infront_of_agent(&game->agents[agent_index]);

    for (size_t i = 0; i < WALLS_COUNT; ++i) {
        if (coord_equals(infront, game->walls[i].pos)) {
            return &game->walls[i];
        }
    }

    return NULL;
}

Env env_of_agent(Game *game, size_t agent_index)
{
    if (food_infront_of_agent(game, agent_index) != NULL) {
        return ENV_FOOD;
    }

    if (wall_infront_of_agent(game, agent_index) != NULL) {
        return ENV_WALL;
    }

    if (agent_infront_of_agent(game, agent_index) != NULL) {
        return ENV_AGENT;
    }

    return ENV_NOTHING;
}

void execute_action(Game *game, size_t agent_index, Action action)
{
    switch (action) {
    case ACTION_NOP:
        break;

    case ACTION_STEP:
        if (env_of_agent(game, agent_index) != ENV_WALL) {
            step_agent(&game->agents[agent_index]);
        }
        break;

    case ACTION_EAT: {
        Food *food = food_infront_of_agent(game, agent_index);
        if (food != NULL) {
            food->eaten = 1;
            game->agents[agent_index].hunger += FOOD_HUNGER_RECOVERY;
            if (game->agents[agent_index].hunger > HUNGER_MAX) {
                game->agents[agent_index].hunger = HUNGER_MAX;
            }
        }
    } break;

    case ACTION_ATTACK: {
        // TODO: make agents drop the food when they die
        Agent *other_agent = agent_infront_of_agent(game, agent_index);
        if (other_agent != NULL) {
            other_agent->health -= ATTACK_DAMAGE;
        }
    } break;

    case ACTION_TURN_LEFT:
        game->agents[agent_index].dir = mod_int(game->agents[agent_index].dir + 1, 4);
        break;

    case ACTION_TURN_RIGHT:
        game->agents[agent_index].dir = mod_int(game->agents[agent_index].dir - 1, 4);
        break;
    }
}

void step_game(Game *game)
{
    // Interpret genes
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        for (size_t j = 0; j < JEANS_COUNT; ++j) {
            Gene gene = game->chromos[i].jeans[j];
            if (gene.state == game->agents[i].state && gene.env == env_of_agent(game, i)) {
                execute_action(game, i, gene.action);
                game->agents[i].state = gene.next_state;
            }
        }
    }

    // Handle hunger
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        game->agents[i].hunger -= STEP_HUNGER_DAMAGE;
        if (game->agents[i].hunger <= 0) {
            game->agents[i].health = 0;
        }
    }
}

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
                }
            } break;
            }
        }

        SDL_SetRenderDrawColor(renderer, HEX_COLOR(BACKGROUND_COLOR));
        scc(SDL_RenderClear(renderer));

        render_board_grid(renderer);
        render_game(renderer, &game);

        SDL_RenderPresent(renderer);
    }

    SDL_Quit();

    return 0;
}
