#include "algorithm.h"
#include "defs.h"
#include "glob_var.h"
#include "screen.h"

#include <SDL_render.h>
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
 *
 * (31 de agosto de 2024).
 * Ahora me doy cuenta de que en BlockFlag la bandera 'hacer' es totalmente
 * irrelevante, porque siempre puedo hacer que sea 'HACER' y lo que hace es
 * escribir bloques vacío, que equivalente a 'DESTRUIR'.
 *
 * Pero es más óptimo si simplemente pongo 'DESTRUIR'.
 * */

struct BlockFlag *BlockFlagArr = nullptr;
int LenBlockFlagArr = 0;

#define LIM_BLOCKFLAGARR 5
#define TOO_MUCH_FLAGS 10
#define POR_BOLUDO 1729
// Si se ingresa una cantidad no positiva para la bandera, entonces se considera
// que no hay banderas
void leer_flags(void) {
  fprintf(stdverbose, "\n###Leyendo flags###\n");

  if (nullptr == map || nullptr == steps_map) {
    fprintf(stderr, "Aún no se han leído los mapas??\n");
    exit(POR_BOLUDO);
  }

  if (1 != scanf(" %d", &LenBlockFlagArr) ||
      LenBlockFlagArr > LIM_BLOCKFLAGARR) {
    fprintf(stderr, "Se ha excedido el límite de flags\n");
    exit(TOO_MUCH_FLAGS);
  }

  fprintf(stdverbose, "LenBlockFlagArr: %d\n", LenBlockFlagArr);

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
  int BlockCant;
  for (int i = 0; i < LenBlockFlagArr; ++i) {
    while ((c = getchar()) == ' ' || c == '\n' || c == '\t')
      ;
    switch (c) {
    case 'h':
      BlockFlagArr[i].hacer = HACER;
      fprintf(stdverbose, "HACER\n");
      break;
    case 'd':
      BlockFlagArr[i].hacer = DESTRUIR;
      fprintf(stdverbose, "DESTRUIR\n");
      break;
    default:
      printf("Opción de acción desconocida: %c", c);
      exit(POR_BOLUDO);
    }

#define LIM_SUP_BLOCK_MODIFY 10
    if (1 != scanf(" %d", &BlockCant) || BlockCant <= 0 ||
        BlockCant > LIM_SUP_BLOCK_MODIFY) {
      fprintf(stderr, "BlockCant: Impropio\n");
      exit(POR_BOLUDO);
    }

    fprintf(stdverbose, "BlockCant: %d", BlockCant);

    BlockFlagArr[i].block_cant = BlockCant;
    BlockFlagArr[i].CoordinateBlockArr =
        malloc(BlockCant * sizeof(struct CoordinateBlock));
    if (nullptr == BlockFlagArr[i].CoordinateBlockArr) {
      perror("malloc");
      exit(BAD_ALLOC);
    }

    for (int j = 0; j < BlockCant; ++j) {
      fprintf(stdverbose, "\nReading block\n");
      if (1 !=
          scanf(" %hhu",
                (uint8_t *)&(BlockFlagArr[i].CoordinateBlockArr[j].block))) {
        fprintf(stderr,
                "No se leyó algo que tenga sentido en el tipo de bloque\n");
        exit(POR_BOLUDO);
      }
      fprintf(stdverbose, "Bloque leído: %d\n",
              BlockFlagArr[i].CoordinateBlockArr[j].block);
      if (!scanf(READ_TEMPLATE_COORDINATES,
                 &(BlockFlagArr[i].CoordinateBlockArr[j].x))) {
        fprintf(stderr, "Illegal read in x\n");
        exit(POR_BOLUDO);
      }
      if (BlockFlagArr[i].CoordinateBlockArr[j].x >= map_size_y) {
        fprintf(stderr, "Illegal read in x=%d (exeeded len)\n",
                BlockFlagArr[i].CoordinateBlockArr[j].x);
        exit(POR_BOLUDO);
      }
      fprintf(stdverbose, "x del bloque leído: %d\n",
              BlockFlagArr[i].CoordinateBlockArr[j].x);
      if (!scanf(READ_TEMPLATE_COORDINATES,
                 &(BlockFlagArr[i].CoordinateBlockArr[j].y))) {
        fprintf(stderr, "Illegal read in y\n");
        exit(POR_BOLUDO);
      }
      if (BlockFlagArr[i].CoordinateBlockArr[j].y >= map_size_x) {
        fprintf(stderr, "Illegal read in y (exceded len)\n");
        fprintf(stderr, "Improper input while reading x or y in coordinates of "
                        "the blocks in leer_flags\n");
        exit(POR_BOLUDO);
      }
      fprintf(stdverbose, "y del bloque leído: %d\n",
              BlockFlagArr[i].CoordinateBlockArr[j].y);
    }
  }
}

