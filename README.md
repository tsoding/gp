# Genetic Programming

Just a Playground Repo for my Genetic Programming experiments. I have no priori exprience with Genetic Programming and Algorithms.

## Idea

- Write a simple game where agents are trying to survive in an environment fighting for food and with each other.
- The behaviour of a single agent is described with a Turing machine style state machine.
- Use Genetic Algorithms to find an optimial behaviour that survives the longest.

### Environment

<!-- TODO: Document environment -->
TBD

### Entities

<!-- TODO: Document entities -->
TBD

### Agents

<!-- TODO: Document agents -->
TBD

## Dependencies

Should be available on Windows, MacOS, and the majority of Linux distros.

- [SDL2]
- [SDL2_gfx]

## Quick Start

```console
$ make
$ ./gp
```

## Controls

### Keyboard

| Key              | Action                         |
|------------------|--------------------------------|
| <kbd>r</kbd>     | Generate new state of the game |
| <kbd>SPACE</kbd> | Step the state of the game     |

### Mouse

- Clicking on agents (red triangles) dumps their state to stdout

[SDL2]: https://www.libsdl.org/
[SDL2_gfx]: https://github.com/ferzkopp/SDL_gfx
