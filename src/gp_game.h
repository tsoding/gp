#ifndef GP_GAME_H_
#define GP_GAME_H_

#define BOARD_WIDTH 100
#define BOARD_HEIGHT 100

#define AGENTS_COUNT 2000
#define FOODS_COUNT 1000
#define WALLS_COUNT 60
#define JEANS_COUNT 20
#define FOOD_HUNGER_RECOVERY 10
#define STEP_HUNGER_DAMAGE 5
#define HUNGER_MAX 100
#define HEALTH_MAX 100
#define ATTACK_DAMAGE 10
#define STATES_COUNT 7
#define MUTATION_CHANCE 100
#define SELECTION_POOL 50

static_assert(
    AGENTS_COUNT + FOODS_COUNT + WALLS_COUNT <= BOARD_WIDTH * BOARD_HEIGHT,
    "Too many entities. Can't fit all of them on the board");

static_assert(
    JEANS_COUNT % 2 == 0,
    "Amount of jeans in the chormosome must be an even number for the mating "
    "process to happen properly.");

typedef struct {
    int x, y;
} Coord;

int coord_equals(Coord a, Coord b);

typedef enum {
    DIR_RIGHT = 0,
    DIR_UP,
    DIR_LEFT,
    DIR_DOWN,
    DIR_COUNT,
} Dir;

const char *dir_as_cstr(Dir dir);

typedef int State;

typedef enum {
    ENV_NOTHING = 0,
    ENV_AGENT,
    ENV_FOOD,
    ENV_WALL,
    ENV_COUNT,
} Env;

const char *env_as_cstr(Env env);

typedef enum {
    ACTION_NOP = 0,
    ACTION_STEP,
    ACTION_TURN_LEFT,
    ACTION_TURN_RIGHT,
    ACTION_COUNT,
} Action;

const char *action_as_cstr(Action action);

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

void print_chromo(FILE *stream, const Chromo *chromo);

typedef struct {
    Coord pos;
    Dir dir;
    int hunger;
    int health;
    State state;
    int lifetime;
    Chromo chromo;
} Agent;

void print_agent(FILE *stream, const Agent *agent);

typedef struct {
    int eaten;
    Coord pos;
} Food;

typedef struct {
    Coord pos;
} Wall;

typedef struct {
    Agent agents[AGENTS_COUNT];
    Food foods[FOODS_COUNT];
    Wall walls[WALLS_COUNT];
} Game;

int random_int_range(int low, int high);
Dir random_dir(void);
Coord random_coord_on_board(void);
Coord random_empty_coord_on_board(const Game *game);
Env random_env(void);
Action random_action(void);

int mod_int(int a, int b);
int is_cell_empty(const Game *game, Coord pos);

Agent *agent_at(Game *game, Coord coord);

Coord coord_infront_of_agent(const Agent *agent);
Food *food_infront_of_agent(Game *game, size_t agent_index);
Agent *agent_infront_of_agent(Game *game, size_t agent_index);
Wall *wall_infront_of_agent(Game *game, size_t agent_index);
Env env_infront_of_agent(Game *game, size_t agent_index);

void init_game(Game *game);

void dump_game(const char *filepath, const Game *game);
void load_game(const char *filepath, Game *game);

int is_everyone_dead(const Game *game);

void step_agent(Agent *agent);
void execute_action(Game *game, size_t agent_index, Action action);
void step_game(Game *game);

void mate_agents(const Agent *parent1, const Agent *parent2, Agent *child);
void mutate_agent(Agent *agent);

// TODO(#5): more mating strategies for next generations
void make_next_generation(Game *prev_game, Game *next_game);

#endif  // GP_GAME_H_
