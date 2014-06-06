//#include "bool.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"


Uint32 get_texture_pixel(SDL_Texture *surface, int x, int y);
void set_texture_pixel(SDL_Texture *surf, int x, int y, Uint32 pixel);
void apply_surface( int x, int y,  SDL_Texture* source, SDL_Texture* destination );
int apply_text(SDL_Texture *datsurface, int x, int y, TTF_Font *theFont, char *theString, SDL_Color theColor);
void apply_surface_clips( int x, int y,  SDL_Texture *source, SDL_Texture *destination, SDL_Rect *clip );
void apply_outline(SDL_Texture *dest, SDL_Rect *box, unsigned short thickness, Uint32 fillColor);
#define grad_linear 0
#define grad_radial 1
void gradient(SDL_Texture *datsurface, SDL_Rect *gradClip, int x1, int y1, int x2, int y2, Uint32 color1, Uint32 color2, unsigned int gradientType);
void draw_line(SDL_Texture *dest, float x1, float y1, float x2, float y2, float thickness, unsigned int lineColor);
// this will draw the line from point (x0,y0) to point (x0+x,y0+y).
#define draw_line_diff(dest, x0, y0, x, y, thickness, lineColor) draw_line(dest, x0, y0, x0+x, y0+y, thickness, lineColor)
void draw_circle(SDL_Texture *dest, float x, float y, float radius, Uint32 color);
void SDL_FillRectTexture(SDL_Texture *dest, SDL_Rect *rect, unsigned int color);
