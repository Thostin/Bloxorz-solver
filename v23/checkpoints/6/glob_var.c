/*
 * Its worth noticing that x refers to rows and y refers to columns
 * */
#ifndef GLOB_VAR
#define GLOB_VAR
#include "defs.h"

// The map will be a 2d array interpreted as whatever you want
block_t *map;

// Steps for each posible position
steps_count_t *steps_map;

type_coordinates start_x = 0;
type_coordinates start_y = 0;

type_coordinates map_size_x = 0;
type_coordinates map_size_y = 0;

/*
Bool defined_start_point = false;
Bool defined_winning_point = false;
*/

struct Path {
  struct Path *from;

  // from 0 to 3 colgados
  unsigned short colgados : 2;
  unsigned move : 2;
} __attribute__((packed));

struct Path *current_shortest_path = nullptr;

size_t dinamic_RAM_usage = 0;
size_t dinamic_map_RAM_usage = 0;
#endif
