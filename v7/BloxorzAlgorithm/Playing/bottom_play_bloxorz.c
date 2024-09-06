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

#ifndef POR_BOLUDO
#warning "POR_BOLUDO wasn't defined yet"
#define POR_BOLUDO 1729
#endif

void execute_action(uint8_t ActionDescriptor, union FlagsFlags *f) {
  fprintf(stdverbose, "ActionDescriptor in execute_action: %d\n",
          ActionDescriptor);
  if (ActionDescriptor >= LenPossibleActionList) {
    fprintf(stderr,
            "How is ActionDescriptor accesing a non-existent Action?\n");
    exit(POR_BOLUDO);
  }
  switch (PossibleActionList[ActionDescriptor].jogo) {
  case 0:
    switch (PossibleActionList[ActionDescriptor].FlagDescriptor) {
    case 0:
      f->f0 = 0;
      break;
    case 1:
      f->f1 = 0;
      break;
    case 2:
      f->f2 = 0;
      break;
    case 3:
      f->f3 = 0;
      break;
    case 4:
      f->f4 = 0;
      break;
    }
    break;

  case 1:
    switch (PossibleActionList[ActionDescriptor].FlagDescriptor) {
    case 0:
      f->f0 = 1;
      break;
    case 1:
      f->f1 = 1;
      break;
    case 2:
      f->f2 = 1;
      break;
    case 3:
      f->f3 = 1;
      break;
    case 4:
      f->f4 = 1;
      break;
    }
    break;

  case 2:
    switch (PossibleActionList[ActionDescriptor].FlagDescriptor) {
    case 0:
      f->f0 += 1;
      break;
    case 1:
      f->f1 += 1;
      break;
    case 2:
      f->f2 += 1;
      break;
    case 3:
      f->f3 += 1;
      break;
    case 4:
      f->f4 += 1;
      break;
    }
    break;
  default:
    fprintf(stderr, "How did jogo got a value other that 0, 1 or 2?\n");
#define ILLEGAL_VALUE 345
    exit(ILLEGAL_VALUE);
  }
}

void Render_execute_action(uint8_t ActionDescriptor) {
  fprintf(stderr, "ActionDescriptor in Render_execute_action: %d\n",
          ActionDescriptor);
  if (ActionDescriptor >= LenPossibleActionList) {
    fprintf(stderr,
            "How is ActionDescriptor accesing a non-existent Action?\n");
    exit(POR_BOLUDO);
  }
  switch (PossibleActionList[ActionDescriptor].jogo) {
  case 0:
    switch (PossibleActionList[ActionDescriptor].FlagDescriptor) {
    case 0:
      ActualRenderFlagState.f0 = 0;
      break;
    case 1:
      ActualRenderFlagState.f1 = 0;
      break;
    case 2:
      ActualRenderFlagState.f2 = 0;
      break;
    case 3:
      ActualRenderFlagState.f3 = 0;
      break;
    case 4:
      ActualRenderFlagState.f4 = 0;
      break;
    }
    break;

  case 1:
    switch (PossibleActionList[ActionDescriptor].FlagDescriptor) {
    case 0:
      ActualRenderFlagState.f0 = 1;
      break;
    case 1:
      ActualRenderFlagState.f1 = 1;
      break;
    case 2:
      ActualRenderFlagState.f2 = 1;
      break;
    case 3:
      ActualRenderFlagState.f3 = 1;
      break;
    case 4:
      ActualRenderFlagState.f4 = 1;
      break;
    }
    break;

  case 2:
    switch (PossibleActionList[ActionDescriptor].FlagDescriptor) {
    case 0:
      ActualRenderFlagState.f0 += 1;
      break;
    case 1:
      ActualRenderFlagState.f1 += 1;
      break;
    case 2:
      ActualRenderFlagState.f2 += 1;
      break;
    case 3:
      ActualRenderFlagState.f3 += 1;
      break;
    case 4:
      ActualRenderFlagState.f4 += 1;
      break;
    }
    break;
  default:
    fprintf(stderr, "How did jogo got a value other that 0, 1 or 2?\n");
#define ILLEGAL_VALUE 345
    exit(ILLEGAL_VALUE);
  }
}

