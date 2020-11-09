#include "./gp_game.h"


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


int coord_equals(Coord a, Coord b)
{
    return a.x == b.x && a.y == b.y;
}

const char *env_as_cstr(Env env)
{
    switch (env) {
    case ENV_NOTHING: return "ENV_NOTHING";
    case ENV_AGENT:   return "ENV_AGENT";
    case ENV_FOOD:    return "ENV_FOOD";
    case ENV_WALL:    return "ENV_WALL";
    case ENV_COUNT:   {}
    }
    assert(0 && "Unreachable");
}

const char *action_as_cstr(Action action)
{
    switch (action) {
    case ACTION_NOP:        return "ACTION_NOP";
    case ACTION_STEP:       return "ACTION_STEP";
    case ACTION_TURN_LEFT:  return "ACTION_TURN_LEFT";
    case ACTION_TURN_RIGHT: return "ACTION_TURN_RIGHT";
    case ACTION_COUNT:      {}
    }
    assert(0 && "Unreachable");
}

void print_chromo(FILE *stream, const Chromo *chromo)
{
    for (size_t i = 0; i < JEANS_COUNT; ++i) {
        fprintf(stream, "%d %s %s %d\n",
                chromo->jeans[i].state,
                env_as_cstr(chromo->jeans[i].env),
                action_as_cstr(chromo->jeans[i].action),
                chromo->jeans[i].next_state);
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

Env random_env(void)
{
    return random_int_range(0, ENV_COUNT);
}

Action random_action(void)
{
    return random_int_range(0, ACTION_COUNT);
}

void init_game(Game *game)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        game->agents[i].pos = random_empty_coord_on_board(game);
        game->agents[i].dir = random_dir();
        game->agents[i].hunger = HUNGER_MAX;
        game->agents[i].health = HEALTH_MAX;
        game->agents[i].dir = i % 4;
        game->agents[i].lifetime = 0;

        for (size_t j = 0; j < JEANS_COUNT; ++j) {
            game->chromos[i].jeans[j].state = random_int_range(0, STATES_COUNT);
            game->chromos[i].jeans[j].env = random_env();
            game->chromos[i].jeans[j].action = random_action();
            game->chromos[i].jeans[j].next_state = random_int_range(0, STATES_COUNT);
        }
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

Agent *agent_infront_of_agent(Game *game, size_t agent_index)
{
    Coord infront = coord_infront_of_agent(&game->agents[agent_index]);

    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        if (i != agent_index &&
            game->agents[i].health > 0 &&
            coord_equals(infront, game->agents[i].pos))
        {
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

Food *food_infront_of_agent(Game *game, size_t agent_index)
{
    Coord infront = coord_infront_of_agent(&game->agents[agent_index]);

    for (size_t i = 0; i < FOODS_COUNT; ++i) {
        if (!game->foods[i].eaten && coord_equals(infront, game->foods[i].pos)) {
            return &game->foods[i];
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

    case ACTION_STEP: {
        Food *food = food_infront_of_agent(game, agent_index);
        Agent *other_agent = agent_infront_of_agent(game, agent_index);
        Wall *wall = wall_infront_of_agent(game, agent_index);

        if (food != NULL) {
            food->eaten = 1;
            game->agents[agent_index].hunger += FOOD_HUNGER_RECOVERY;
            if (game->agents[agent_index].hunger > HUNGER_MAX) {
                game->agents[agent_index].hunger = HUNGER_MAX;
            }
            step_agent(&game->agents[agent_index]);
        } else if (other_agent != NULL) {
            // TODO: make agents drop the food when they die
            other_agent->health -= ATTACK_DAMAGE;
        } else if (wall == NULL) {
            step_agent(&game->agents[agent_index]);
        }
    } break;

    case ACTION_TURN_LEFT:
        game->agents[agent_index].dir = mod_int(game->agents[agent_index].dir + 1, 4);
        break;

    case ACTION_TURN_RIGHT:
        game->agents[agent_index].dir = mod_int(game->agents[agent_index].dir - 1, 4);
        break;

    case ACTION_COUNT:
        assert(0 && "Unreachable");
        break;
    }
}

void step_game(Game *game)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        if (game->agents[i].health > 0) {
            // Interpret genes
            for (size_t j = 0; j < JEANS_COUNT; ++j) {
                Gene gene = game->chromos[i].jeans[j];
                if (gene.state == game->agents[i].state && gene.env == env_of_agent(game, i)) {
                    execute_action(game, i, gene.action);
                    game->agents[i].state = gene.next_state;
                    break;
                }
            }

            // Apply hunger
            game->agents[i].hunger -= STEP_HUNGER_DAMAGE;
            if (game->agents[i].hunger <= 0) {
                game->agents[i].health = 0;
            }

            game->agents[i].lifetime += 1;
        }
    }
}

Agent *agent_at(Game *game, Coord pos)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        if (coord_equals(game->agents[i].pos, pos)) {
            return &game->agents[i];
        }
    }

    return NULL;
}

void print_agent(FILE *stream, Agent *agent)
{
    printf("Agent {\n");
    printf("  .pos = (%d, %d)\n", agent->pos.x, agent->pos.y);
    printf("  .dir = %s\n", dir_as_cstr(agent->dir));
    printf("  .hunger = %d\n", agent->hunger);
    printf("  .health = %d\n", agent->health);
    printf("  .state = %d\n", agent->state);
    printf("}\n");
}

const char *dir_as_cstr(Dir dir)
{
    switch (dir) {
    case DIR_RIGHT: return "DIR_RIGHT";
    case DIR_UP:    return "DIR_UP";
    case DIR_LEFT:  return "DIR_LEFT";
    case DIR_DOWN:  return "DIR_DOWN";
    case DIR_COUNT: {}
    }
    assert(0 && "Unreachable");
}
