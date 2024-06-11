#include "glob_var.c"
// #include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

  // Varies from 0 to 4
  type_coordinates x;
  type_coordinates y;
} __attribute__((packed));

// int *map;

struct Thread_args {
  // NORTH 0, EAST 1, SOUTH 2, WEAST 3

  struct Path *path;

  steps_count_t steps;
  struct Position pos;
  unsigned move : 2;
};

// This function just works perfect, does not need any changes
void mov_arg(struct Position *const restrict to_ptr,
             const struct Position *restrict from_ptr, const int8_t move) {
  static const int hash[7] = {1, 2, 3, 5, 7, 11, 13};
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

int is_legal_position(struct Position pos) {
  switch (pos.orientation) {
  case 0:
    if (pos.x >= map_size_y || pos.y >= map_size_x) {
      /*
    printf("FALLS DOWN THE MAP\n");
    printf("----------------------------------------\n\n");
    */
      return 0;
    }
    break;
  case 1:
    if (pos.x >= map_size_y || pos.x + 1 >= map_size_y || pos.y >= map_size_x) {
      /*
    printf("FALLS DOWN THE MAP\n");
    printf("----------------------------------------\n\n");
    */
      return 0;
    }
    break;
  case 2:
    if (pos.x >= map_size_y || pos.y + 1 >= map_size_x || pos.y >= map_size_x) {
      /*
    printf("FALLS DOWN THE MAP\n");
    printf("----------------------------------------\n\n");
    */
      return 0;
    }
  }
  // Should give a warning here, but it allways falls in these three cases
}

int is_legal_block(struct Position pos) {
  // Check if legal position
  static unsigned block_map1, block_map2;

  block_map1 = map[map_size_x * pos.x + pos.y];
  printf("\n\nBLOCK_MAP: %d\n\n", block_map1);
  if (block_map1 == VOID) {
    printf("VOID BLOCK MAP\n");
    printf("----------------------------------------\n\n");
    return 0;
  }

  static int u;
  switch (pos.orientation) {
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

    u = map_size_x * (pos.x + 1) + pos.y;
    if ((block_map1 = map[u]) == VOID) {
      printf("\n\nVOID BLOCK_MAP2: %d | u == %d\n\n", block_map1, u);
      printf("----------------------------------------\n\n");
      return 0;
    }
    break;
  case 2:
    if ((block_map1 = map[map_size_x * pos.x + pos.y + 1]) == VOID) {
      printf("\n\nVOID BLOCK_MAP2: %d\n\n", block_map1);
      printf("----------------------------------------\n\n");
      return 0;
    }
  }
  return 1;
}

/*
 * Return nullptr for illegal position*/
void *mov(const void *_args) {
  // printf("###############################\n");
  /*
  printf("map_size_x:%d\n", map_size_x);
  printf("map_size_y:%d\n", map_size_y);*/

  struct Position pos_arg;
  const thread_args *args = _args;
  pos_arg = args->pos;

  /*
  printf("MOVE TO %d                      *\n", args->move);
  printf("STEPS: %ld\n", (args->steps));

  printf("(%u, %u, %u)\n", pos_arg.orientation, pos_arg.x, pos_arg.y);
  */

  if (0 == is_legal_position(pos_arg))
    return nullptr;

  /*
    printf("Too much steps\n");
    printf("----------------------------------------\n\n");
 */
  if (steps_map[map_size_x * map_size_y * ((args->pos).orientation) +
                (args->pos).x * map_size_x + (args->pos).y] <= args->steps) {
    return nullptr;
  }
  steps_map[map_size_x * map_size_y * ((args->pos).orientation) +
            (args->pos).x * map_size_x + (args->pos).y] = args->steps;

  /*
  printf("\n\nSTEPS ALREADY: %ld\n\n",
         steps_map[map_size_x * map_size_y * ((args->pos).orientation) +
                   (args->pos).x * map_size_x + (args->pos).y]);
  */
  // Could be more efficient if checked before calling the function

  // int is_legal_block(pos)
  // Check if legal position
  /*
    switch (is_legal_block(args->pos)) {
    case 0:
      return nullptr;
    case 2:
      current_shortest_path = args->path;
      return _args;
    }
  */
  thread_args aux = *args;

  struct Path *path1;
  struct Path *path2;
  struct Path *path3;

  /*
    if (!path1 || !path2 || !path3) {
      perror("malloc");
      exit(1);
    }
  */
  // Raster total RAM usage
  // dinamic_RAM_usage += 3 * sizeof(struct Path);

  // path1->from = (path2->from = (path3->from = args->path));

  void *ret1 = nullptr, *ret2 = nullptr, *ret3 = nullptr;
  int aux_know_legal_block;
  // switch ((args->path)->move) {
#define call_mov_arg(where_arg, TO, _path, arg, _ret)                          \
  {                                                                            \
    mov_arg(&where_arg, &(args->pos), TO);                                     \
    if (is_legal_position(where_arg) &&                                        \
        (aux_know_legal_block = is_legal_block(where_arg))) {                  \
      _path = malloc(sizeof(struct Position));                                 \
      if (nullptr == _path) {                                                  \
        perror("malloc");                                                      \
        exit(1);                                                               \
      }                                                                        \
      dinamic_RAM_usage += sizeof(struct Position);                            \
                                                                               \
      _path->move = TO;                                                        \
      _path->from = args->path;                                                \
      if (2 == aux_know_legal_block) {                                         \
        current_shortest_path = _path;                                         \
        printf("sleeping for some reason...");                                 \
        sleep(10);                                                             \
        putchar(10);                                                           \
        return _args;                                                          \
      }                                                                        \
      arg.move = TO;                                                           \
      arg.pos = where_arg;                                                     \
      arg.path = _path;                                                        \
      /* It has to first check if the position is legal, then create the path, \
       * if it is a winning path, then return, if it isn't a legal path, then  \
       * don't allocate the memory, if the recursive call returns nullptr,     \
       * then free the memory used (teh memory is freed when exiting the       \
       * switch)*/                                                             \
      _ret = mov((void *)&arg);                                                \
      if (nullptr == _ret) {                                                   \
        free(_path);                                                           \
      }                                                                        \
    }                                                                          \
  }

  ++(aux.steps);
  switch (args->move) {
  case NORTH:
    call_mov_arg(pos_arg, NORTH, path1, aux, ret1);
    call_mov_arg(pos_arg, EAST, path2, aux, ret2);
    call_mov_arg(pos_arg, WEST, path3, aux, ret3);
    break;

  case EAST:
    call_mov_arg(pos_arg, EAST, path1, aux, ret1);
    call_mov_arg(pos_arg, SOUTH, path2, aux, ret2);
    call_mov_arg(pos_arg, NORTH, path3, aux, ret3);
    break;

  case SOUTH:
    call_mov_arg(pos_arg, SOUTH, path1, aux, ret1);
    call_mov_arg(pos_arg, WEST, path2, aux, ret2);
    call_mov_arg(pos_arg, EAST, path3, aux, ret3);
    break;
  case WEST:
    call_mov_arg(pos_arg, WEST, path1, aux, ret1);
    call_mov_arg(pos_arg, NORTH, path2, aux, ret2);
    call_mov_arg(pos_arg, SOUTH, path3, aux, ret3);
  }

  printf("RETURN VALUES: (%p, %p, %p)\n", ret1, ret2, ret3);

  printf("----------------------------------------\n\n");

  // return not_nullptr if a new path was found;
  /*
    if (nullptr == ret1)
      free(path1);

    if (nullptr == ret2)
      free(path2);

    if (nullptr == ret3)
      free(path3);
*/
  if (ret1 || ret2 || ret3)
    return _args;

  return nullptr;
}
