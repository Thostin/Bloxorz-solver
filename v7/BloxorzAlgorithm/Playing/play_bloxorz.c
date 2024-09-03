#include "play_bloxorz.h"

#include "algorithm.h"
#include "defs.h"
// #include "glob_var.h"
#include "screen.h"

// #include "menu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>

#include <SDL_log.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void CRASH_GAME(char *func_msg) {
  SDL_Quit();
  printf("The game was intentionally crashed by %s\n", func_msg);
  exit(1);
}

int is_legal_position_no_test_steps(const struct Position pos_arg) {
  fprintf(stdverbose, "\n$$$$$$$$$$$$$$$$$$$$$$$\n");

  switch (pos_arg.orientation) {
  case 0:
    if (pos_arg.x >= map_size_y || pos_arg.y >= map_size_x) {
      fprintf(stdverbose, "FALLS DOWN THE MAP\n");
      fprintf(stdverbose, "----------------------------------------\n\n");
      return 0;
    }
    break;
  case 1:
    if (pos_arg.x >= map_size_y || pos_arg.x + 1 >= map_size_y ||
        pos_arg.y >= map_size_x) {
      fprintf(stdverbose, "FALLS DOWN THE MAP\n");
      fprintf(stdverbose, "----------------------------------------\n\n");
      return 0;
    }
    break;
  case 2:
    if (pos_arg.x >= map_size_y || pos_arg.y + 1 >= map_size_x ||
        pos_arg.y >= map_size_x) {
      fprintf(stdverbose, "FALLS DOWN THE MAP\n");
      fprintf(stdverbose, "----------------------------------------\n\n");
      return 0;
    }
  }

  unsigned block_map1;

  block_map1 = map[map_size_x * pos_arg.x + pos_arg.y];
  fprintf(stdverbose, "\n\nBLOCK_MAP: %d\n\n", block_map1);
  if (VOID == block_map1) {
    fprintf(stdverbose, "VOID BLOCK MAP\n");
    fprintf(stdverbose, "----------------------------------------\n\n");
    return 0;
  }

  int u;
  switch (pos_arg.orientation) {
  case 0:
    if (3 == block_map1) {
      fprintf(stdverbose, "NEW WINNING PATH                   $$\n");
      fprintf(stdverbose, "----------------------------------------\n\n");
      // current_winning_path_steps = args->steps;
      // current_shortest_path = args->path;
      return 2;
    }
    if (4 == block_map1) {
      fprintf(stdverbose, "MUERTE POR VIDRIO\n");
      fprintf(stdverbose, "--------------------------------------\n\n");
      return 0;
    }
    break;
  case 1:

    u = map_size_x * (pos_arg.x + 1) + pos_arg.y;
    if ((block_map1 = map[u]) == VOID) {
      fprintf(stdverbose, "\n\nVOID BLOCK_MAP2: %d | u == %d\n\n", block_map1,
              u);
      fprintf(stdverbose, "----------------------------------------\n\n");
      return 0;
    }
    break;
  case 2:
    if ((block_map1 = map[map_size_x * pos_arg.x + pos_arg.y + 1]) == VOID) {
      fprintf(stdverbose, "\n\nVOID BLOCK_MAP2: %d\n\n", block_map1);
      fprintf(stdverbose, "----------------------------------------\n\n");
      return 0;
    }
  }

  return 1;
}