/*
 * Algo que tiene sentido es leer todo el mapa primero y luego leer los botones
 * */
/*
struct Action {
  // Activate, deactivate, flip (0, 1, 2)
  unsigned jogo : 2;

  // FlagDescriptor is the number of the flag the is going to be canged
  uint8_t FlagDescriptor;
};
*/

struct Action *PossibleActionList = nullptr;

#define LIM_ACTIONS 31
int LenPossibleActionList = 0;

// I need this structure because a bottom can or not have access to make an
// action

/*
 * What if I want a flag to activate / deactivate the possible actions to a
 * Bottom? I think that the best way of doing this is: a bottom can only make
 * one action, and there can be multiple bottoms in the same place, or it can
 * activate and deactivate the possible actions that a bottom can do.
 *
 * But I also think that this can eliminate some posibilities: What if I want an
 * action to be deactivated by default?
 *
 * So that's the problem, therefore it will be also a requirement to set another
 * two informations pieces: If the action is activated by default and if it is
 * activated or deactivated.
 *
 * But there's also another solution: There are flags, there are bottoms, and
 * there are certain actions that a bottom can execute, and the bottom is
 * programed to execute or not the action.
 *
 * A possible advantage of this last one is that There can be multiple bottoms
 * that can make repeated actions, and therefore this will be a more general
 * solution.
 *
 * If I have considered that there can be multiple bottoms on the same block, it
 * will be the same but it will be harder to search what bottoms are in there,
 * may be a linear search into a list or something like that.
 *
 * This is the final conclusion: activate/deactivate a bottom is to just
 * activate/deactivate the actions it can do, but I think it could ran into some
 * optimizations if I let all the options be deactivated by desapearing the
 * bottom. A bottom is repressented in the map as a number beetween 100 and 104
 * (for LIM_SUP_LENARRBOTTOMS = 5) and it can make some actions
 * */

/*
struct Bottom {
  // Nivel de presión: 0 o 1
  unsigned pressure : 1;

  // Holds whether the bottom actually exists in the map
  unsigned def : 1;

  type_coordinates x, y;

  struct BottomAction *ActionList;
};
*/

/* Orden de escritura de datos:
 * - Presión.
 * - Acción.
 * - Coordenadas (x y luego y)
 * - Descritor de flag (número de flag)
 * */
struct Bottom *ArrBottoms = nullptr;
int LenArrBottoms = 0;

// Máxima cantidad de botones
#define LIM_SUP_LENARRBOTTOMS 5

// Desde qué valor se indexarán los botones
#define FROM_INDEX_BOTTOM 100

void leer_acciones(void) {
  fprintf(stdverbose, "\n###Reading Actions###\n");

  int how_many;
  if (1 != scanf(" %d", &how_many)) {
    fprintf(stderr, "Illegal read in how_many in  leer_acciones, how_many=%d\n",
            how_many);
    exit(POR_BOLUDO);
  }
  if (how_many <= 0 || how_many >= LIM_ACTIONS) {
    fprintf(stderr, "Illegal amount in how_many in leer_acciones\n");
    exit(POR_BOLUDO);
  }

  fprintf(stdverbose, "Cantidad de Acciones: %d\n", how_many);
  LenPossibleActionList = how_many;

  PossibleActionList = malloc(LenPossibleActionList * sizeof(struct Action));

  if (nullptr == PossibleActionList) {
    fprintf(stderr, "Illegal read in PossibleActionList in leer_acciones\n");
    exit(POR_BOLUDO);
  }

  uint8_t flag_descriptor, jogo;
  for (int i = 0; i < how_many; ++i) {
    if (1 != scanf(" %hhu", &jogo) || jogo >= 3) {
      fprintf(stderr, "Illegal read in jogo in leer_acciones\n");
      exit(POR_BOLUDO);
    }

    if (1 != scanf(" %hhu", &flag_descriptor) || flag_descriptor < 0 ||
        flag_descriptor >= LenBlockFlagArr) {
      fprintf(stderr, "Invalidate coordinate descriptor\n");
      exit(POR_BOLUDO);
    }

    PossibleActionList[i].jogo = jogo;
    PossibleActionList[i].FlagDescriptor = flag_descriptor;

    fprintf(stdverbose, "Jogo leído: %d\n", jogo);
    fprintf(stdverbose, "flag_descriptor leído: %d\n", flag_descriptor);
  }
}

// A bottom can make at most three actions
#define LIM_ACTION_LEN 3

