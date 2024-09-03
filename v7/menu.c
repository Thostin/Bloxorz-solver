/*
 * I already made this program in my smartphone, but
 * I want to do a remake with more functionalities and also
 * make it quit faster (multithreading)
 * */
#include "menu.h"
#include "algorithm.h"
#include "play_bloxorz.h"

#include <glob_var.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
#include "glob_var.h"
#include "lib/algorithm.c"
#include "lib/algorithm.h"
#include "lib/defs.h"
#include "lib/glob_var.c"
*/

#include <algorithm.h>
#include <animate.h>
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

void display_help(void) {
  printf("Options:\n"
         "-h,  --help:      Display this help screen\n"
         "-v,  --verbose:   Print verbose information (almost allways "
         "unusefull)\n"
         "-m,  --moves:     Similar to --verbose, print what moves are "
         "being played\n"
         "-t,  --trash:     Print uneusefull information\n"
         "-s,  --solve:     Just solve the map (find the shortest path)\n"
         "-as, --autosolve: Solve the map and animate it into the window\n"
         "-im, --ilegal-moves: Show all the illegal moves that have been made\n"
         "-f,  --flags:     Enable the use of blocks that modify the map\n"
         "By default, playing the map is set\n");
}

void menu(int argc, char *argv[]) {
  if (argc <= 0) {
    fprintf(stderr, "Wait, what? Why argc <= 0?\n");
    exit(-1);
  }
  if (1 == argc)
    display_help();
  char *options[] = {"--verbose",      "-v",  "--moves", "-m",
                     "--trash",        "-t",  "--solve", "-s",
                     "--autosolve",    "-as", "--help",  "-h",
                     "--ilegal-moves", "-im", "-f",      "--flags"};
  enum Modes mode = PLAY;
  for (int i = 1; i < argc; ++i) {
    if (0 == strcmp(argv[i], options[0]) || 0 == strcmp(argv[i], options[1]))
      FileFlags.verbose = 1;
    else if (0 == strcmp(argv[i], options[2]) ||
             0 == strcmp(argv[i], options[3]))
      FileFlags.moves = 1;
    else if (0 == strcmp(argv[i], options[4]) ||
             0 == strcmp(argv[i], options[5]))
      FileFlags.trash = 1;
    else if (0 == strcmp(argv[i], options[6]) ||
             0 == strcmp(argv[i], options[7]))
      mode = SOLVE;
    else if (0 == strcmp(argv[i], options[8]) ||
             0 == strcmp(argv[i], options[9]))
      mode = AUTOSOLVE;
    else if (0 == strcmp(argv[i], options[10]) ||
             0 == strcmp(argv[i], options[11])) {
      display_help();
      exit(1);
    } else if (0 == strcmp(argv[i], options[12]) ||
               0 == strcmp(argv[i], options[13])) {
      FileFlags.ilegal_moves = 1;
    } else if (0 == strcmp(argv[i], options[14]) ||
               0 == strcmp(argv[i], options[15])) {
      FileFlags.flags = 1;
    }
  }

  init_files();
  switch (mode) {
  case SOLVE:

    if (1 == FileFlags.flags)
      bottom_solve_map(true);
    else
      solve_map(true);
    break;
  case PLAY:
    if (1 == FileFlags.flags)
      bottom_play_map();
    else
      play_map();
    break;
  case AUTOSOLVE:
    solve_map(false);
    animate_map();
  }
}

