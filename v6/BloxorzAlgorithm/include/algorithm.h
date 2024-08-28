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
