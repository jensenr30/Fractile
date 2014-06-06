#include "SDL2/SDL_ttf.h"

#define DEBUG 1 // general debug output.


//The CONSTANT initial size of the window
#define DEFAULT_WINDOW_WIDTH 896
#define DEFAULT_WINDOW_HEIGHT 576
//The DYNAMIC  initial size of the window
unsigned int WINDOW_WIDTH;
unsigned int WINDOW_HEIGHT;

#define within_window(x,y) (x>=0 && x<WINDOW_WIDTH && y>=0 && y<WINDOW_HEIGHT)

#define SCREEN_BPP 32

// global variable that tells you the FPS of the game.
int FPS; // arbitrary initial value

// The surfaces that will be used

SDL_Window *window;			// this is the main window
SDL_Renderer *renderer;		// this is the main renderer
SDL_Texture *texture;		// this is the main texture that all graphics will be printed to before being rendered to the window.

// The event structure that will be used
SDL_Event event;

// the fint that will be used
TTF_Font *font22;
TTF_Font *font16;

float programVersion;
SDL_Color colorRed;
