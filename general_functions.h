#include "bool.h"
#include <SDL2/SDL.h>


// these are different error types
#define e_loadfile 1
#define e_surface_creation 2
#define e_scalingFactor 3

//void handle_error(int type, char *msg);
bool is_empty_file(char *relativeFilePath);
void quit_game(Uint32 quitFlag);
void set_window_size(int w, int h);
SDL_Texture *load_image(char* filename);
SDL_Texture *create_surface(int width, int height);
int init();
int load_files();
void clean_up();
int get_rand(int,int);
int roll_ht(int chance);
bool within_rect(SDL_Rect *datrect, int x, int y);
void print_red_box(SDL_Texture *dest);
void error(char *);
