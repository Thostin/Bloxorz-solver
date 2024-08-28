#include "SDL.h"
#include <stdint.h>

#ifdef SCREEN_SIZE_Y
#error "Macro SCREEN_SIZE_Y already defined"
#endif

#ifdef SCREEN_SIZE_X
error "Macro SCREEN_SIZE_X already defined "
#endif

#define SCREEN_SIZE_X 1280
#define SCREEN_SIZE_Y 720

    int
    init_SDL(void);
void InitRenderMap(void);

#ifndef nullptr
#define nullptr NULL
#endif

#ifdef SQUARE_LEN
#error "Macro SQUARE_LEN already defined"
#endif /* ifdef MACRO */

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define SQUARE_LEN                                                             \
  (min(SCREEN_SIZE_X, SCREEN_SIZE_Y) / (max(map_size_x, map_size_y)))

typedef struct {
  uint8_t r, g, b;
} Color_t;

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern SDL_Rect last1, last2;
extern Color_t LastColor1, LastColor2;

extern struct Position RenderPos;

extern const Color_t Var_player_color;
extern const Color_t Var_background_color;

// static SDL_Rect *RectMatrix;

extern const Color_t BlockColors[];
