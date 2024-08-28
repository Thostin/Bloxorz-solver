#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <stdlib.h>

#define nullptr NULL

#define SCREEN_SIZE_X 1920
#define SCREEN_SIZE_Y 1080

int InitRects(SDL_Rect **rects) {
#define SQUARE_LEN 30
  SDL_Rect *matrix = malloc(SCREEN_SIZE_X * SCREEN_SIZE_Y /
                            (SQUARE_LEN * SQUARE_LEN) * sizeof(SDL_Rect));
  if (nullptr == matrix) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  for (int i = 0, I = 0; i < SCREEN_SIZE_Y; i += SQUARE_LEN) {
    for (int j = 0, J = 0; j < SCREEN_SIZE_X; j += SQUARE_LEN) {
      matrix[I * (SCREEN_SIZE_X / SQUARE_LEN) + J].x = j;
      matrix[I * (SCREEN_SIZE_X / SQUARE_LEN) + J].y = i;
      matrix[I * (SCREEN_SIZE_X / SQUARE_LEN) + J].w = SQUARE_LEN;
      matrix[I * (SCREEN_SIZE_X / SQUARE_LEN) + J].h = SQUARE_LEN;
      ++J;
    }
    ++I;
  }

  *rects = matrix;

  return EXIT_SUCCESS;
}

int init(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    fprintf(stderr, "Could not init everything\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int CRASH_GAME(void) { exit(1); }

int main(void) {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;

  if (EXIT_FAILURE == init()) {
    printf("<<< main");
    exit(EXIT_FAILURE);
  }

#define DRAW_SCALE 1
  SDL_CreateWindowAndRenderer(SCREEN_SIZE_X * DRAW_SCALE,
                              SCREEN_SIZE_Y * DRAW_SCALE, 0, &window,
                              &renderer);
  SDL_RenderSetScale(renderer, DRAW_SCALE, DRAW_SCALE);

#define BACKGROUND_COLOR 60, 60, 60
  SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR, 255);
  SDL_RenderClear(renderer);

  // SDL_RenderDrawPoint(renderer, 200, 200);

  // 640 * 540
  SDL_Rect *RectMatrix;
  InitRects(&RectMatrix);

#define LINEA_COLOR 255, 255, 255
  SDL_SetRenderDrawColor(renderer, LINEA_COLOR, 255);
  SDL_RenderDrawRects(renderer, RectMatrix,
                      SCREEN_SIZE_X * SCREEN_SIZE_Y /
                          (SQUARE_LEN * SQUARE_LEN));

#define RELLENAR_COLOR 255, 165, 0
#define PLAYER_COLOR 255, 0, 127
  /*
  #define RELLENAR_COLOR 255, 165, 0
  #define PLAYER_COLOR 255, 0, 127
  */
  SDL_SetRenderDrawColor(renderer, RELLENAR_COLOR, 255);

  int posx = 10, posy = 0;
  SDL_RenderFillRect(renderer,
                     RectMatrix + SCREEN_SIZE_X / SQUARE_LEN * posy + posx);
  SDL_RenderPresent(renderer);

#define CANTIDAD_CELDAS                                                        \
  (SCREEN_SIZE_X * SCREEN_SIZE_Y / (SQUARE_LEN * SQUARE_LEN))

  SDL_Event e;
  while (1) {
    SDL_PollEvent(&e);
    if (SDL_KEYDOWN == e.type) {
      switch (e.key.keysym.sym) {
#define la_macron(COLOR)                                                       \
  SDL_SetRenderDrawColor(renderer, COLOR, 255);                                \
  SDL_RenderFillRect(renderer,                                                 \
                     RectMatrix + SCREEN_SIZE_X / SQUARE_LEN * posy + posx);

      case SDLK_RIGHT:
        if (posx == SCREEN_SIZE_X / SQUARE_LEN - 1)
          break;
        la_macron(PLAYER_COLOR);
        ++posx;
        la_macron(RELLENAR_COLOR);
        break;
      case SDLK_LEFT:
        if (0 == posx)
          break;
        la_macron(PLAYER_COLOR);
        --posx;
        la_macron(RELLENAR_COLOR);
        break;
      case SDLK_UP:
        if (0 == posy)
          break;
        la_macron(PLAYER_COLOR);
        --posy;
        la_macron(RELLENAR_COLOR);
        break;
      case SDLK_DOWN:
        if (SCREEN_SIZE_Y / SQUARE_LEN - 1 == posy)
          break;
        la_macron(PLAYER_COLOR);
        ++posy;
        la_macron(RELLENAR_COLOR);
        break;
      case SDLK_q:
        CRASH_GAME();
      }
      SDL_RenderPresent(renderer);
      SDL_Delay(10);
    }
    SDL_Delay(10);
  }
  SDL_Delay(1000);

  return EXIT_SUCCESS;
}