void execute_bottom(uint8_t BottomDescriptor) {
  if (BottomDescriptor >= LenArrBottoms) {
    fprintf(stderr, "How did BottomDescriptor access an unexistent Bottom?\n");
    exit(POR_BOLUDO);
  }

  fprintf(stdverbose, "LenActionList: %d\n",
          ArrBottoms[BottomDescriptor].LenActionList);

  for (int i = 0; i < ArrBottoms[BottomDescriptor].LenActionList; ++i) {
    Render_execute_action(
        ArrBottoms[BottomDescriptor].ActionList[i].ActionDescriptor);
  }
}

void bottom_execute_bottom(uint8_t BottomDescriptor, union FlagsFlags *f) {
  if (BottomDescriptor >= LenArrBottoms) {
    fprintf(stderr, "How did BottomDescriptor access an unexistent Bottom?\n");
    exit(POR_BOLUDO);
  }

  fprintf(stdverbose, "LenActionList: %d\n",
          ArrBottoms[BottomDescriptor].LenActionList);

  for (int i = 0; i < ArrBottoms[BottomDescriptor].LenActionList; ++i) {
    execute_action(ArrBottoms[BottomDescriptor].ActionList[i].ActionDescriptor,
                   f);
  }
}

void bottom_mov_arg(struct Position *to_ptr,
                    const struct Position *const restrict from_ptr,
                    const int8_t move) {
  const int hash[7] = {1, 2, 3, 5, 7, 11, 13};
  const int pos_hash = hash[from_ptr->orientation] * hash[move + 3];

  *to_ptr = *from_ptr;

  switch (pos_hash) {
  // Case of orientation = 0
  case 5:
    to_ptr->orientation = 1;
    to_ptr->x -= 2;
    break;
  case 7:
    to_ptr->orientation = 2;
    to_ptr->y += 1;
    break;
  case 11:
    to_ptr->orientation = 1;
    to_ptr->x += 1;
    break;
  case 13:
    to_ptr->orientation = 2;
    to_ptr->y -= 2;
    break;

  // Case of orientation = 1
  case 10:
    to_ptr->orientation = 0;
    to_ptr->x -= 1;
    break;
  case 14:
    to_ptr->orientation = 1;
    to_ptr->y += 1;
    break;
  case 22:
    to_ptr->orientation = 0;
    to_ptr->x += 2;
    break;
  case 26:
    to_ptr->orientation = 1;
    to_ptr->y -= 1;
    break;

  // Case orientation = 2
  case 15:
    to_ptr->orientation = 2;
    to_ptr->x -= 1;
    break;
  case 21:
    to_ptr->orientation = 0;
    to_ptr->y += 2;
    break;
  case 33:
    to_ptr->orientation = 2;
    to_ptr->x += 1;
    break;
  case 39:
    to_ptr->orientation = 0;
    to_ptr->y -= 1;
    break;
  }
}

void detect_in_bottom(const struct Position pos_arg) {
  switch (pos_arg.orientation) {
  case 0:
    if (map[map_size_x * pos_arg.x + pos_arg.y] >= 100) {
      execute_bottom(map[map_size_x * pos_arg.x + pos_arg.y] - 100);
    }
    break;
  case 1:
    if (map[map_size_x * pos_arg.x + pos_arg.y] >= 100) {
      if (ArrBottoms[map[map_size_x * pos_arg.x + pos_arg.y] - 100].pressure ==
          0)
        execute_bottom(map[map_size_x * pos_arg.x + pos_arg.y] - 100);
    }
    if (map[map_size_x * (pos_arg.x + 1) + pos_arg.y] >= 100) {
      if (ArrBottoms[map[map_size_x * (pos_arg.x + 1) + pos_arg.y] - 100]
              .pressure == 0)
        execute_bottom(map[map_size_x * (pos_arg.x + 1) + pos_arg.y] - 100);
    }
    break;
  case 2:
    if (map[map_size_x * pos_arg.x + pos_arg.y] >= 100) {
      if (ArrBottoms[map[map_size_x * pos_arg.x + pos_arg.y] - 100].pressure ==
          0)
        execute_bottom(map[map_size_x * pos_arg.x + pos_arg.y] - 100);
    }
    if (map[map_size_x * pos_arg.x + pos_arg.y + 1] >= 100) {
      if (ArrBottoms[map[map_size_x * pos_arg.x + pos_arg.y + 1] - 100]
              .pressure == 0)
        execute_bottom(map[map_size_x * pos_arg.x + pos_arg.y + 1] - 100);
    }
  }
}

