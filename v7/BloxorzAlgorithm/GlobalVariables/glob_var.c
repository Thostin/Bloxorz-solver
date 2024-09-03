/*
 * Its worth noting that x refers to rows and y refers to colums
 * */
#ifndef GLOB_VAR
#define GLOB_VAR
#include "defs.h"

// The map will be a 2d array interpreted as whatever you want
block_t *map = nullptr;

// Steps for each posible position
steps_count_t *steps_map = nullptr;

type_coordinates start_x = 0;
type_coordinates start_y = 0;

type_coordinates map_size_x = 0;
type_coordinates map_size_y = 0;

Bool defined_start_point = false;
Bool defined_winning_point = false;

struct Path *current_shortest_path = nullptr;
unsigned int current_winning_path_steps = 2000000000;

size_t dinamic_RAM_usage = 0;
unsigned int TotalSteps;

// Files to write to

FILE *stdverbose = nullptr;
FILE *stdmoves = nullptr;
FILE *stdilegalmoves = nullptr;
FILE *stdtrash = nullptr;
#endif
