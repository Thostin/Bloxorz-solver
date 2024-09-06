#include "defs.h"
void play_map(void);
void bottom_play_map(void);
void bottom_animate_map();

int is_legal_position_no_test_steps(const struct Position pos_arg);
void CRASH_GAME(char *msg);

void detect_in_bottom(const struct Position pos_arg);
void Render_undo_flags(void);
void Render_do_flags(void);
