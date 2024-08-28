/*
 * I already made this program in my smartphone, but
 * I want to do a remake with more functionalities and also
 * make it quit faster (multithreading)
 * */
#include "menu.h"

#include <stdio.h>
#include <stdlib.h>

/*
#include "glob_var.h"
#include "lib/algorithm.c"
#include "lib/algorithm.h"
#include "lib/defs.h"
#include "lib/glob_var.c"
*/

#include <algorithm.h>
#include <play_bloxorz.h>

// #include <play_bloxorz.h>

/*
#include "algorithm.c"
#include "defs.h"
#include "glob_var.c"
*/

/*
void init_map(void) {
  type_coordinates x;
  type_coordinates y;

  printf("MAP INITIALIZER\n"
         "Enter the following:\n"
         "_x _y <first row> <second row> <third row> ...\n");

  if (!scanf(READ_TEMPLATE_COORDINATES, &x) || x < INF_LIM ||
      !scanf(READ_TEMPLATE_COORDINATES, &y) || y < INF_LIM) {
    printf("Improper input _a\n");
    exit(1);
  }

  map = malloc(x * y * sizeof(block_t));
  if (nullptr == map)
    exit(BAD_ALLOC);

  block_t aux;
  for (int i = 0, j; i < x; ++i) {
    for (j = 0; j < y; ++j) {
      if (!scanf(READ_TEMPLATE_BLOCK, &aux)) {
        printf("Improper input _b\n");
        exit(1);
      }
      if (aux == 2) {
        if (true == defined_start_point) {
          printf("Redefinition of starting point\n");
          exit(1);
        } else {
          defined_start_point = true;
          start_x = i;
          start_y = j;
        }
      } else if (aux == 3) {
        if (defined_winning_point == true) {
          printf("Redefinition of winning point\n");
          exit(1);
        }
        defined_winning_point = true;
      }

      map[i * y + j] = aux;
    }
  }

  if (false == defined_start_point) {
    printf("Undefined starting point\n");
    exit(1);
  }
  if (false == defined_winning_point) {
    printf("Undefined winnning point\n");
    exit(1);
  }

  steps_map = malloc(3 * x * y * sizeof(steps_count_t));
  if (nullptr == steps_map)
    exit(BAD_ALLOC);

  __builtin_memset(steps_map, 255, 3 * x * y * sizeof(steps_count_t));

  // x represents the amount of rows, but map_size_x represents the width of the
  // grid similarly for map_size_y
  map_size_x = y;
  map_size_y = x;

  printf("map_size_x: %d\n", map_size_x);
  printf("map_size_y: %d\n", map_size_y);

  printf("MAP READED:\n");
  for (int i = 0; i < x; ++i) {
    for (int j = 0; j < y; ++j) {
      printf("%d ", map[i * map_size_x + j]);
    }
    putchar(10);
  }
}
*/

void menu(enum Modes mode) {
  switch (mode) {
  case SOLVE:
    solve_map();
    break;
  case PLAY:
    play_map();
  }
}

void solve_map(void) {
  init_map();
  thread_args th_aux;
  struct Position pos_init;
  struct Position pos_th;

  pos_init.orientation = 0;
  pos_init.x = start_x;
  pos_init.y = start_y;

  th_aux.path = nullptr;
  th_aux.steps = 0;

  struct Path *path1 = malloc(sizeof(struct Path));
  struct Path *path2 = malloc(sizeof(struct Path));
  struct Path *path3 = malloc(sizeof(struct Path));
  struct Path *path4 = malloc(sizeof(struct Path));

  if (!(path1 && path2 && path3 && path4))
    exit(BAD_ALLOC);

  path1->from = path2->from = path3->from = path4->from = nullptr;
  path1->colgados = path2->colgados = path3->colgados = path4->colgados = 3;

  // Raster total RAM usage
  dinamic_RAM_usage += 4 * sizeof(struct Path);

  /*void *ret1;
  void *ret2;
  void *ret3;
  void *ret4;*/
  // steps_map[start_x * map_size_x + start_y] = 0;

  args_IsLegalPosition pos_arg;
  pos_arg.steps = 0;

  mov_arg(&pos_th, &pos_init, NORTH);
  printf("(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  th_aux.steps = 0;
  path1->move = th_aux.move = NORTH;
  th_aux.pos = pos_th;
  th_aux.path = path1;

  pos_arg.pos_arg = pos_th;

  if (is_legal_position((void *)&pos_arg))
    mov(&th_aux);
  else
    free(path1);

  mov_arg(&pos_th, &pos_init, EAST);
  printf("(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  path2->move = (th_aux.move = EAST);
  th_aux.pos = pos_th;
  th_aux.path = path2;

  pos_arg.pos_arg = pos_th;
  if (is_legal_position((void *)&pos_arg))
    mov(&th_aux);
  else
    free(path2);

  mov_arg(&pos_th, &pos_init, SOUTH);
  printf("(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  path3->move = (th_aux.move = SOUTH);
  th_aux.pos = pos_th;
  th_aux.path = path3;

  pos_arg.pos_arg = pos_th;
  if (is_legal_position((void *)&pos_arg))
    mov(&th_aux);
  else
    free(path3);

  mov_arg(&pos_th, &pos_init, WEST);
  printf("(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  path4->move = th_aux.move = WEST;
  th_aux.pos = pos_th;
  th_aux.path = path4;

  pos_arg.pos_arg = pos_th;
  if (is_legal_position((void *)&pos_arg))
    mov(&th_aux);
  else
    free(path4);

  unsigned int TotalSteps = 0;

  if (current_shortest_path) {
    // printf("ret3: %p\n", ret3);
    char *strs[] = {"UP", "RIGHT", "DOWN", "LEFT"};
    fprintf(stderr, "SHORTEST PATH:\n");
    struct Path *path_aux = current_shortest_path;
    while (nullptr != path_aux) {
      ++TotalSteps;
      fprintf(stderr, "%s ", strs[path_aux->move]);
      path_aux = path_aux->from;
    }
    borrar_path(current_shortest_path);
    putc(10, stderr);
  } else {
    printf("No paths found so far\n");
    printf("current_shortest_path: %p\n\n", (void *)current_shortest_path);
  }

  fprintf(stderr, "Total steps: %d\n", TotalSteps);
  printf("dinamic_RAM_usage: %zu bytes\n\n", dinamic_RAM_usage);
  free(map);
  free(steps_map);
  // TODO
}
