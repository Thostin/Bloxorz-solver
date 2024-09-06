#include "SDL.h"
#include <stdint.h>

#define MY_COMPUTER

#ifdef SCREEN_SIZE_Y
#error "Macro SCREEN_SIZE_Y already defined"
#endif

#ifdef SCREEN_SIZE_X
error "Macro SCREEN_SIZE_X already defined "
#endif

    /*
    #define SCREEN_SIZE_X 1920
    #define SCREEN_SIZE_Y 1080
    */

    void
    render_menu(void);

int init_SDL(void);
void InitRenderMap(void);

#ifndef nullptr
#define nullptr NULL
#endif

#ifdef SQUARE_LEN
#error "Macro SQUARE_LEN already defined"
#endif /* ifdef MACRO */

/*
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define SQUARE_LEN                                                             \
  (min(SCREEN_SIZE_X, SCREEN_SIZE_Y) / (max(map_size_x, map_size_y)))
*/

#define SQUARE_LEN (SCREEN_SIZE_Y / map_size_y)
typedef struct {
  uint8_t r, g, b;
} Color_t;

extern const Color_t BlockColors[];
extern const Color_t BottomColors[];

extern SDL_Window *window;
extern SDL_Renderer *renderer;

// For my computer:
#ifdef MY_COMPUTER
#define INIT_SCREEN_BMP_FILE                                                   \
  "/home/thotstin/Code/C/p/bloxorz/v8/BloxorzAlgorithm/img_src/"               \
  "bloxorz_menu_3.bmp"
#define GAME_OVER_SCREEN_BMP_FILE                                              \
  "/home/thotstin/Code/C/p/bloxorz/v8/BloxorzAlgorithm/img_src/perdiste_1.bmp"
#define WIN_SCREEN_BMP_FILE                                                    \
  "/home/thotstin/Code/C/p/bloxorz/v8/BloxorzAlgorithm/img_src/ganaste_1.bmp"

#define SCREEN_SIZE_X 1920
#define SCREEN_SIZE_Y 1080
#else
// Para la PC del club
#define INIT_SCREEN_BMP_FILE                                                   \
  "/home/karai_kacho/Code/bloxorz-solver/v8/BloxorzAlgorithm/img_src/"         \
  "bloxorz_menu_3.bmp"
#define GAME_OVER_SCREEN_BMP_FILE                                              \
  "/home/karai_kacho/Code/bloxorz-solver/v8/BloxorzAlgorithm/img_src/"         \
  "perdiste_1.bmp"
#define WIN_SCREEN_BMP_FILE                                                    \
  "/home/karai_kacho/Code/bloxorz-solver/v8/BloxorzAlgorithm/img_src/"         \
  "ganaste_1.bmp"

#define SCREEN_SIZE_X 1366
#define SCREEN_SIZE_Y 768
#endif

extern SDL_Rect last1, last2;
extern Color_t LastColor1, LastColor2;

extern struct Position RenderPos;

extern const Color_t Var_player_color;
extern const Color_t Var_background_color;

// static SDL_Rect *RectMatrix;

extern const Color_t BlockColors[];

void MoveRender(uint8_t move, struct Position DstPos);
void render_static_texture(char *tex);