void leer_botones(void) {
  fprintf(stdverbose, "\n###Reading Bottoms####\n");

  if (nullptr == map || nullptr == steps_map) {
    fprintf(stderr, "Aún no se leyó el mapa??\n");
    exit(POR_BOLUDO);
  }

  if (1 != scanf(" %d", &LenArrBottoms) ||
      LenArrBottoms > LIM_SUP_LENARRBOTTOMS) {
    fprintf(stderr, "Improper input while reading LenArrBottoms\n");
    exit(POR_BOLUDO);
  }

  fprintf(stdverbose, "LenArrBottoms: %d\n", LenArrBottoms);
  ArrBottoms = malloc(LenArrBottoms * sizeof(struct Bottom));

  if (nullptr == ArrBottoms) {
    perror("malloc");
    exit(BAD_ALLOC);
  }

  int bottom_pressure, activated_default;

  for (int i = 0; i < LenArrBottoms; ++i) {
    if (1 != scanf(" %d", &bottom_pressure) || bottom_pressure < 0 ||
        bottom_pressure > 1) {
      fprintf(stderr, "Inválido lo que se escribió en bottom_pressure\n");
      exit(POR_BOLUDO);
    }

    ArrBottoms[i].pressure = bottom_pressure;
    fprintf(stdverbose, "bottom_pressure: %d\n", bottom_pressure);

    if (1 != scanf(READ_TEMPLATE_COORDINATES, &(ArrBottoms[i].x)) ||
        ArrBottoms[i].x >= map_size_y ||
        1 != scanf(READ_TEMPLATE_COORDINATES, &(ArrBottoms[i].y)) ||
        ArrBottoms[i].y >= map_size_x) {
      fprintf(stderr, "Lectura ilegal en x o y en ArrBottoms\n");
      exit(POR_BOLUDO);
    }

    if (1 != scanf(" %d", &activated_default)) {
      fprintf(stderr,
              "Illegal read while reading activated_default in ArrBottoms\n");
      exit(POR_BOLUDO);
    }

    ArrBottoms[i].def = activated_default;
    fprintf(stdverbose, "activated_default: %d\n", activated_default);
    if (activated_default)
      map[map_size_x * ArrBottoms[i].x + ArrBottoms[i].y] = 100 + i;

    uint8_t ActionBottomLen;

    if (1 != scanf(" %hhu", &ActionBottomLen) || ActionBottomLen <= 0 ||
        ActionBottomLen > LIM_ACTION_LEN) {
      fprintf(stderr, "Illegal read in ActionBottomLen in leer_botones\n");
      exit(POR_BOLUDO);
    }

    fprintf(stdverbose, "ActionBottomLen: %hhu\n", ActionBottomLen);

    ArrBottoms[i].ActionList =
        malloc(ActionBottomLen * sizeof(struct BottomAction));

    if (nullptr == ArrBottoms[i].ActionList) {
      perror("malloc");
      exit(BAD_ALLOC);
    }
    ArrBottoms[i].LenActionList = ActionBottomLen;

    unsigned DefaultActionPermitted, ActionDescriptor;
    for (int j = 0; j < ActionBottomLen; ++j) {
      if (1 != scanf(" %d", &DefaultActionPermitted) ||
          DefaultActionPermitted < 0 || DefaultActionPermitted > 1) {
        fprintf(stderr,
                "Illegal read in DefaultActionPermitted in leer_botones\n");
        exit(POR_BOLUDO);
      }
      ArrBottoms[i].ActionList[j].activated_permission = DefaultActionPermitted;
      fprintf(stdverbose, "DefaultActionPermitted: %d", DefaultActionPermitted);

      if (1 != scanf(" %d", &ActionDescriptor) || ActionDescriptor < 0 ||
          ActionDescriptor >= LenPossibleActionList) {
        fprintf(stderr, "Illegal read in ActionDescriptor in leer_botones\n");
        exit(POR_BOLUDO);
      }
      fprintf(stderr, "ActionDescriptor: %d\n", ActionDescriptor);
      ArrBottoms[i].ActionList[j].ActionDescriptor = ActionDescriptor;
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

  if (1 == FileFlags.flags) {
    // Oiko ndaje peteĩ jagua ñarõ rembo oñarõsetéva yvágape
    leer_flags();
    leer_acciones();
    leer_botones();
  }
}

// We can have at most LIM_BLOCKFLAGARR flags so an uint8_t is enought to hold
// what flags are activated Aunque ahora se me ocurre que es medio estúpido
// tener un campo dentro de la flag para saber si está activa, simplemente hay
// que crear una estructura con bit fields que lo indiquen en todo momento??

// Pero hay otra cosa, cada vez que haga la llamada recursiva de la función,
// tendré que guardar el estado de las flags manualmente

struct CoordinateBlock
    __buffer_aux_blocks[LIM_SUP_BLOCK_MODIFY * LIM_BLOCKFLAGARR];
int __flags_i = 0;
int __last_cant = 0;

void do_sflag(uint8_t FlagDescriptor) {
  for (int i = 0; i < BlockFlagArr[FlagDescriptor].block_cant; ++i) {
    __buffer_aux_blocks[__flags_i].block =
        map[BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i].x * map_size_x +
            BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i].y];
    __buffer_aux_blocks[__flags_i].x =
        BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i].x;
    __buffer_aux_blocks[__flags_i].y =
        BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i].y;

    map[BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i].x * map_size_x +
        BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i].y] =
        BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i].block;
    ++__flags_i;
  }
}