void bottom_solve_map(Bool delete_path) {
  init_map();
  bottom_thread_args th_aux;

  struct Position pos_init;
  struct Position pos_th;

  pos_init.orientation = 0;
  pos_init.x = start_x;
  pos_init.y = start_y;

  steps_map[start_x * map_size_x + start_y] = 0;
  th_aux.path = nullptr;
  // th_aux.steps = 0;

  struct Path *path1 = malloc(sizeof(struct Path));
  struct Path *path2 = malloc(sizeof(struct Path));
  struct Path *path3 = malloc(sizeof(struct Path));
  struct Path *path4 = malloc(sizeof(struct Path));

  if (!(path1 && path2 && path3 && path4))
    exit(BAD_ALLOC);

  path1->from = path2->from = path3->from = path4->from = nullptr;
  path1->colgados = path2->colgados = path3->colgados = path4->colgados = 4;

  // Raster total RAM usage
  // dinamic_RAM_usage += 4 * sizeof(struct Path);

  /*void *ret1;
  void *ret2;
  void *ret3;
  void *ret4;*/
  // steps_map[start_x * map_size_x + start_y] = 0;

  struct ExtendedPosition pos_arg;
  mov_arg(&pos_th, &pos_init, NORTH);
  fprintf(stdverbose, "(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  pos_arg.Pos = pos_th;
  pos_arg.next = nullptr;
  pos_arg.f.fulval = 0;
  pos_arg.steps = 1;
  if (is_inside_map(pos_th))
    bottom_detect_in_bottom(pos_th, &pos_arg.f);
  undo_flags();
  do_flags(pos_arg.f);

  th_aux.pos = pos_arg;
  path1->move = th_aux.move = NORTH;
  th_aux.path = path1;

  if (bottom_is_legal_position((void *)&pos_arg))
    bottom_mov(&th_aux);
  else
    free(path1);

  /******/

  mov_arg(&pos_th, &pos_init, EAST);
  fprintf(stdverbose, "(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  pos_arg.Pos = pos_th;
  pos_arg.next = nullptr;
  pos_arg.f.fulval = 0;
  pos_arg.steps = 1;
  if (is_inside_map(pos_th))
    bottom_detect_in_bottom(pos_th, &pos_arg.f);
  undo_flags();
  do_flags(pos_arg.f);

  th_aux.pos = pos_arg;
  path2->move = th_aux.move = EAST;
  th_aux.path = path2;

  if (bottom_is_legal_position((void *)&pos_arg))
    bottom_mov(&th_aux);
  else
    free(path2);

  /******/

  mov_arg(&pos_th, &pos_init, SOUTH);
  fprintf(stdverbose, "(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  pos_arg.Pos = pos_th;
  pos_arg.next = nullptr;
  pos_arg.f.fulval = 0;
  pos_arg.steps = 1;
  if (is_inside_map(pos_th))
    bottom_detect_in_bottom(pos_th, &pos_arg.f);
  undo_flags();
  do_flags(pos_arg.f);

  th_aux.pos = pos_arg;
  path3->move = th_aux.move = SOUTH;
  th_aux.path = path3;

  if (bottom_is_legal_position((void *)&pos_arg))
    bottom_mov(&th_aux);
  else
    free(path3);

  /******/

  mov_arg(&pos_th, &pos_init, WEST);
  fprintf(stdverbose, "(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  pos_arg.Pos = pos_th;
  pos_arg.next = nullptr;
  pos_arg.f.fulval = 0;
  pos_arg.steps = 1;
  if (is_inside_map(pos_th))
    bottom_detect_in_bottom(pos_th, &pos_arg.f);
  undo_flags();
  do_flags(pos_arg.f);

  th_aux.pos = pos_arg;
  path4->move = th_aux.move = WEST;
  th_aux.path = path4;

  if (bottom_is_legal_position((void *)&pos_arg))
    bottom_mov(&th_aux);
  else
    free(path4);
  undo_flags();

  /*
  mov_arg(&pos_th, &pos_init, EAST);
  fprintf(stdverbose, "(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  // th_aux.steps = 0;
  path2->move = th_aux.move = EAST;
  th_aux.path = path2;

  pos_arg.Pos = pos_th;

  pos_arg.f.fulval = 0;
  if (is_inside_map(pos_th))
    bottom_detect_in_bottom(pos_th, &pos_arg.f);
  undo_flags();
  do_flags(pos_arg.f);

  th_aux.pos = pos_arg;
  if (bottom_is_legal_position((void *)&pos_arg))
    bottom_mov(&th_aux);
  else
    free(path2);

  / *****

  mov_arg(&pos_th, &pos_init, SOUTH);
  fprintf(stdverbose, "(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  // th_aux.steps = 0;
  path3->move = th_aux.move = SOUTH;
  th_aux.path = path3;

  pos_arg.Pos = pos_th;

  pos_arg.f.fulval = 0;
  if (is_inside_map(pos_th))
    bottom_detect_in_bottom(pos_th, &pos_arg.f);
  undo_flags();
  do_flags(pos_arg.f);

  th_aux.pos = pos_arg;
  if (bottom_is_legal_position((void *)&pos_arg))
    bottom_mov(&th_aux);
  else
    free(path3);

  / *****

  mov_arg(&pos_th, &pos_init, WEST);
  fprintf(stdverbose, "(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  // th_aux.steps = 0;
  path4->move = th_aux.move = WEST;
  th_aux.path = path4;

  pos_arg.Pos = pos_th;

  pos_arg.f.fulval = 0;
  if (is_inside_map(pos_th))
    bottom_detect_in_bottom(pos_th, &pos_arg.f);
  undo_flags();
  do_flags(pos_arg.f);

  th_aux.pos = pos_arg;
  if (bottom_is_legal_position((void *)&pos_arg))
    bottom_mov(&th_aux);
  else
    free(path4);
*/
  /*
  mov_arg(&pos_th, &pos_init, EAST);
  fprintf(stdverbose, "(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  // th_aux.steps = 0;
  path2->move = (th_aux.move = EAST);
  th_aux.pos = pos_th;
  th_aux.path = path2;

  pos_arg.pos_arg = pos_th;
  if (is_legal_position((void *)&pos_arg))
    mov(&th_aux);
  else
    free(path2);

  mov_arg(&pos_th, &pos_init, SOUTH);
  fprintf(stdverbose, "(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  th_aux.steps = 0;
  path3->move = (th_aux.move = SOUTH);
  th_aux.pos = pos_th;
  th_aux.path = path3;

  pos_arg.pos_arg = pos_th;
  if (is_legal_position((void *)&pos_arg))
    mov(&th_aux);
  else
    free(path3);

  mov_arg(&pos_th, &pos_init, WEST);
  fprintf(stdverbose, "(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  th_aux.steps = 0;
  path4->move = th_aux.move = WEST;
  th_aux.pos = pos_th;
  th_aux.path = path4;

  pos_arg.pos_arg = pos_th;
  if (is_legal_position((void *)&pos_arg))
    mov(&th_aux);
  else
    free(path4);
*/
  extern unsigned int TotalSteps;

  if (current_shortest_path) {
    // printf("ret3: %p\n", ret3);
    char *strs[] = {"UP", "RIGHT", "DOWN", "LEFT"};
    fprintf(stdout, "SHORTEST PATH:\n");
    struct Path *path_aux = current_shortest_path;
    while (nullptr != path_aux) {
      ++TotalSteps;
      fprintf(stderr, "%s ", strs[path_aux->move]);
      path_aux = path_aux->from;
    }
    if (true == delete_path)
      borrar_path(current_shortest_path);
    putc(10, stderr);
  } else {
    fprintf(stdout, "No paths found so far\n");
    fprintf(stdout, "current_shortest_path: %p\n\n",
            (void *)current_shortest_path);
  }

  fprintf(stdout, "Total steps: %d\n", TotalSteps);
  fprintf(stdtrash, "dinamic_RAM_usage: %zu bytes\n\n", dinamic_RAM_usage);
  if (true == delete_path)
    free(map);
  free(steps_map);
  // TODO
}

void solve_map(Bool delete_path) {
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
  fprintf(stdverbose, "(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

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
  fprintf(stdverbose, "(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  th_aux.steps = 0;
  path2->move = (th_aux.move = EAST);
  th_aux.pos = pos_th;
  th_aux.path = path2;

  pos_arg.pos_arg = pos_th;
  if (is_legal_position((void *)&pos_arg))
    mov(&th_aux);
  else
    free(path2);

  mov_arg(&pos_th, &pos_init, SOUTH);
  fprintf(stdverbose, "(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  th_aux.steps = 0;
  path3->move = (th_aux.move = SOUTH);
  th_aux.pos = pos_th;
  th_aux.path = path3;

  pos_arg.pos_arg = pos_th;
  if (is_legal_position((void *)&pos_arg))
    mov(&th_aux);
  else
    free(path3);

  mov_arg(&pos_th, &pos_init, WEST);
  fprintf(stdverbose, "(%u, %u, %u)\n", pos_th.orientation, pos_th.x, pos_th.y);

  th_aux.steps = 0;
  path4->move = th_aux.move = WEST;
  th_aux.pos = pos_th;
  th_aux.path = path4;

  pos_arg.pos_arg = pos_th;
  if (is_legal_position((void *)&pos_arg))
    mov(&th_aux);
  else
    free(path4);

  extern unsigned int TotalSteps;

  if (current_shortest_path) {
    // printf("ret3: %p\n", ret3);
    char *strs[] = {"UP", "RIGHT", "DOWN", "LEFT"};
    fprintf(stdout, "SHORTEST PATH:\n");
    struct Path *path_aux = current_shortest_path;
    while (nullptr != path_aux) {
      ++TotalSteps;
      fprintf(stderr, "%s ", strs[path_aux->move]);
      path_aux = path_aux->from;
    }
    if (true == delete_path)
      borrar_path(current_shortest_path);
    putc(10, stderr);
  } else {
    fprintf(stdout, "No paths found so far\n");
    fprintf(stdout, "current_shortest_path: %p\n\n",
            (void *)current_shortest_path);
  }

  fprintf(stdout, "Total steps: %d\n", TotalSteps);
  fprintf(stdtrash, "dinamic_RAM_usage: %zu bytes\n\n", dinamic_RAM_usage);
  if (true == delete_path)
    free(map);
  free(steps_map);
  // TODO
}