void bottom_detect_in_bottom(const struct Position pos_arg,
                             union FlagsFlags *f) {
  fprintf(stdverbose, "pos_arg.orientation in bottom_detect_in_bottom: %d\n",
          pos_arg.orientation);

  switch (pos_arg.orientation) {
  case 0:
    if (map[map_size_x * pos_arg.x + pos_arg.y] >= 100) {
      bottom_execute_bottom(map[map_size_x * pos_arg.x + pos_arg.y] - 100, f);
    }
    break;
  case 1:
    if (map[map_size_x * pos_arg.x + pos_arg.y] >= 100) {
      if (ArrBottoms[map[map_size_x * pos_arg.x + pos_arg.y] - 100].pressure ==
          0)
        bottom_execute_bottom(map[map_size_x * pos_arg.x + pos_arg.y] - 100, f);
    }
    if (map[map_size_x * (pos_arg.x + 1) + pos_arg.y] >= 100) {
      if (ArrBottoms[map[map_size_x * (pos_arg.x + 1) + pos_arg.y] - 100]
              .pressure == 0)
        bottom_execute_bottom(
            map[map_size_x * (pos_arg.x + 1) + pos_arg.y] - 100, f);
    }
    break;
  case 2:
    if (map[map_size_x * pos_arg.x + pos_arg.y] >= 100) {
      if (ArrBottoms[map[map_size_x * pos_arg.x + pos_arg.y] - 100].pressure ==
          0)
        bottom_execute_bottom(map[map_size_x * pos_arg.x + pos_arg.y] - 100, f);
    }
    if (map[map_size_x * pos_arg.x + pos_arg.y + 1] >= 100) {
      if (ArrBottoms[map[map_size_x * pos_arg.x + pos_arg.y + 1] - 100]
              .pressure == 0)
        bottom_execute_bottom(map[map_size_x * pos_arg.x + pos_arg.y + 1] - 100,
                              f);
    }
  }
}

/*
int is_legal_position_no_test_steps(const struct Position pos_arg) {
  fprintf(stdverbose, "\n$$$$$$$$$$$$$$$$$$$$$$$\n");

  if (!is_inside_map(pos_arg))
    return 0;
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
*/

void bottom_play_map() {
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
  bottom_mov_arg(&DstPos, &ActPos, MOVE);                                      \
  detect_in_bottom(DstPos);                                                    \
  Render_undo_flags();                                                         \
  Render_do_flags();                                                           \
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

void bottom_animate_map() {
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

  for (i = 0; i < TotalSteps; ++i) {
    SDL_PollEvent(&e);
    if (SDL_KEYDOWN == e.type) {
#define la_re_macron(MOVE)                                                     \
  bottom_mov_arg(&DstPos, &ActPos, MOVE);                                      \
  detect_in_bottom(DstPos);                                                    \
  Render_undo_flags();                                                         \
  Render_do_flags();                                                           \
  switch (is_legal_position_no_test_steps(DstPos)) {                           \
  case 0:                                                                      \
    printf("GAME OVER\n");                                                     \
    CRASH_GAME("play_map");                                                    \
    break;                                                                     \
  case 1:                                                                      \
    SDL_Log("Rendering Legal Move");                                           \
    ActPos = DstPos;                                                           \
    MoveRender(MOVE, ActPos);                                                  \
    break;                                                                     \
  case 2:                                                                      \
    SDL_Log("Rendering Winning Move");                                         \
    SDL_Log("Steps: %u\n", TotalSteps);                                        \
    ActPos = DstPos;                                                           \
    MoveRender(MOVE, ActPos);                                                  \
    printf("YOU WON!\n");                                                      \
    CRASH_GAME("winning the game");                                            \
  }
      la_re_macron(move_arr[i]);
      // La función MoveRender ya hace eso.
      // SDL_RenderPresent(renderer);
      // SDL_Delay(10);
      // SDL_Delay(10);
    } else if (e.type == SDL_QUIT) {
      CRASH_GAME("Game was intentionally closed by user");
    } else
      --i;
    SDL_Delay(10);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
  CRASH_GAME("opa");
}