void do_flags(union FlagsFlags f) {
  if (f.f0 == 1)
    do_sflag(0);
  if (f.f1 == 1)
    do_sflag(1);
  if (f.f2 == 1)
    do_sflag(2);
  if (f.f3 == 1)
    do_sflag(3);
  if (f.f4 == 1)
    do_sflag(4);
}

void undo_flags(void) {
  for (int i = 0; i < __flags_i; ++i)
    map[__buffer_aux_blocks[i].x * map_size_x + __buffer_aux_blocks[i].y] =
        __buffer_aux_blocks[i].block;
  __flags_i = 0;
}

/* Render versions of the above ones */

void RenderIndividualBlock(struct CoordinateBlock block) {
  SDL_Rect b;
  b.w = b.h = SQUARE_LEN;
  b.x = SQUARE_LEN * block.y;
  b.y = SQUARE_LEN * block.x;

  Color_t aux_color;
  if (block.block >= 100) {
    aux_color.r = BottomColors[0].r;
    aux_color.g = BottomColors[0].g;
    aux_color.b = BottomColors[0].b;
  } else {
    aux_color.r = BlockColors[block.block].r;
    aux_color.g = BlockColors[block.block].g;
    aux_color.b = BlockColors[block.block].b;
  }
  SDL_SetRenderDrawColor(renderer, aux_color.r, aux_color.g, aux_color.b, 255);

  SDL_RenderFillRect(renderer, &b);
}

void Render_do_sflag(uint8_t FlagDescriptor) {
  for (int i = 0; i < BlockFlagArr[FlagDescriptor].block_cant; ++i) {
    __buffer_aux_blocks[__flags_i].block =
        map[BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i].x * map_size_x +
            BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i].y];
    __buffer_aux_blocks[__flags_i].x =
        BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i].x;
    __buffer_aux_blocks[__flags_i].y =
        BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i].y;

    map[BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i].x * map_size_x +
        BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i].y] =
        BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i].block;

    RenderIndividualBlock(BlockFlagArr[FlagDescriptor].CoordinateBlockArr[i]);
    ++__flags_i;
  }
}

union FlagsFlags ActualRenderFlagState = {0};

void Render_do_flags() {
  if (ActualRenderFlagState.f0 == 1)
    Render_do_sflag(0);
  if (ActualRenderFlagState.f1 == 1)
    Render_do_sflag(1);
  if (ActualRenderFlagState.f2 == 1)
    Render_do_sflag(2);
  if (ActualRenderFlagState.f3 == 1)
    Render_do_sflag(3);
  if (ActualRenderFlagState.f4 == 1)
    Render_do_sflag(4);
}

void Render_undo_flags(void) {
  for (int i = 0; i < __flags_i; ++i) {
    map[__buffer_aux_blocks[i].x * map_size_x + __buffer_aux_blocks[i].y] =
        __buffer_aux_blocks[i].block;
    RenderIndividualBlock(__buffer_aux_blocks[i]);
  }
  __flags_i = 0;
}

void RenderResetFlags(void) {
  Render_undo_flags();
  Render_do_flags();
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

// Se supone que is_legal_position no depende de las flags que estén activas,
// pero sí de la cantidad de pasos que puede conllevar estar en una posición con
// flags activas. La idea es crear una lista con "Posiciones extendidas", que si
// una o más flags están activas, entonces se guarda esa posición en la lista y
// no en steps_map.

int is_inside_map(const struct Position pos_arg) {
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
  return 1;
}

int is_legal_position(const void *_args) {
  fprintf(stdverbose, "\n$$$$$$$$$$$$$$$$$$$$$$$\n");
  const args_IsLegalPosition *args = (args_IsLegalPosition *)_args;
  const struct Position pos_arg = args->pos_arg;

  if (!is_inside_map(pos_arg))
    return 0;
  /*
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
  */

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
    path->from = nullptr;
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

  return nullptr;
}