void play_map() {
  if (EXIT_FAILURE == init_SDL())
    CRASH_GAME("Game crashed because of an SDL error");

  init_map();
  InitRenderMap();

  last1.h = SQUARE_LEN;
  last1.w = SQUARE_LEN;
  last2.h = SQUARE_LEN;
  last2.w = SQUARE_LEN;

  unsigned pos_x = start_x;
  unsigned pos_y = start_y;

  struct Position DstPos;
  struct Position ActPos;
  ActPos.orientation = 0;
  ActPos.x = start_x;
  ActPos.y = start_y;

  RenderPos = ActPos;

  last1.x = SQUARE_LEN * start_y;
  last1.y = SQUARE_LEN * start_x;

  last2 = last1;
  // #define PLAYER_COLOR 255, 165, 0

  LastColor1 = Var_background_color;
  LastColor2 = Var_background_color;

  SDL_SetRenderDrawColor(renderer, Var_player_color.r, Var_player_color.g,
                         Var_player_color.b, 255);
  // SDL_RenderDrawRect(renderer, &last1);
  SDL_RenderFillRect(renderer, &last1);
  SDL_RenderPresent(renderer);
  SDL_Delay(10);

  SDL_Event e;
  unsigned int StepsCounter = 0;
  while (1) {
    SDL_PollEvent(&e);
    if (SDL_KEYDOWN == e.type) {
      switch (e.key.keysym.sym) {
#define la_macron(MOVE)                                                        \
  mov_arg(&DstPos, &ActPos, MOVE);                                             \
  switch (is_legal_position_no_test_steps(DstPos)) {                           \
  case 0:                                                                      \
    printf("GAME OVER\n");                                                     \
    CRASH_GAME("play_map");                                                    \
    break;                                                                     \
  case 1:                                                                      \
    ++StepsCounter;                                                            \
    SDL_Log("Rendering Legal Move");                                           \
    ActPos = DstPos;                                                           \
    MoveRender(MOVE, ActPos);                                                  \
    break;                                                                     \
  case 2:                                                                      \
    ++StepsCounter;                                                            \
    SDL_Log("Rendering Winning Move");                                         \
    SDL_Log("Steps: %u\n", StepsCounter);                                      \
    ActPos = DstPos;                                                           \
    MoveRender(MOVE, ActPos);                                                  \
    printf("YOU WON!\n");                                                      \
    CRASH_GAME("winning the game");                                            \
  }
      case SDLK_RIGHT:
      case SDLK_d:
        SDL_Log("SDLK_RIGHT\n");
        la_macron(1);
        break;
      case SDLK_LEFT:
      case SDLK_a:
        SDL_Log("SDLK_LEFT\n");
        la_macron(3);
        break;
      case SDLK_UP:
      case SDLK_w:
        SDL_Log("SDLK_UP\n");
        la_macron(0);
        break;
      case SDLK_DOWN:
      case SDLK_s:
        SDL_Log("SDLK_DOWN\n");
        la_macron(2);
        break;
      case SDLK_q:
        CRASH_GAME("play_map occasionaded CRASH_GAME: SDLK_q");
      }
      // La función MoveRender ya hace eso.
      // SDL_RenderPresent(renderer);
      // SDL_Delay(10);
      SDL_Delay(10);
    } else if (e.type == SDL_QUIT) {
      CRASH_GAME("Game was intentionally closed by user");
    }
    SDL_Delay(10);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
  CRASH_GAME("opa");
}

void animate_map(void) {
  if (EXIT_FAILURE == init_SDL())
    CRASH_GAME("Game crashed because of an SDL error");

  // init_map();
  InitRenderMap();

  last1.h = SQUARE_LEN;
  last1.w = SQUARE_LEN;
  last2.h = SQUARE_LEN;
  last2.w = SQUARE_LEN;

  unsigned pos_x = start_x;
  unsigned pos_y = start_y;

  struct Position DstPos;
  struct Position ActPos;
  ActPos.orientation = 0;
  ActPos.x = start_x;
  ActPos.y = start_y;

  RenderPos = ActPos;

  last1.x = SQUARE_LEN * start_y;
  last1.y = SQUARE_LEN * start_x;

  last2 = last1;
  // #define PLAYER_COLOR 255, 165, 0

  LastColor1 = Var_background_color;
  LastColor2 = Var_background_color;

  SDL_SetRenderDrawColor(renderer, Var_player_color.r, Var_player_color.g,
                         Var_player_color.b, 255);
  // SDL_RenderDrawRect(renderer, &last1);
  SDL_RenderFillRect(renderer, &last1);
  SDL_RenderPresent(renderer);
  SDL_Delay(10);

  uint8_t move_arr[TotalSteps];

  struct Path *path_aux = current_shortest_path;
  int i = TotalSteps - 1;
  while (nullptr != path_aux) {
    move_arr[i] = path_aux->move;
    path_aux = path_aux->from;
    --i;
  }

  SDL_Event e;
  int band;
  for (i = 0; i < TotalSteps; ++i) {
    // printf("Is here?\n");
    band = true;
    while (band) {
      SDL_PollEvent(&e);
      if (e.type == SDL_KEYDOWN)
        band = false;
      // SDL_Delay(1);
    }
    // SDL_Delay(300);
    // printf("Is here?\n");
    mov_arg(&DstPos, &ActPos, move_arr[i]);
    ActPos = DstPos;
    // printf("Is here?\n");
    MoveRender(move_arr[i], DstPos);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
  CRASH_GAME("opa");
}
