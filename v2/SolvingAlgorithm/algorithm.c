#include "algorithm.h"
#include "defs.h"
#include "glob_var.h"

#include <stdio.h>
#include <stdlib.h>

#define not_nullptr (void *)0x1111

/*
 * Orientations:
 *
 * 0: #
 *
 * 1: #
 *    #
 *
 * 2: ##
 */

void mov_arg(struct Position *const restrict to_ptr,
             const struct Position *restrict from_ptr, const int8_t move) {
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

int is_legal_position(const void *_args) {
  struct Position pos_arg;
  const thread_args *args = _args;

  pos_arg = args->pos;
  printf("(%u, %u, %u)\n", pos_arg.orientation, pos_arg.x, pos_arg.y);

  switch (pos_arg.orientation) {
  case 0:
    if ((args->pos).x >= map_size_y || (args->pos).y >= map_size_x) {
      printf("FALLS DOWN THE MAP\n");
      printf("----------------------------------------\n\n");
      return 0;
    }
    break;
  case 1:
    if ((args->pos).x >= map_size_y || (args->pos).x + 1 >= map_size_y ||
        (args->pos).y >= map_size_x) {
      printf("FALLS DOWN THE MAP\n");
      printf("----------------------------------------\n\n");
      return 0;
    }
    break;
  case 2:
    if ((args->pos).x >= map_size_y || (args->pos).y + 1 >= map_size_x ||
        (args->pos).y >= map_size_x) {
      printf("FALLS DOWN THE MAP\n");
      printf("----------------------------------------\n\n");
      return 0;
    }
  }

  printf("\n\nSTEPS ALREADY: %ld\n\n",
         steps_map[map_size_x * map_size_y * ((args->pos).orientation) +
                   (args->pos).x * map_size_x + (args->pos).y]);
  if (steps_map[map_size_x * map_size_y * ((args->pos).orientation) +
                (args->pos).x * map_size_x + (args->pos).y] <= args->steps) {
    printf("Too much steps\n");
    printf("----------------------------------------\n\n");
    return 0;
  }

  steps_map[map_size_x * map_size_y * ((args->pos).orientation) +
            (args->pos).x * map_size_x + (args->pos).y] = args->steps;

  // Check if legal position
  unsigned block_map1; // block_map2;

  block_map1 = map[map_size_x * (args->pos).x + (args->pos).y];
  printf("\n\nBLOCK_MAP: %d\n\n", block_map1);
  if (block_map1 == VOID) {
    printf("VOID BLOCK MAP\n");
    printf("----------------------------------------\n\n");
    return 0;
  }

  int u;
  switch ((args->pos).orientation) {
  case 0:
    if (block_map1 == 3) {
      printf("NEW WINNING PATH                   $$\n");
      printf("----------------------------------------\n\n");
      // current_shortest_path = args->path;
      return 2;
    }
    if (block_map1 == 4) {
      printf("MUERTE POR VIDRIO\n");
      printf("--------------------------------------\n\n");
      return 0;
    }
    break;
  case 1:

    u = map_size_x * ((args->pos).x + 1) + (args->pos).y;
    if ((block_map1 = map[u]) == VOID) {
      printf("\n\nVOID BLOCK_MAP2: %d | u == %d\n\n", block_map1, u);
      printf("----------------------------------------\n\n");
      return 0;
    }
    break;
  case 2:
    if ((block_map1 = map[map_size_x * (args->pos).x + (args->pos).y + 1]) ==
        VOID) {
      printf("\n\nVOID BLOCK_MAP2: %d\n\n", block_map1);
      printf("----------------------------------------\n\n");
      return 0;
    }
  }

  return 1;
}

void borrar_path(struct Path *path) {
  if (nullptr == path || --(path->colgados) > 0)
    return;
  struct Path *aux = nullptr;
  do {
    aux = path->from;
    free(path);
    path = aux;
  } while (nullptr != aux && 0 == --(aux->colgados));
  aux = nullptr;
  path = nullptr;
}

void *mov(const void *_args) {
  printf("###############################\n");

  struct Position pos_arg;
  const thread_args *args = _args;
  printf("MOVE TO %d                      *\n", args->move);
  printf("STEPS: %ld\n", (args->steps));

  pos_arg = args->pos;
  steps_map[map_size_x * map_size_y * ((args->pos).orientation) +
            (args->pos).x * map_size_x + (args->pos).y] = args->steps;

  thread_args aux = *args;
  struct Path *path1, *path2, *path3;

#define la_macro_de_la_muerte(CARDINAL, el_path)                               \
  mov_arg(&pos_arg, &(args->pos), CARDINAL);                                   \
  aux.pos = pos_arg;                                                           \
  is_legal = is_legal_position((void *)&aux);                                  \
  if (0 != is_legal) {                                                         \
    ++cantidad_legales;                                                        \
    if ((el_path = malloc(sizeof(struct Path))) == nullptr)                    \
      exit(BAD_ALLOC);                                                         \
    aux.move = CARDINAL;                                                       \
    el_path->from = args->path;                                                \
    el_path->move = CARDINAL;                                                  \
                                                                               \
    switch (is_legal) {                                                        \
    case 1:                                                                    \
      aux.path = el_path;                                                      \
      el_path->colgados = 3;                                                   \
      mov((void *)&aux);                                                       \
      break;                                                                   \
    case 2:                                                                    \
      borrar_path(current_shortest_path);                                      \
      (args->path)->colgados = 1;                                              \
      current_shortest_path = el_path;                                         \
      el_path->colgados = 1;                                                   \
      return nullptr;                                                          \
    }                                                                          \
  } else {                                                                     \
    borrar_path(args->path);                                                   \
  }

  ++(aux.steps);
  int is_legal;
  int cantidad_legales = 0;
  switch (args->move) {
  case NORTH:
    la_macro_de_la_muerte(NORTH, path1);
    la_macro_de_la_muerte(EAST, path2);
    la_macro_de_la_muerte(WEST, path3);
    break;
  case EAST:
    la_macro_de_la_muerte(EAST, path1);
    la_macro_de_la_muerte(SOUTH, path2);
    la_macro_de_la_muerte(NORTH, path3);
    break;
  case SOUTH:
    la_macro_de_la_muerte(SOUTH, path1);
    la_macro_de_la_muerte(WEST, path2);
    la_macro_de_la_muerte(EAST, path3);
    break;
  case WEST:
    la_macro_de_la_muerte(WEST, path1);
    la_macro_de_la_muerte(NORTH, path2);
    la_macro_de_la_muerte(SOUTH, path3);
  }

  /*
   * Encontré un caso en el que esto falla:
   * Si el primer y segundo intento fallan, entonces se le resta.
   * Y S el primer intento acierta pero falla al no encontrar una
   * ruta nueva, entonces restará 1 la función borrar_path, y
   * liberará la memoria a la que apunta args->path, y por ende
   * ya no tiene sentido acceder a ella.
  if (0 == (args->path)->colgados)
    borrar_path((args->path));
*/
  return nullptr;
}
