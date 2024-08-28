#include "algorithm.h"
#include "defs.h"
#include "glob_var.h"

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

union FILEFLAGS FileFlags = {(uint8_t)0};

void init_files(void) {
  stdverbose = stdtrash = stdmoves = stdilegalmoves = stdout;

  // xdfile is where is gonna go everything we don't want to see
  // xdfile is not suposed to know if it exists or not
  FILE *xdfile = nullptr;
  if (!(FileFlags.moves && FileFlags.trash && FileFlags.verbose &&
        FileFlags.ilegal_moves)) {
    xdfile = tmpfile();
    if (!FileFlags.verbose)
      stdverbose = xdfile;
    if (!FileFlags.trash)
      stdtrash = xdfile;
    if (!FileFlags.moves)
      stdmoves = xdfile;
    if (!FileFlags.ilegal_moves)
      stdilegalmoves = xdfile;
  }
}

struct CoordinateBlock {
  block_t block;
  type_coordinates x, y;
} __attribute__((packed));

#define HACER 0
#define DESTRUIR 1
struct BlockFlag {
  // Aparecer o destruir
  unsigned hacer : 1;

  unsigned short block_cant;
  struct CoordinateBlock *CoordinateBlockArr;
};

/*
 * Syntaxis for flags that I will make now (27 de agosto de 2024):
 * - Cada flag está por defecto en estado 0 (puede tener dos o más estados).
 * - La función de una flag es declarar la existencia o la inexistencia de algo.
 * - Una flag es cambiada de estado por presionar un botón del mapa, estos
 *   botones pueden ser de varios tipos: principalmente dos: de presión débil y
 * de presión fuerte. Es suficiente con que una parte del jugador toque el botón
 * de presión débil para activarlo, pero es necesario que el jugador esté
 * completamente encima del botón de presión fuerte para activarlo.
 *
 * Syntaxis:
 * Pueden haber n botones, nombrados a, b, c, d, ..., pero en el mapa se
 * enunciarán como números a partir de un cierto rango llamado el límite
 * inferior (nombre xd), Cada botón afecta el estado de una o más flags.
 *
 * Para la lectura de las banderas, se debe especificar su nombre (un número).
 * Para simplificar, solo se especificará si una flag tiene dos estaados.
 * */

/*
 * - El primer número ingresado es la cantidad de flags.
 * - El segundo es qué hará (eliminar o aparecer).
 * - Tercero es a cuántos bloques afectará.
 * - Los siguientes argumentos son las posiciones en x e y de los bloques a
 * modificarse.
 * Si hace aparecer, entonces se ignora el tipo de bloque de CoordinateBlock
 * *arr; Caso contrario se hace aparecer ese bloque.
 *
 * Para verificar una posición legal en caso de que una flag esté ectivada,
 * entonces se reemplaza el tipo de bloque momentáneamente y luego se regresa al
 * estado original.
 * */

struct BlockFlag *BlockFlagArr = nullptr;
int LenBlockFlagArr = 0;

