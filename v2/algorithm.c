#include "glob_var.c"
// #include <iso646.h>
#include <bits/pthreadtypes.h>
#include <stdint.h>
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

struct Position {
  unsigned orientation : 2;
  type_coordinates x;
  type_coordinates y;
};

// int *map;

struct Thread_args {
  // NORTH 0, EAST 1, SOUTH 2, WEAST 3

  struct Path *path;

  steps_count_t steps;
  struct Position pos;
  unsigned move : 2;
};

void mov_arg(struct Position *const restrict to_ptr,
             const struct Position *restrict from_ptr, const int8_t move) {
  const int hash[7] = {1, 2, 3, 5, 7, 11, 13};
  const int pos_hash = hash[from_ptr->orientation] * hash[move + 3];

  // const int x_y = map_size_y * map_size_x;
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

/*
 * Returns 0 for illegal position, 1 for legal position, and 2 for WINNING
 * position*/
int is_legal(const struct Position pos_arg, const steps_count_t steps) {
  switch (pos_arg.orientation) {
  case 0:
    if (pos_arg.x >= map_size_y || pos_arg.y >= map_size_x) {
      printf("FALLS DOWN THE MAP\n");
      printf("----------------------------------------\n\n");
      return 0;
    }
    break;
  case 1:
    if (pos_arg.x >= map_size_y || pos_arg.x + 1 >= map_size_y ||
        pos_arg.y >= map_size_x) {
      printf("FALLS DOWN THE MAP\n");
      printf("----------------------------------------\n\n");
      return 0;
    }
    break;
  case 2:
    if (pos_arg.x >= map_size_y || pos_arg.y + 1 >= map_size_x ||
        pos_arg.y >= map_size_x) {
      printf("FALLS DOWN THE MAP\n");
      printf("----------------------------------------\n\n");
      return 0;
    }
  }

  if (steps_map[map_size_x * map_size_y * (pos_arg.orientation) +
                pos_arg.x * map_size_x + pos_arg.y] <= steps) {
    printf("Too much steps\n");
    printf("----------------------------------------\n\n");
    return 0;
  }

  unsigned block_map1, block_map2;

  block_map1 = map[map_size_x * pos_arg.x + pos_arg.y];
  printf("\n\nBLOCK_MAP: %d\n\n", block_map1);
  if (block_map1 == VOID) {
    printf("VOID BLOCK MAP\n");
    printf("----------------------------------------\n\n");
    return 0;
  }

  int u;
  switch (pos_arg.orientation) {
  case 0:
    if (block_map1 == 3) {
      printf("NEW WINNING PATH                   $$\n");
      printf("----------------------------------------\n\n");
      return 2;
    }
    if (block_map1 == 4) {
      printf("MUERTE POR VIDRIO\n");
      printf("--------------------------------------\n\n");
      return 0;
    }
    break;
  case 1:

    u = map_size_x * (pos_arg.x + 1) + pos_arg.y;
    if ((block_map1 = map[u]) == VOID) {
      printf("\n\nVOID BLOCK_MAP2: %d | u == %d\n\n", block_map1, u);
      printf("----------------------------------------\n\n");
      return 0;
    }
    break;
  case 2:
    if ((block_map1 = map[map_size_x * pos_arg.x + pos_arg.y + 1]) == VOID) {
      printf("\n\nVOID BLOCK_MAP2: %d\n\n", block_map1);
      printf("----------------------------------------\n\n");
      return 0;
    }
  }
  return 1;
}
/*
void path_deleter(struct Path *path) {
  if (nullptr == path)
    return;
  struct Path *aux = path->from;
  free(path);
  path = aux;
  while (path) {
    if (1 == (path->colgados)) {
      aux = path->from;
      free(path);
      path = aux;
    } else {
      --(path->colgados);
      return;
    }
  }
}
*/
/* Returns nullptr if no paths where found, and not_nullptr otherwise*/
void *mov(const void *_args) {
  printf("###############################\n");

  const thread_args *args = _args;
  const struct Position pos_arg = args->pos;
  printf("MOVE TO %d                      *\n", args->move);
  printf("STEPS: %ld\n", (args->steps));

  printf("(%u, %u, %u)\n", pos_arg.orientation, pos_arg.x, pos_arg.y);

  steps_map[map_size_x * map_size_y * ((args->pos).orientation) +
            (args->pos).x * map_size_x + (args->pos).y] = args->steps;

  thread_args aux = *args;

  struct Path *path1 = nullptr;
  struct Path *path2 = nullptr;
  struct Path *path3 = nullptr;

  // Raster total RAM usage
  dinamic_RAM_usage += 3 * sizeof(struct Path);

  // path1->from = (path2->from = (path3->from = args->path));

  // switch ((args->path)->move) {

  /*
   *
   *
   *
   *
   * */
  /* AT MOST IMPORTANT */
  ++(aux.steps);
  /* AT MOST IMPORTANT */
  /*
   *
   *
   *
   *
   **/

#define mov_arg_template(pos_arg_var, pos_arg, cardinal, cardinal_var)         \
  {                                                                            \
    mov_arg(&pos_arg_var, &pos_arg, cardinal);                                 \
    cardinal_var = cardinal;                                                   \
  }

  //  path_name->colgados = 3;                                                   \

  struct Position pos_arg_var1;
  struct Position pos_arg_var2;
  struct Position pos_arg_var3;

  pos_arg_var1 = pos_arg_var2 = pos_arg_var3 = pos_arg;

  uint8_t cardinal1;
  uint8_t cardinal2;
  uint8_t cardinal3;

  switch (args->move) {
  case NORTH:
    mov_arg_template(pos_arg_var1, pos_arg, NORTH, cardinal1);
    mov_arg_template(pos_arg_var2, pos_arg, EAST, cardinal2);
    mov_arg_template(pos_arg_var3, pos_arg, WEST, cardinal3);
    break;

  case EAST:
    mov_arg_template(pos_arg_var1, pos_arg, EAST, cardinal1);
    mov_arg_template(pos_arg_var2, pos_arg, SOUTH, cardinal2);
    mov_arg_template(pos_arg_var3, pos_arg, NORTH, cardinal3);
    break;

  case SOUTH:
    mov_arg_template(pos_arg_var1, pos_arg, SOUTH, cardinal1);
    mov_arg_template(pos_arg_var2, pos_arg, WEST, cardinal2);
    mov_arg_template(pos_arg_var3, pos_arg, EAST, cardinal3);

    break;
  case WEST:
    mov_arg_template(pos_arg_var1, pos_arg, WEST, cardinal1);
    mov_arg_template(pos_arg_var2, pos_arg, NORTH, cardinal2);
    mov_arg_template(pos_arg_var3, pos_arg, SOUTH, cardinal3);
  }

  void *ret1;
  void *ret2;
  void *ret3;

  ret1 = ret2 = ret3 = nullptr;

  int not_legal = 0;

#define call_mov_args(pos_arg_var, cardinal, path_name, ret_var, aux)          \
  {                                                                            \
    path_name = malloc(sizeof(struct Path));                                   \
    path_name->from = aux.path;                                                \
                                                                               \
    path_name->move = aux.move = cardinal;                                     \
    aux.pos = pos_arg_var;                                                     \
    aux.path = path_name;                                                      \
    ret_var = mov((void *)&aux);                                               \
  }

  switch (is_legal(pos_arg_var1, aux.steps)) {
  case 1:
    call_mov_args(pos_arg_var1, cardinal1, path1, ret1, aux);
    break;
  case 2:
    path1 = malloc(sizeof(struct Path));
    // path1->colgados = 0;
    path1->from = args->path;
    path1->move = cardinal1;

    current_shortest_path = path1;
    /**
     * Hay que hacer que las casillas se pongan de pasos también*/
    return not_nullptr;
  }

  switch (is_legal(pos_arg_var2, aux.steps)) {
  case 1:
    call_mov_args(pos_arg_var2, cardinal2, path2, ret2, aux);
    break;
  case 2:
    path2 = malloc(sizeof(struct Path));
    // path2->colgados = 0;
    path2->from = args->path;
    path2->move = cardinal2;

    current_shortest_path = path2;
    return not_nullptr;
  }

  switch (is_legal(pos_arg_var3, aux.steps)) {
  case 1:
    call_mov_args(pos_arg_var3, cardinal3, path3, ret3, aux);
    break;
  case 2:
    path3 = malloc(sizeof(struct Path));
    // path3->colgados = 0;
    path3->from = args->path;
    path3->move = cardinal3;

    current_shortest_path = path3;
    return not_nullptr;
  }

  printf("RETURN VALUES: (%p, %p, %p)\n", ret1, ret2, ret3);
  printf("----------------------------------------\n\n");

  if (ret1 || ret2 || ret3)
    return not_nullptr;
  return nullptr;
}
