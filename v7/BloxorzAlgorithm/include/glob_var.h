#pragma once

#include "defs.h"
extern block_t *map;

extern steps_count_t *steps_map;

extern type_coordinates start_x;
extern type_coordinates start_y;

extern type_coordinates map_size_x;
extern type_coordinates map_size_y;

extern Bool defined_start_point;
extern Bool defined_winning_point;

extern struct Path *current_shortest_path;
extern unsigned int current_winning_path_steps;

extern size_t dinamic_RAM_usage;
extern unsigned int TotalSteps;

extern FILE *stdtrash, *stdmoves, *stdverbose, *stdilegalmoves;