#define LIM_BLOCKFLAGARR 5
#define TOO_MUCH_FLAGS 10
#define POR_BOLUDO 1729
// Si se ingresa una cantidad no positiva para la bandera, entonces se considera
// que no hay banderas
void leer_flags(void) {
  if (1 != scanf(" %d", &LenBlockFlagArr) ||
      LenBlockFlagArr > LIM_BLOCKFLAGARR) {
    fprintf(stderr, "Se ha excedido el límite de flags\n");
    exit(TOO_MUCH_FLAGS);
  }
  if (LenBlockFlagArr <= 0) {
    LenBlockFlagArr = 0;
    return;
  }

  BlockFlagArr = malloc(LenBlockFlagArr * sizeof(struct BlockFlag));
  if (nullptr == BlockFlagArr) {
    perror("malloc");
    exit(BAD_ALLOC);
  }

  int c;
  for (int i = 0; i < 0; ++i) {
    while ((c = getchar()) == ' ' || c == '\n' || c == '\t')
      ;
    switch (c) {
    case 'h':
      BlockFlagArr[i].hacer = HACER;
      break;
    case 'd':
      BlockFlagArr[i].hacer = DESTRUIR;
      break;
    default:
      printf("Opción de acción desconocida: %c", c);
    }

#define LIM_SUP_BLOCK_MODIFY 10
    int BlockCant;
    if (1 != scanf(" %d", &BlockCant) || BlockCant <= 0 ||
        BlockCant >= LIM_SUP_BLOCK_MODIFY) {
      fprintf(stderr, "BlockCant: Impropio\n");
      exit(POR_BOLUDO);
    }

    BlockFlagArr[i].block_cant = BlockCant;
    BlockFlagArr[i].CoordinateBlockArr =
        malloc(BlockCant * sizeof(struct CoordinateBlock));
    if (nullptr == BlockFlagArr[i].CoordinateBlockArr) {
      perror("malloc");
      exit(BAD_ALLOC);
    }

    for (int j = 0; j < BlockCant; ++j) {
      if (1 !=
          scanf(" %hhu",
                (uint8_t *)&(BlockFlagArr[i].CoordinateBlockArr[j].block))) {
        fprintf(stderr,
                "No se leyó algo que tenga sentido en el tipo de bloque\n");
        exit(POR_BOLUDO);
      }
      if (!scanf(READ_TEMPLATE_COORDINATES,
                 &(BlockFlagArr[i].CoordinateBlockArr[j].x)) ||
          BlockFlagArr[i].CoordinateBlockArr[j].x < INF_LIM ||
          !scanf(READ_TEMPLATE_COORDINATES,
                 &(BlockFlagArr[i].CoordinateBlockArr[j].y)) ||
          BlockFlagArr[i].CoordinateBlockArr[j].x < INF_LIM) {
        fprintf(stderr, "Improper input _a\n");
        exit(1);
      }
    }
  }
}

/*
 * Algo que tiene sentido es leer todo el mapa primero y luego leer los botones
 * */
struct Bottom {
  unsigned action : 2;
  type_coordinates x, y;

  // Activate, deactivate, flip (0, 1, 2)
  uint8_t CoordinateDescriptor;
  // Coordinate descriptor is the number of the flag the is going to be canged
};

struct Bottom *ArrBottoms = nullptr;
int LenArrBottoms = 0;

#define LIM_SUP_LENARRBOTTOMS 5
void leer_botones(void) {
  if (nullptr == map || nullptr == steps_map) {
    fprintf(stderr, "Aún no se leyó el mapa??\n");
    exit(POR_BOLUDO);
  }

  if (1 != scanf(" %d", &LenArrBottoms) ||
      LenArrBottoms > LIM_SUP_LENARRBOTTOMS) {
    fprintf(stderr, "Improper input while reading LenArrBottoms\n");
    exit(POR_BOLUDO);
  }

  ArrBottoms = malloc(LenArrBottoms * sizeof(struct Bottom));
  for (int i = 0; i < LenArrBottoms; ++i) {
    if (1 != scanf(READ_TEMPLATE_COORDINATES, &(ArrBottoms[i].x)) ||
        ArrBottoms[i].x >= map_size_y ||
        1 != scanf(READ_TEMPLATE_COORDINATES, &(ArrBottoms[i].y)) ||
        ArrBottoms[i].y >= map_size_x

    ) {
    }
  }
}

