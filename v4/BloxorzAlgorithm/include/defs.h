#pragma once

#include <stdio.h>
#define CHEREMBO

#ifndef nullptr
#define nullptr (void *)0x0
#endif
#define BAD_ALLOC 7

typedef struct Thread_args thread_args;
typedef struct Thread_args *thread_args_ptr;

typedef struct Position Pos;
typedef unsigned char block_t;

// unsigned short because I'm gonna use strange properties...
typedef unsigned short type_coordinates;

typedef unsigned long steps_count_t;
typedef unsigned char cardinal;

typedef unsigned char Bool;

#define true 1
#define false 0

enum { VOID = 0, BLANK_BLOCK = 1, START_BLOCK = 2, WINNING_BLOCK = 3 };

enum { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };

struct Position {
  unsigned orientation : 2;
  type_coordinates x;
  type_coordinates y;
};

struct Thread_args {
  // NORTH 0, EAST 1, SOUTH 2, WEAST 3

  struct Path *path;

  steps_count_t steps;
  struct Position pos;
  unsigned move : 2;
};

struct Path {
  unsigned move : 2;
  signed colgados;
  struct Path *from;
} __attribute__((packed));

typedef struct {
  steps_count_t steps;
  struct Position pos_arg;
} args_IsLegalPosition;

#define INF_LIM 1

#define READ_TEMPLATE_BLOCK "%hhu"
#define READ_TEMPLATE_COORDINATES "%hu"
