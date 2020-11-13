#include "./gp_game.h"

FILE *gnuplot_pipe = 0;

#define nOfGnuplotCmds 14

char* gnuplotCmds[] = {
"reset\n",
"set size 1,1\n",
"set origin 0,0\n",
"set xlabel 'Generations'\n",
"set multiplot layout 2, 2 columnsfirst title 'gp live stats'\n",
"set ylabel 'Avg Lifetime'\n",
"plot 'log.dat' using 1:2 with lines lw 2 lt 10 notitle \n",
"set ylabel 'Min lifetime'\n",
"plot 'log.dat' using 1:3 with lines lw 2 lt 10 notitle\n",
"set ylabel 'Max lifetime'\n",
"plot 'log.dat' using 1:4 with lines lw 2 lt 10 notitle\n",
"set ylabel 'Food Eaten'\n",
"plot 'log.dat' using 1:5 with lines lw 2 lt 10 notitle\n",
"unset multiplot\n"
};

float avg_lifetime(Agent *agents)
{
  int result = 0.0;
  for (size_t i = 0; i < AGENTS_COUNT; ++i) {
    result += agents[i].lifetime;
  }

  return  (float) result / AGENTS_COUNT;
}

void log_header(FILE *stream)
{
  fprintf(stream, "#Loggind generations\n");
  fprintf(stream,
          "#Generation, Avg Lifetime, Min Lifetime, Max Lifetime, Food eaten\n");
  gnuplot_pipe = popen("gnuplot -p", "w");
}

void log_generation(FILE *stream, int gen, Game *game)
{
  float avg_lf = avg_lifetime(game->agents);

  int max_lifetime = game->agents[0].lifetime;
  int min_lifetime = game->agents[0].lifetime;
  for (size_t i = 1; i < AGENTS_COUNT; ++i) {
    if (game->agents[i].lifetime > max_lifetime) {
      max_lifetime = game->agents[i].lifetime;
    }
    if (game->agents[i].lifetime < min_lifetime) {
      min_lifetime = game->agents[i].lifetime;
    }
  }

  int food_eaten = 0;
  for (size_t i = 0; i < FOODS_COUNT; ++i) {
    if (game->foods[i].eaten) {
      food_eaten++;
    }
  }

  /* Writing to log file*/
  fprintf(stream, "%d, %f, %d, %d, %d\n", gen, avg_lf, min_lifetime,
          max_lifetime, food_eaten);

}

void log_live_update(void)
{
  for (size_t i = 0; i < nOfGnuplotCmds; ++i) {
    fprintf(gnuplot_pipe, "%s \n", gnuplotCmds[i]);
  }
  fflush(gnuplot_pipe);
}

void log_close_pipe(void)
{
  pclose(gnuplot_pipe);
}
