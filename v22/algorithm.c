#include "glob_var.c"
// #include <iso646.h>
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
 * Return nullptr for illegal position*/
void *mov(const void *_args) {
  printf("###############################\n");
  /*
  printf("map_size_x:%d\n", map_size_x);
  printf("map_size_y:%d\n", map_size_y);*/

  const thread_args *args = _args;
  const struct Position pos_arg = args->pos;
  printf("MOVE TO %d                      *\n", args->move);
  printf("STEPS: %ld\n", (args->steps));

  printf("(%u, %u, %u)\n", pos_arg.orientation, pos_arg.x, pos_arg.y);

  switch (pos_arg.orientation) {
  case 0:
    if ((args->pos).x >= map_size_y || (args->pos).y >= map_size_x) {
      printf("FALLS DOWN THE MAP\n");
      printf("----------------------------------------\n\n");
      return nullptr;
    }
    break;
  case 1:
    if ((args->pos).x >= map_size_y || (args->pos).x + 1 >= map_size_y ||
        (args->pos).y >= map_size_x) {
      printf("FALLS DOWN THE MAP\n");
      printf("----------------------------------------\n\n");
      return nullptr;
    }
    break;
  case 2:
    if ((args->pos).x >= map_size_y || (args->pos).y + 1 >= map_size_x ||
        (args->pos).y >= map_size_x) {
      printf("FALLS DOWN THE MAP\n");
      printf("----------------------------------------\n\n");
      return nullptr;
    }
  }

  // Could be more efficient if checked before calling the function

  printf("\n\nSTEPS ALREADY: %ld\n\n",
         steps_map[map_size_x * map_size_y * ((args->pos).orientation) +
                   (args->pos).x * map_size_x + (args->pos).y]);
  if (steps_map[map_size_x * map_size_y * ((args->pos).orientation) +
                (args->pos).x * map_size_x + (args->pos).y] <= args->steps) {
    printf("Too much steps\n");
    printf("----------------------------------------\n\n");
    return nullptr;
  }

  steps_map[map_size_x * map_size_y * ((args->pos).orientation) +
            (args->pos).x * map_size_x + (args->pos).y] = args->steps;

  // Check if legal position
  unsigned block_map1, block_map2;

  block_map1 = map[map_size_x * (args->pos).x + (args->pos).y];
  printf("\n\nBLOCK_MAP: %d\n\n", block_map1);
  if (block_map1 == VOID) {
    printf("VOID BLOCK MAP\n");
    printf("----------------------------------------\n\n");
    return nullptr;
  }

  int u;
  switch ((args->pos).orientation) {
  case 0:
    if (block_map1 == 3) {
      printf("NEW WINNING PATH                   $$\n");
      printf("----------------------------------------\n\n");
      current_shortest_path = args->path;
      return not_nullptr;
    }
    if (block_map1 == 4) {
      printf("MUERTE POR VIDRIO\n");
      printf("--------------------------------------\n\n");
      return nullptr;
    }
    break;
  case 1:

    u = map_size_x * ((args->pos).x + 1) + (args->pos).y;
    if ((block_map1 = map[u]) == VOID) {
      printf("\n\nVOID BLOCK_MAP2: %d | u == %d\n\n", block_map1, u);
      printf("----------------------------------------\n\n");
      return nullptr;
    }
    break;
  case 2:
    if ((block_map1 = map[map_size_x * (args->pos).x + (args->pos).y + 1]) ==
        VOID) {
      printf("\n\nVOID BLOCK_MAP2: %d\n\n", block_map1);
      printf("----------------------------------------\n\n");
      return nullptr;
    }
  }

  thread_args aux = *args;

  struct Path *path1 = malloc(sizeof(struct Path));
  struct Path *path2 = malloc(sizeof(struct Path));
  struct Path *path3 = malloc(sizeof(struct Path));

  // Raster total RAM usage
  dinamic_RAM_usage += 3 * sizeof(struct Path);

  path1->from = (path2->from = (path3->from = args->path));

  void *ret1, *ret2, *ret3;

  // switch ((args->path)->move) {
  ++(aux.steps);

  struct Position pos_arg1;
  struct Position pos_arg2;
  struct Position pos_arg3;

#define make_args(pos_args, pos_origin, cardinal_var, cardinal_val)            \
  {                                                                            \
    mov_arg(&pos_args, &pos_origin, cardinal_val);                             \
    path_name->move = aux.move = cardinal;                                     \
    aux.pos = pos_args;                                                        \
    aux.path = path_name;                                                      \
  }

#define call_mov_arg(pospath_name, aux, ret_name, cardinal)                    \
  {                                                                            \
    path_name->move = aux.move = cardinal_val;
  aux.pos = p
}
switch (args->move) {
case NORTH:
  mov_arg(&pos_arg, &(args->pos), NORTH);
  path1->move = (aux.move = NORTH);
  aux.pos = pos_arg;
  aux.path = path1;
  ret1 = mov((void *)&aux);

  mov_arg(&pos_arg, &(args->pos), EAST);
  path2->move = aux.move = EAST;
  aux.pos = pos_arg;
  aux.path = path2;
  ret2 = mov((void *)&aux);

  mov_arg(&pos_arg, &(args->pos), WEST);
  path3->move = aux.move = WEST;
  aux.pos = pos_arg;
  aux.path = path3;
  ret3 = mov((void *)&aux);
  break;

case EAST:
  mov_arg(&pos_arg, &(args->pos), EAST);
  path1->move = aux.move = EAST;
  aux.pos = pos_arg;
  aux.path = path1;

  ret1 = mov((void *)&aux);

  mov_arg(&pos_arg, &(args->pos), SOUTH);
  path2->move = aux.move = SOUTH;
  aux.pos = pos_arg;
  aux.path = path2;

  ret2 = mov((void *)&aux);

  mov_arg(&pos_arg, &(args->pos), NORTH);
  path3->move = (aux.move = NORTH);
  aux.pos = pos_arg;
  aux.path = path3;

  ret3 = mov((void *)&aux);

  break;

case SOUTH:
  mov_arg(&pos_arg, &(args->pos), SOUTH);
  path1->move = (aux.move = SOUTH);
  aux.pos = pos_arg;
  aux.path = path1;

  ret1 = mov((void *)&aux);

  mov_arg(&pos_arg, &(args->pos), WEST);
  path2->move = (aux.move = WEST);
  aux.pos = pos_arg;
  aux.path = path2;

  ret2 = mov((void *)&aux);

  mov_arg(&pos_arg, &(args->pos), EAST);
  path3->move = (aux.move = EAST);
  aux.pos = pos_arg;
  aux.path = path3;

  ret3 = mov((void *)&aux);
  break;
case WEST:
  mov_arg(&pos_arg, &(args->pos), WEST);
  path1->move = aux.move = WEST;
  aux.pos = pos_arg;
  aux.path = path1;

  ret1 = mov((void *)&aux);

  mov_arg(&pos_arg, &(args->pos), NORTH);
  path2->move = aux.move = NORTH;
  aux.pos = pos_arg;
  aux.path = path2;

  ret2 = mov((void *)&aux);

  mov_arg(&pos_arg, &(args->pos), SOUTH);
  path3->move = aux.move = SOUTH;
  aux.pos = pos_arg;
  aux.path = path3;

  ret3 = mov((void *)&aux);
}
printf("RETURN VALUES: (%p, %p, %p)\n", ret1, ret2, ret3);

printf("----------------------------------------\n\n");
if (ret1 || ret2 || ret3)
  return _args;
// return not_nullptr;
return nullptr;
}
