#include "screen.h"
#include "SDL.h"
#include "defs.h"
#include "glob_var.h"
#include <stdint.h>
// #include <stdio.h>

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

SDL_Rect last1, last2;
Color_t LastColor1, LastColor2;

struct Position RenderPos;

const Color_t Var_player_color = {255, 0, 165};
const Color_t Var_background_color = {60, 60, 60};

// static SDL_Rect *RectMatrix;

const Color_t BlockColors[] = {{190, 190, 255},
                               {255, 255, 255},
                               {255, 255, 255},
                               {255, 0, 0},
                               {255, 200, 0}};

int init_SDL(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    SDL_Log("SDL2 init error: %s\n", SDL_GetError());
    // fprintf(stderr, "Could not init everything\n");
    return EXIT_FAILURE;
  }
  SDL_CreateWindowAndRenderer(SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, &window,
                              &renderer);

  if (nullptr == window || nullptr == renderer) {
    SDL_Log("SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_SetRenderDrawColor(renderer, Var_background_color.r,
                         Var_background_color.g, Var_background_color.b, 255);
  SDL_RenderClear(renderer);

  SDL_RenderSetScale(renderer, 1, 1);
  SDL_RenderPresent(renderer);

  return EXIT_SUCCESS;
}

void InitRenderMap(void) {
  Color_t aux;
  SDL_Rect rect;
  rect.w = rect.h = SQUARE_LEN;
  for (int i = 0; i < map_size_x; ++i) {
    for (int j = 0; j < map_size_y; ++j) {
      aux = BlockColors[map[j * map_size_x + i]];
      SDL_SetRenderDrawColor(renderer, aux.r, aux.g, aux.b, 255);
      rect.x = SQUARE_LEN * i;
      rect.y = SQUARE_LEN * j;
      SDL_RenderFillRect(renderer, &rect);
    }
  }
}
