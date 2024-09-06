#include "screen.h"
#include "SDL.h"
#include "defs.h"
#include "glob_var.h"
#include "play_bloxorz.h"

#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_log.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <stdint.h>
// #include <stdio.h>

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

SDL_Rect last1, last2;
Color_t LastColor1, LastColor2;

struct Position RenderPos;

const Color_t Var_player_color = {20, 200, 50};
const Color_t Var_background_color = {60, 60, 60};

// static SDL_Rect *RectMatrix;

const Color_t BlockColors[] = {{190, 190, 255},
                               {255, 255, 255},
                               {255, 255, 255},
                               {255, 0, 0},
                               {255, 200, 0}};

const Color_t BottomColors[] = {{0, 0, 255}};

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

  if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN)) {
    SDL_Log("SDL_SetWindowFullscreen: %s", SDL_GetError());
    CRASH_GAME("Could not put window in full screen\n");
  }

  SDL_SetRenderDrawColor(renderer, Var_background_color.r,
                         Var_background_color.g, Var_background_color.b, 255);
  SDL_RenderClear(renderer);

  SDL_RenderSetScale(renderer, 1, 1);
  SDL_RenderPresent(renderer);

  return EXIT_SUCCESS;
}

#define MINI_SQUARE_LEN 10

/* Does perform SDL_RenderPresent(renderer) */
void SetBackgroundRandomArt(void) {
  SDL_Rect rect;
  rect.h = rect.w = MINI_SQUARE_LEN;
  const Color_t InitColor = {120, 70, 98};
  Color_t RenderColor = InitColor;
  uint8_t Opaccity = 255;

  for (int x = 0; x < SCREEN_SIZE_X; x += MINI_SQUARE_LEN) {
    rect.x = x;
    for (int y = 0; y < SCREEN_SIZE_Y; y += MINI_SQUARE_LEN) {
      rect.y = y;
      RenderColor.b += 7 * (rand() % 3);
      RenderColor.g += 7 * (rand() % 3);
      RenderColor.r += 7 * (rand() % 3);

      // Opaccity = 150 + rand() % (255 - 150 + 1);

      SDL_SetRenderDrawColor(renderer, RenderColor.r, RenderColor.g,
                             RenderColor.b, Opaccity);
      SDL_RenderFillRect(renderer, &rect);
    }
  }

  SDL_RenderPresent(renderer);
  SDL_Event e;
  while (1) {
    SDL_PollEvent(&e);
    if (e.type == SDL_KEYDOWN)
      return;
    SDL_Delay(10);
  }
}

void render_static_texture(char *tex) {
  SDL_Surface *tmp_surface = SDL_LoadBMP(tex);
  if (nullptr == tmp_surface) {
    SDL_Log("Could not create surface from bmp image: %s", SDL_GetError());
    exit(1729);
  }

  SDL_Texture *menu_texture =
      SDL_CreateTextureFromSurface(renderer, tmp_surface);

  SDL_FreeSurface(tmp_surface);

  SDL_RenderCopy(renderer, menu_texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  SDL_DestroyTexture(menu_texture);
  // SDL_Delay(5000);

  SDL_Event e;
  while (1) {
    SDL_Delay(10);
    SDL_PollEvent(&e);
    if (e.type == SDL_KEYDOWN)
      break;
  }

  SetBackgroundRandomArt();
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

void MoveRender(uint8_t move, struct Position DstPos) {
  SDL_SetRenderDrawColor(renderer, LastColor1.r, LastColor1.g, LastColor1.b,
                         255);
  SDL_RenderFillRect(renderer, &last1);
  SDL_SetRenderDrawColor(renderer, LastColor2.r, LastColor2.g, LastColor2.b,
                         255);

  fprintf(stdmoves, "Position: (%d, %d, %d)\n", DstPos.orientation, DstPos.y,
          DstPos.x);
  switch (RenderPos.orientation) {
  case 1:
    last2.x = SQUARE_LEN * RenderPos.y;
    last2.y = SQUARE_LEN * (RenderPos.x + 1);
    // SDL_RenderDrawRect(renderer, &last2);
    SDL_RenderFillRect(renderer, &last2);

    break;
  case 2:
    last2.x = SQUARE_LEN * (RenderPos.y + 1);
    last2.y = SQUARE_LEN * RenderPos.x;
    // SDL_RenderDrawRect(renderer, &last2);
    SDL_RenderFillRect(renderer, &last2);
    break;
  }

  RenderPos = DstPos;
  last1.x = SQUARE_LEN * RenderPos.y;
  last1.y = SQUARE_LEN * RenderPos.x;
  SDL_SetRenderDrawColor(renderer, Var_player_color.r, Var_player_color.g,
                         Var_player_color.b, 255);
  // SDL_RenderDrawRect(renderer, &last2);
  SDL_RenderFillRect(renderer, &last1);

  LastColor1 = BlockColors[map[map_size_x * RenderPos.x + RenderPos.y]];

  switch (RenderPos.orientation) {
  case 1:
    last2.x = SQUARE_LEN * RenderPos.y;
    last2.y = SQUARE_LEN * (RenderPos.x + 1);
    LastColor2 = BlockColors[map[map_size_x * (RenderPos.x + 1) + RenderPos.y]];
    // SDL_RenderDrawRect(renderer, &last2);
    SDL_RenderFillRect(renderer, &last2);

    break;
  case 2:
    last2.x = SQUARE_LEN * (RenderPos.y + 1);
    last2.y = SQUARE_LEN * RenderPos.x;
    LastColor2 = BlockColors[map[map_size_x * RenderPos.x + RenderPos.y + 1]];
    // SDL_RenderDrawRect(renderer, &last2);
    SDL_RenderFillRect(renderer, &last2);
    break;
  }

  SDL_RenderPresent(renderer);
  SDL_Delay(10);
}
