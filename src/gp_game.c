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

    if (game->foods[pos.y][pos.x]) {
        return 0;
    }

    if (game->walls[pos.y][pos.x]) {
        return 0;
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
    memset(game, 0, sizeof(*game));

    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        game->agents[i].pos = random_empty_coord_on_board(game);
        game->agents[i].dir = random_dir();
        game->agents[i].hunger = HUNGER_MAX;
        game->agents[i].health = HEALTH_MAX;
        game->agents[i].dir = i % 4;
        game->agents[i].lifetime = 0;

        for (size_t j = 0; j < JEANS_COUNT; ++j) {
            game->agents[i].chromo.jeans[j].state = random_int_range(0, STATES_COUNT);
            game->agents[i].chromo.jeans[j].env = random_env();
            game->agents[i].chromo.jeans[j].action = random_action();
            game->agents[i].chromo.jeans[j].next_state = random_int_range(0, STATES_COUNT);
        }
    }

    for (size_t i = 0; i < FOODS_COUNT; ++i) {
        Coord pos = random_empty_coord_on_board(game);
        game->foods[pos.y][pos.x] = 1;
    }

    for (size_t i = 0; i < WALLS_COUNT; ++i) {
        Coord pos = random_empty_coord_on_board(game);
        game->walls[pos.y][pos.x] = 1;
    }
}

void dump_game(const char *filepath, const Game *game)
{
    FILE *stream = fopen(filepath, "wb");
    if (stream == NULL) {
        fprintf(stderr, "Could not open file `%s`\n", filepath);
        exit(1);
    }

    fwrite(game, sizeof(*game), 1, stream);

    if (ferror(stream)) {
        fprintf(stderr, "Could not dump to file `%s`\n", filepath);
        exit(1);
    }

    fclose(stream);
}

void load_game(const char *filepath, Game *game)
{
    FILE *stream = fopen(filepath, "rb");

    if (stream == NULL) {
        fprintf(stderr, "Could not open file `%s`\n", filepath);
        exit(1);
    }

    size_t n = fread(game, sizeof(*game), 1, stream);
    assert(n == 1);

    if (ferror(stream)) {
        fprintf(stderr, "Could not load from file `%s`\n", filepath);
        exit(1);
    }

    fclose(stream);
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

int *wall_infront_of_agent(Game *game, size_t agent_index)
{
    Coord infront = coord_infront_of_agent(&game->agents[agent_index]);

    if (game->walls[infront.y][infront.x]) {
        return &game->walls[infront.y][infront.x];
    }

    return NULL;
}

int *food_infront_of_agent(Game *game, size_t agent_index)
{
    Coord infront = coord_infront_of_agent(&game->agents[agent_index]);

    if (game->foods[infront.y][infront.x]) {
        return &game->foods[infront.y][infront.x];
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
        int *food = food_infront_of_agent(game, agent_index);
        Agent *other_agent = agent_infront_of_agent(game, agent_index);
        int *wall = wall_infront_of_agent(game, agent_index);

        if (food != NULL) {
            *food = 0;
            game->agents[agent_index].hunger += FOOD_HUNGER_RECOVERY;
            if (game->agents[agent_index].hunger > HUNGER_MAX) {
                game->agents[agent_index].hunger = HUNGER_MAX;
            }
            step_agent(&game->agents[agent_index]);
        } else if (other_agent != NULL) {
            other_agent->health -= ATTACK_DAMAGE;

            if (other_agent->health <= 0) {
                game->agents[agent_index].hunger += FOOD_HUNGER_RECOVERY;
                if (game->agents[agent_index].hunger > HUNGER_MAX) {
                    game->agents[agent_index].hunger = HUNGER_MAX;
                }
                step_agent(&game->agents[agent_index]);
            }
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
                Gene gene = game->agents[i].chromo.jeans[j];
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

void print_agent(FILE *stream, const Agent *agent)
{
    printf("Agent {\n");
    printf("  .pos = (%d, %d)\n", agent->pos.x, agent->pos.y);
    printf("  .dir = %s\n", dir_as_cstr(agent->dir));
    printf("  .hunger = %d\n", agent->hunger);
    printf("  .health = %d\n", agent->health);
    printf("  .state = %d\n", agent->state);
    printf("  .lifetime = %d\n", agent->lifetime);
    printf("  .chromo = \n");
    print_chromo(stream, &agent->chromo);
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

int compare_agents_lifetimes(const void *a, const void *b)
{
    const Agent *agent_a = a;
    const Agent *agent_b = b;
    return agent_b->lifetime - agent_a->lifetime;
}

void mate_agents(const Agent *parent1, const Agent *parent2, Agent *child)
{
    const size_t length = JEANS_COUNT / 2;
    memcpy(child->chromo.jeans,
           parent1->chromo.jeans,
           length * sizeof(Gene));
    memcpy(child->chromo.jeans + length,
           parent2->chromo.jeans + length,
           length * sizeof(Gene));
}

void mutate_agent(Agent *agent)
{
    for (size_t i = 0; i < JEANS_COUNT; ++i) {
        if (random_int_range(0, MUTATION_CHANCE) == 0) {
            agent->chromo.jeans[i].state = random_int_range(0, STATES_COUNT);
            agent->chromo.jeans[i].env = random_env();
            agent->chromo.jeans[i].action = random_action();
            agent->chromo.jeans[i].next_state = random_int_range(0, STATES_COUNT);
        }
    }
}

void make_next_generation(Game *prev_game, Game *next_game)
{
    memset(next_game, 0, sizeof(*next_game));

    qsort(prev_game->agents, AGENTS_COUNT, sizeof(Agent),
          compare_agents_lifetimes);

    for (size_t i = 0; i < FOODS_COUNT; ++i) {
        Coord pos = random_empty_coord_on_board(next_game);
        next_game->foods[pos.y][pos.x] = 1;
    }

    for (size_t i = 0; i < WALLS_COUNT; ++i) {
        Coord pos = random_empty_coord_on_board(next_game);
        next_game->walls[pos.y][pos.x] = 1;
    }

    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        size_t p1 = random_int_range(0, SELECTION_POOL);
        size_t p2 = random_int_range(0, SELECTION_POOL);

        mate_agents(&prev_game->agents[p1],
                    &prev_game->agents[p2],
                    &next_game->agents[i]);
        mutate_agent(&next_game->agents[i]);

        next_game->agents[i].pos = random_empty_coord_on_board(next_game);
        next_game->agents[i].dir = random_dir();
        next_game->agents[i].hunger = HUNGER_MAX;
        next_game->agents[i].health = HEALTH_MAX;
        next_game->agents[i].dir = i % 4;
        next_game->agents[i].lifetime = 0;
    }
}

int is_everyone_dead(const Game *game)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        if (game->agents[i].health > 0) {
            return 0;
        }
    }

    return 1;
}
