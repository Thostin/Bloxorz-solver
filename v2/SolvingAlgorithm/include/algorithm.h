#pragma once

#include "glob_var.h"
#include <stdint.h>

struct Position;
struct Thread_args;

void mov_arg(struct Position *const to_ptr, const struct Position *from_ptr,
             const int8_t move);

int is_legal_position(const void *_args);

void borrar_path(struct Path *path);
void *mov(const void *_args);
