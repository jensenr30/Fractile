#include "bool.h"


// this keeps track of what vector is selected
int currentVector;

struct vectorData{
	//----------------------------------------------
	// normal parameters
	//----------------------------------------------
	// magnitude of each component
	double x, y;
	// offsets from the origin
	double x0, y0;
	//----------------------------------------------
	// wobble parameters
	//----------------------------------------------
	// these are the original x and y magnitudes just before wobble was started.
	double xorig, yorig;
	// these two variables describe (relative to xorig and yorig) the radius of the circle that the wobble orbits on around <xorig,yorig>
	double xWobble, yWobble;
	// this is when (in milliseconds) the wobble started (giving us a reference
	int wobbleStartTime;
	// this is how long it takes for the vector to wobble full circle. (milliseconds)
	// if period == 0, then we are NOT WOBBLING
	// any other positive integer, and this vector is wobbling.
	int period;
	
};


struct pointData{
	double x, y;
};

// this is the maximum number of individual fractals that can be added to the base iteration.
// when the program DRAWS a fractal, it will end up drawing a LOT MORE than this number.
// this number just describes how many different vectors there can be on the base vector (the starting one)
#define MAX_VECTORS_PER_FRACTAL 100
// this defines how many exit points the fractal can have. and exit point is where the current level fractal will spawn the next level fractal.
#define MAX_EXITS_PER_FRACTAL 100

struct fractalData{
	
	// this is the main color of the fractal
	unsigned long int color1;
	// this is the secondary color of the fractal
	unsigned long int color2;
	
	// the initial thickness of the lines
	float thickness;
	
	
	// this is the fractal's array of vectors
	struct vectorData vects[MAX_VECTORS_PER_FRACTAL];
	// this is the array of the fractal's exit points
	struct pointData exits[MAX_EXITS_PER_FRACTAL];
	
	
	// this is the scaling factor for every iteration down the fractal does
	double scale;
	// this describes how quickly the color changes from the first iteration (at color1) to color2 (in later iterations)
	double colorScaler;
	
	
	// this is how many iterations of the fractal will be printed
	int iterations;
	
	
	// this is the number of exit points the fractal has
	int numbExits;
	// this is how many vectors there currently are in the fractal
	int numbVectors;
	
	// this is how much the fractal will twist on each iteration
	// measured in radians
	double twist;
	
};



void fractal_iteration(SDL_Texture *dest, struct fractalData **f, double entryx, double entryy, int iter);
void fractal_print(SDL_Texture *dest, struct fractalData *fractal);
void fractal_random(struct fractalData *f, int maxVects, int maxIterations);


/// vector wobble (vw_)
#define vw_evaluate	0
#define vw_toggle	1

// this is the period of one full circle wobble (in milliseconds)
#define WOBBLE_DEFAULT_PERIOD 1400
// this is the radius of the circle that the wobble will orbit (expressed in terms of the original vector's magnitude) 
#define WOBBLE_PERCENTAGE_OF_MAGNITUDE 0.10f

void fractal_wobble(struct fractalData *f, int wobbleEvent);


// the editor is a sidbar on the left side of the window.
#define EDITOR_DEFAULT_WIDTH 150
#define EDITOR_COLOR_PRIMARY 0xff2f2f2f
#define EDITOR_COLOR_SECONDARY 0xff3f3f3f
#define EDITOR_COLOR_SCROLL_BAR 0xff5f5f5f
#define EDITOR_COLOR_SCROLL_BAR_BACKGROUND EDITOR_COLOR_PRIMARY
#define EDITOR_CURRENT_VECTOR_OUTLINE_COLOR 0xffff0000
#define EDITOR_CURRENT_BUTTON_OUTLINE_COLOR 0xffff0000
#define EDITOR_WOBBLE_OUTLINE_COLOR 0xff0000ff
#define EDITOR_OUTLINE_THICKNESS 2
#define EDITOR_BUTTON_SIZE 32
#define EDITOR_TITLE_BAR_HEIGHT 16
#define EDITOR_SCROLL_BAR_WIDTH 16

// these are enumerations for editor buttons. list starts at zero so it can easily be plugged into an array.
#define EDITOR_BUTTONS_VECTORS 0
#define EDITOR_BUTTONS_EXITPOINTS 1
// this is how many buttons there are.
#define EDITOR_BUTTONS_NUMBER_OF 2

/// these are enumerations of editor events
#define ee_print 0			// just print the editor. the user isn't doing anything with it at this time.
#define ee_toggle 1			// this will toggle if the editor is open or not.
#define ee_left_click_down 2	// this is when the user presses  the left mouse button
#define ee_left_click_up 3		// this is when the user releases the left mouse button
#define ee_right_click_down 4	// this is when the user presses  the right mouse button
#define ee_right_click_up 5		// this is when the user releases the right mouse button


bool fractal_editor(SDL_Texture *dest, struct fractalData *f, int x, int y, int editorEvent);
void init_fractal_editor();

#define MAX_FRACTILE_PATH 256
int fractal_save(struct fractalData *f, char *filename);
int fractal_load(struct fractalData *f, char *filename);
int fractal_save_windows(struct fractalData *f);
int fractal_load_windows(struct fractalData *f);


