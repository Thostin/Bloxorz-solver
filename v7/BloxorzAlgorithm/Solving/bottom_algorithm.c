#include "algorithm.h"
#include "defs.h"
#include "glob_var.h"
#include "screen.h"

#include <SDL_pixels.h>
#include <SDL_render.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct ExtendedPosition root = {{0, 0, 0}, {0}, 255, nullptr};
// struct ExtendedPosition *act_ext_pos = &root;

// Checks if it is a legal amount of steps, and stores a new record either in
// steps_map or in the list of extended positions

static inline int TwoPosAreEqual(const struct Position *a,
                                 const struct Position *b) {
  if (a->x == b->x && a->y == b->y && a->orientation == b->orientation)
    return 1;
  return 0;
}

#define UNKNOWN_ERROR 69

int PushExtendedPosition(const struct ExtendedPosition *sample,
                         struct ExtendedPosition *node) {
  struct ExtendedPosition *aux;
  if (nullptr == node) {
    fprintf(stderr, "Why is node == nullptr in PushExtendedPosition\n");
    exit(UNKNOWN_ERROR);
  }
  while (nullptr != node) {
    if (sample->f.fulval == node->f.fulval &&
        TwoPosAreEqual(&sample->Pos, &node->Pos)) {
      if (sample->steps < node->steps) {
        node->steps = sample->steps;
        return 1;
      } else
        return 0;
    }
    aux = node;
    node = node->next;
  }

  struct ExtendedPosition *nuevo = malloc(sizeof(struct ExtendedPosition));
  if (nullptr == nuevo) {
    perror("malloc");
    exit(BAD_ALLOC);
  }
  __builtin_memcpy(nuevo, sample, sizeof(struct ExtendedPosition));

  /* Primera revisión, me olvidé de esto: */
  aux->next = nuevo;
  /* Primer error en primera revisión */

  // Recontra importante:
  nuevo->next = nullptr;
  /*
  act_ext_pos->next = nuevo;
  act_ext_pos = nuevo;
*/
  return 1;
}

int check_steps(const struct ExtendedPosition *nodo) {
  if (nodo->steps >= current_winning_path_steps)
    return 0;

  struct Position pos_arg = nodo->Pos;
  if (0 == nodo->f.fulval) {
    if (steps_map[map_size_x * map_size_y * (pos_arg.orientation) +
                  pos_arg.x * map_size_x + pos_arg.y] > nodo->steps) {
      steps_map[map_size_x * map_size_y * (pos_arg.orientation) +
                pos_arg.x * map_size_x + pos_arg.y] = nodo->steps;
      return 1;
    } else
      return 0;
  }

  return PushExtendedPosition(nodo, &root);
}

int bottom_is_legal_position(const void *_args) {
  fprintf(stdverbose, "\n$$$$$$$$$$$$$$$$$$$$$$$\n");
  const struct ExtendedPosition *args = (struct ExtendedPosition *)_args;
  const struct Position pos_arg = args->Pos;

  if (!is_inside_map(pos_arg))
    return 0;

  /*
  fprintf(stdverbose, "\n\nSTEPS ALREADY: %ld\n\n",
          steps_map[map_size_x * map_size_y * (pos_arg.orientation) +
                    pos_arg.x * map_size_x + pos_arg.y]);
  */
  if (!check_steps(args))
    return 0;
  /*
  if (steps_map[map_size_x * map_size_y * (pos_arg.orientation) +
                pos_arg.x * map_size_x + pos_arg.y] <= args->steps ||
      current_winning_path_steps <= args->steps) {
    fprintf(stdverbose, "Too much steps\n");
    fprintf(stdverbose, "----------------------------------------\n\n");
    return 0;
  }*/

  /*
  steps_map[map_size_x * map_size_y * (pos_arg.orientation) +
            pos_arg.x * map_size_x + pos_arg.y] = args->steps;*/

  // steps_map[map_size_x * map_size_y * (pos_arg.orientation) +
  //         pos_arg.x * map_size_x + pos_arg.y] = args->steps;

  // Check if legal position
  unsigned block_map1; // , block_map2;

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
      current_winning_path_steps = args->steps;
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

void *bottom_mov(const void *_args) {
  fprintf(stdverbose, "###############################\n");

  struct Position pos_arg;
  const bottom_thread_args *args = _args;
  fprintf(stdmoves, "MOVE TO %d                      *\n", args->move);
  fprintf(stdverbose, "STEPS: %ld\n", (args->pos.steps));

  pos_arg = args->pos.Pos;

  fprintf(stdtrash, "XDDDDD\n");

  bottom_thread_args aux = *args;
  struct Path *path1, *path2, *path3, *path4;

  struct ExtendedPosition aux_pos;
#define la_macro_de_la_muerte(CARDINAL, el_path)                                 \
  mov_arg(&pos_arg, &(args->pos.Pos), CARDINAL);                                 \
  aux.pos.Pos = pos_arg;                                                         \
  aux_pos.Pos = pos_arg;                                                         \
  aux_pos.steps = aux.pos.steps;                                                 \
  if (is_inside_map(pos_arg))                                                    \
    bottom_detect_in_bottom(pos_arg, &aux.pos.f);                                \
  undo_flags();                                                                  \
  do_flags(args->pos.f);                                                         \
  aux_pos.f = aux.pos.f;                                                         \
  is_legal = bottom_is_legal_position((void *)&aux_pos);                         \
  if (0 != is_legal) {                                                           \
    ++cantidad_legales;                                                          \
    if ((el_path = malloc(sizeof(struct Path))) == nullptr)                      \
      exit(BAD_ALLOC);                                                           \
    el_path->from = args->path;                                                  \
    el_path->move = aux.move = CARDINAL;                                         \
                                                                                 \
    switch (is_legal) {                                                          \
    case 1:                                                                      \
      aux.path = el_path;                                                        \
      /* Segunda revisión, estaba así: el_path->colgados = 3, PERO DEBE SER UN \
       *  NO UN 3*/                                                              \
      el_path->colgados = 4;                                                     \
      bottom_mov((void *)&aux);                                                  \
      break;                                                                     \
    case 2:                                                                      \
      printf("Borrando current_shortest_path:\n");                               \
      borrar_path(current_shortest_path);                                        \
      (args->path)->colgados = 1;                                                \
      current_shortest_path = el_path;                                           \
      el_path->colgados = 1;                                                     \
      return nullptr;                                                            \
    }                                                                            \
  } else {                                                                       \
    fprintf(stdilegalmoves, "Posición ilegal: (%d, %d, %d) Pasos: %d\n",         \
            aux_pos.Pos.orientation, aux_pos.Pos.y, aux_pos.Pos.x,               \
            (int)aux.pos.steps);                                                 \
    printf("Borrando path superior:\n");                                         \
    borrar_path(args->path);                                                     \
  }

  ++(aux.pos.steps);
  int is_legal;
  int cantidad_legales = 0;
  la_macro_de_la_muerte(NORTH, path1);
  la_macro_de_la_muerte(EAST, path2);
  la_macro_de_la_muerte(WEST, path3);
  la_macro_de_la_muerte(SOUTH, path4);

  return nullptr;
}
