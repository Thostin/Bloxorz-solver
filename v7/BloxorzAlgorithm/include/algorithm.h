#pragma once

#include "glob_var.h"
#include <stdint.h>

struct Position;
struct Thread_args;

union FILEFLAGS {
  struct {
    uint8_t state;
  };

  struct {
    unsigned verbose : 1;
    unsigned trash : 1;
    unsigned moves : 1;
    unsigned ilegal_moves : 1;
    unsigned flags : 1;
  };
} __attribute__((packed));

extern union FILEFLAGS FileFlags;

void mov_arg(struct Position *const to_ptr, const struct Position *from_ptr,
             const int8_t move);

int is_legal_position(const void *_args);

void borrar_path(struct Path *path);
void *mov(const void *_args);
void init_map();
void init_files(void);

extern union FlagsFlags ActualRenderFlagState;

struct CoordinateBlock {
  block_t block;
  type_coordinates x, y;
} __attribute__((packed));

#define HACER 0
#define DESTRUIR 1

struct BlockFlag {
  // Aparecer o destruir
  unsigned hacer : 1;

  // On/Off
  unsigned state : 1;

  uint8_t block_cant;
  struct CoordinateBlock *CoordinateBlockArr;
};

extern struct BlockFlag *BlockFlagArr;
extern int LenBlockFlagArr;

struct Action {
  // Activate, deactivate, flip (0, 1, 2)
  unsigned jogo : 2;

  // FlagDescriptor is the number of the flag the is going to be canged
  uint8_t FlagDescriptor;
};

extern struct Action *PossibleActionList;
extern int LenPossibleActionList;

struct BottomAction {
  unsigned activated_permission : 1;
  uint8_t ActionDescriptor;
};

struct Bottom {
  // Nivel de presión: 0 o 1
  unsigned pressure : 1;

  // Holds whether the bottom actually exists in the map
  unsigned def : 1;

  type_coordinates x, y;

  uint8_t LenActionList;
  struct BottomAction *ActionList;
};

extern struct Bottom *ArrBottoms;
extern int LenArrBottoms;

void do_sflag(uint8_t FlagDescriptor);
void do_flags(union FlagsFlags f);
void undo_flags(void);
void RenderIndividualBlock(struct CoordinateBlock block);
void Render_do_sflag(uint8_t FlagDescriptor);
void Render_do_flags();
void Render_undo_flags(void);
void RenderResetFlags(void);

int is_inside_map(const struct Position pos_arg);

void bottom_detect_in_bottom(const struct Position pos_arg,
                             union FlagsFlags *f);

int bottom_is_legal_position(const void *_args);
void *bottom_mov(const void *_args);