void init_map(void) {

  type_coordinates x;
  type_coordinates y;

  fprintf(stdverbose, "MAP INITIALIZER\n"
                      "Enter the following:\n"
                      "_x _y <first row> <second row> <third row> ...\n");

  if (!scanf(READ_TEMPLATE_COORDINATES, &x) || x < INF_LIM ||
      !scanf(READ_TEMPLATE_COORDINATES, &y) || y < INF_LIM) {
    fprintf(stderr, "Improper input _a\n");
    exit(1);
  }

  if (1 == FileFlags.flags) {
    leer_flags();
  }

  map = malloc(x * y * sizeof(block_t));
  if (nullptr == map)
    exit(BAD_ALLOC);

  block_t aux;
  for (int i = 0, j; i < x; ++i) {
    for (j = 0; j < y; ++j) {
      if (!scanf(READ_TEMPLATE_BLOCK, &aux)) {
        fprintf(stderr, "Improper input _b\n");
        exit(1);
      }
      if (aux == 2) {
        if (true == defined_start_point) {
          fprintf(stderr, "Redefinition of starting point\n");
          exit(1);
        } else {
          defined_start_point = true;
          start_x = i;
          start_y = j;
        }
      } else if (aux == 3) {
        if (defined_winning_point == true) {
          fprintf(stderr, "Redefinition of winning point\n");
          exit(1);
        }
        defined_winning_point = true;
      }

      map[i * y + j] = aux;
    }
  }

  if (false == defined_start_point) {
    fprintf(stderr, "Undefined starting point\n");
    exit(1);
  }
  if (false == defined_winning_point) {
    fprintf(stderr, "Undefined winnning point\n");
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

  fprintf(stdverbose, "map_size_x: %d\n", map_size_x);
  fprintf(stdverbose, "map_size_y: %d\n", map_size_y);

  fprintf(stdverbose, "MAP READED:\n");
  for (int i = 0; i < x; ++i) {
    for (int j = 0; j < y; ++j) {
      fprintf(stdverbose, "%d ", map[i * map_size_x + j]);
    }
    putchar(10);
  }
}

void mov_arg(struct Position *to_ptr,
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

int is_legal_position(const void *_args) {
  fprintf(stdverbose, "\n$$$$$$$$$$$$$$$$$$$$$$$\n");
  const args_IsLegalPosition *args = (args_IsLegalPosition *)_args;
  const struct Position pos_arg = args->pos_arg;

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

  fprintf(stdverbose, "\n\nSTEPS ALREADY: %ld\n\n",
          steps_map[map_size_x * map_size_y * (pos_arg.orientation) +
                    pos_arg.x * map_size_x + pos_arg.y]);

  if (steps_map[map_size_x * map_size_y * (pos_arg.orientation) +
                pos_arg.x * map_size_x + pos_arg.y] <= args->steps ||
      current_winning_path_steps <= args->steps) {
    fprintf(stdverbose, "Too much steps\n");
    fprintf(stdverbose, "----------------------------------------\n\n");
    return 0;
  }

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
  fprintf(stdverbose, "###############################\n");

  struct Position pos_arg;
  const thread_args *args = _args;
  fprintf(stdmoves, "MOVE TO %d                      *\n", args->move);
  fprintf(stdverbose, "STEPS: %ld\n", (args->steps));

  pos_arg = args->pos;
  steps_map[map_size_x * map_size_y * (pos_arg.orientation) +
            pos_arg.x * map_size_x + pos_arg.y] = args->steps;
  fprintf(stdtrash, "XDDDDD\n");

  thread_args aux = *args;
  struct Path *path1, *path2, *path3;

  args_IsLegalPosition aux_pos;
#define la_macro_de_la_muerte(CARDINAL, el_path)                               \
  mov_arg(&pos_arg, &(args->pos), CARDINAL);                                   \
  aux.pos = pos_arg;                                                           \
  aux_pos.pos_arg = pos_arg;                                                   \
  aux_pos.steps = aux.steps;                                                   \
  is_legal = is_legal_position((void *)&aux_pos);                              \
  if (0 != is_legal) {                                                         \
    ++cantidad_legales;                                                        \
    if ((el_path = malloc(sizeof(struct Path))) == nullptr)                    \
      exit(BAD_ALLOC);                                                         \
    el_path->from = args->path;                                                \
    el_path->move = aux.move = CARDINAL;                                       \
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
    fprintf(stdilegalmoves, "Posición ilegal: (%d, %d, %d) Pasos: %d\n",       \
            aux_pos.pos_arg.orientation, aux_pos.pos_arg.y, aux_pos.pos_arg.x, \
            (int)aux.steps);                                                   \
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
