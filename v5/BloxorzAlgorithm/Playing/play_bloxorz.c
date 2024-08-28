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

// #include <time.h>

/*
#ifndef nullptr
#define nullptr NULL
#endif
*/

int CRASH_GAME(char *func_msg) {
  SDL_Quit();
  printf("The game was intentionally crashed by %s\n", func_msg);
  exit(1);
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

/*
switch (pos_hash) {
// Case of orientation = 0
case 5:
  RenderPos.orientation = 1;
  RenderPos.x -= 2;
  break;
case 7:
  RenderPos.orientation = 2;
  RenderPos.y += 1;
  break;
case 11:
  RenderPos.orientation = 1;
  RenderPos.x += 1;
  break;
case 13:
  RenderPos.orientation = 2;
  RenderPos.y -= 2;
  break;

// Case of orientation = 1
case 10:
  RenderPos.orientation = 0;
  RenderPos.x -= 1;
  break;
case 14:
  RenderPos.orientation = 1;
  RenderPos.y += 1;
  break;
case 22:
  RenderPos.orientation = 0;
  RenderPos.x += 2;
  break;
case 26:
  RenderPos.orientation = 1;
  RenderPos.y -= 1;
  break;

// Case orientation = 2
case 15:
  RenderPos.orientation = 2;
  RenderPos.x -= 1;
  break;
case 21:
  RenderPos.orientation = 0;
  RenderPos.y += 2;
  break;
case 33:
  RenderPos.orientation = 2;
  RenderPos.x += 1;
  break;
case 39:
  RenderPos.orientation = 0;
  RenderPos.y -= 1;
  break;
}
*/

/*
int InitRects(SDL_Rect **rects) {
  //
  #define SCREEN_SIZE_X map_size_x
  #define SCREEN_SIZE_Y map_size_y


  SDL_Rect *matrix = malloc(SCREEN_SIZE_X * SCREEN_SIZE_Y * sizeof(SDL_Rect));
  if (nullptr == matrix) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  for (int I = 0; I < SCREEN_SIZE_Y; I++) {
    for (int J = 0; J < SCREEN_SIZE_X; J++) {
      matrix[I * (SCREEN_SIZE_X) + J].x = J * SQUARE_LEN;
      matrix[I * (SCREEN_SIZE_X) + J].y = I * SQUARE_LEN;
      matrix[I * (SCREEN_SIZE_X) + J].w = SQUARE_LEN;
      matrix[I * (SCREEN_SIZE_X) + J].h = SQUARE_LEN;
    }
  }

  *rects = matrix;

  last1.h = SQUARE_LEN;
  last1.w = SQUARE_LEN;
  last2.h = SQUARE_LEN;
  last2.w = SQUARE_LEN;

  return EXIT_SUCCESS;
}*/

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
  /*
  SDL_Event e;
  unsigned int StepsCounter = 0;
  while (1) {
    SDL_PollEvent(&e);
    if (SDL_KEYDOWN == e.type) {
      switch (e.key.keysym.sym) {
#define la_macron(MOVE)                                                        \
  mov_arg(&DstPos, &ActPos, MOVE);                                             \
  switch (is_legal_position((void *)&DstPos)) {                                \
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
  */
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
  CRASH_GAME("opa");
}
/*
void prueba_play_map() {
  init_map();

  if (EXIT_FAILURE == init_SDL())
    CRASH_GAME("Game crashed because of an SDL error");

  SDL_CreateWindowAndRenderer(SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, &window,
                              &renderer);

  if (nullptr == window || nullptr == renderer) {
    SDL_Log("SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
    CRASH_GAME("");
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderClear(renderer);

  SDL_RenderPresent(renderer);
  SDL_Event e;

  const time_t EndTime = time(NULL) + 5;
  int band = true;
  while (true == band && time(NULL) < EndTime) {
    SDL_PollEvent(&e);
    if (e.type == SDL_KEYDOWN)
      switch (e.key.keysym.sym) {
      case SDL_QUIT:
        printf("SDL_QUIT\n");
        band = false;
        break;
      case SDLK_q:
        printf("SDLK_q\n");
        band = false;
        break;
      case SDLK_p:
        SDL_Log("SDL XDD\n");
      }
    SDL_Delay(5);
  }
  SDL_Log("SDL log prueba");
  SDL_Delay(1);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
  CRASH_GAME("opa");

#define DRAW_SCALE 1

  SDL_RenderSetScale(m_renderer, DRAW_SCALE, DRAW_SCALE);
  SDL_RenderSetScale_renamed_SDL_SetRenderScale(m_renderer,DRAW_SCALE);

#define BACKGROUND_COLOR 60, 60, 60
  SDL_SetRenderDrawColor(m_renderer, BACKGROUND_COLOR, 255);
  SDL_RenderClear(m_renderer);

  SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
  SDL_RenderDrawPoint(m_renderer, 100, 100);

  SDL_RenderPresent(m_renderer);
  SDL_Delay(2000);
  CRASH_GAME("It's just a test");
}
*/

/*
void kplay_map() {
  if (EXIT_FAILURE == init()) {
    printf("<<< main");
    exit(EXIT_FAILURE);
  }

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    fprintf(stderr, "Could not init everything\n");
    // return EXIT_FAILURE;
    CRASH_GAME("SDL did not init everything");
  }

  SDL_CreateWindowAndRenderer(SCREEN_SIZE_X * SQUARE_LEN,
                              SCREEN_SIZE_Y * SQUARE_LEN, 0, &window,
                              &renderer);

#define DRAW_SCALE 1
  SDL_RenderSetScale(renderer, DRAW_SCALE, DRAW_SCALE);

#define BACKGROUND_COLOR 60, 60, 60
  SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR, 255);
  SDL_RenderClear(renderer);

  SDL_RenderPresent(renderer);

  fprintf(stdout, "Sleeping... ");
  sleep(2);

  CRASH_GAME("Just to see if the screen is created\n");

  fprintf(stderr, "\n");

  init_map();
  // InitRects(&RectMatrix);

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

  last1.x = start_y;
  last1.y = start_x;

  last2 = last1;
#define PLAYER_COLOR 255, 165, 0


  LastColor1 = Var_background_color;
  LastColor2 = Var_background_color;


  SDL_SetRenderDrawColor(renderer, PLAYER_COLOR, 255);
  SDL_RenderDrawRect(renderer, &last1);

  SDL_Event e;
  while (1) {
    SDL_PollEvent(&e);
    if (SDL_KEYDOWN == e.type) {

      switch (e.key.keysym.sym) {

#define la_macron(MOVE)                                                        \
  mov_arg(&DstPos, &ActPos, MOVE);                                             \
  if (is_legal_position((void *)&DstPos)) {                                    \
    ActPos = DstPos;                                                           \
    MoveRender(MOVE);                                                          \
  } else {                                                                     \
    printf("GAME OVER\n");                                                     \
    CRASH_GAME("play_map");                                                    \
  }
      case SDLK_RIGHT:
        la_macron(1);
        break;
      case SDLK_LEFT:
        la_macron(3);
        break;
      case SDLK_UP:
        la_macron(0);
        break;
      case SDLK_DOWN:
        la_macron(2);
        break;
      case SDLK_q:
        CRASH_GAME("play_map occasioaned by: SDLK_q");
      }
      /
      SDL_RenderPresent(renderer);
      SDL_Delay(10);
      * La función MoveRender ya hace eso.

    }
    SDL_Delay(10);
  }
}*/
