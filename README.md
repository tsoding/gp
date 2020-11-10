# Genetic Programming

Just a Playground Repo for my Genetic Programming experiments. I have no prior exprience with Genetic Programming and Algorithms.

![demo](./assets/demo.png)

The development of this project was performed live on [https://twitch.tv/tsoding](https://twitch.tv/tsoding). Every stream is archived on YouTube and can be found in [this playlist](https://www.youtube.com/playlist?list=PLpM-Dvs8t0VZhPhStYD0aS30Y1awAv-DO).

## Idea

- Write a simple game where agents are trying to survive in an environment fighting for food and with each other.
- The behaviour of a single agent is described with a Turing machine style state machine that we call Chromosomes.
- Use Genetic Algorithms to find an optimial behaviour that survives the longest.

<!-- TODO(#3): Rules and mechanics of the game is not documented -->

### Environment

*TBD*

### Entities

*TBD*

### Agents

#### State

*TBD*

#### Action

*TBD*

#### Chromosomes

*TBD*

## Dependencies

Should be available on Windows, MacOS, and the majority of Linux distros.

- [SDL2]
- [SDL2_gfx]

## Quick Start

```console
$ make
$ ./gp_trainer 69 ./output.bin
$ ./gp_simulator ./output.bin
```

## gp_trainer

Lets you select agents over several generations in a batch mode. The results can be simulated and inspected by [gp_simulator](#gp_simulator) and [gp_inspector](#gp_inspector).

## gp_simulator

Lets you interactively step through the simulation of the generation produced by [gp_trainer](#gp_trainer).

### Controls

#### Keyboard

| Key              | Action                                                    |
|------------------|-----------------------------------------------------------|
| <kbd>r</kbd>     | Generate completely new state of the game                 |
| <kbd>n</kbd>     | Make a new generation based on the best performing agents |
| <kbd>SPACE</kbd> | Step the state of the game                                |

#### Mouse

- Clicking on agents (red triangles) dumps their state to stdout

## gp_inspector

Lets you inspect generations produced by [gp_trainer](#gp_trainer).

## References

- https://en.wikipedia.org/wiki/Genetic_algorithm
- https://en.wikipedia.org/wiki/Turing_machine
- https://github.com/tsoding/turd

[SDL2]: https://www.libsdl.org/
[SDL2_gfx]: https://github.com/ferzkopp/SDL_gfx
