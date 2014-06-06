#include <SDL2/SDL.h>
#include "fractile.h"
#include "graphics.h"
#include "globals.h"
#include "math.h"
#include "math_custom.h"
#include "Windows.h"
#include <stdio.h>



// this is the recursive function that will draw the fractal.
// iter (iterations) starts at 0!!
// **f is a pointer to an array of fractals
void fractal_iteration(SDL_Texture *dest, struct fractalData **f, double entryx, double entryy, int iter){

	int i;

	// print all the vectors
	for(i=0; i<f[iter]->numbVectors; i++){
		draw_line(dest, entryx+(f[iter]->vects[i].x0), entryy+(f[iter]->vects[i].y0), entryx+(f[iter]->vects[i].x0+f[iter]->vects[i].x), entryy+(f[iter]->vects[i].y0+f[iter]->vects[i].y), f[iter]->thickness, f[iter]->color1);
	}


	// return if you are done with all iterations of the fractal
	if(iter >= f[0]->iterations-1){
		return;
	}


	// recurdoes sively call more fractal iteration functions at the exit points of this iteration.
	for(i=0; i<f[iter]->numbExits; i++){
		fractal_iteration(dest, f, entryx+f[iter]->exits[i].x, entryy + f[iter]->exits[i].y, iter+1);
	}

}


// dest is the SDL_Texture to which everything will be printed
// f is a pointer to the fractal structure that contains all the information about the fractal that will be printed.
// entryx and entryy are the <x,y> coordinates of the first entry point of this iteration of the fractal
void fractal_print(SDL_Texture *dest, struct fractalData *f){
	int j;
	// make sure the fractal and the destination surface are valid
	if(f == NULL || dest == NULL) return;

	// make sure numbVectors isn't larger than the cap
	if(f->numbVectors >= MAX_VECTORS_PER_FRACTAL) f->numbVectors = MAX_VECTORS_PER_FRACTAL-1;
	// make sure numbExits   isn't larger than the cap
	if(f->numbExits >= MAX_EXITS_PER_FRACTAL) f->numbExits = MAX_EXITS_PER_FRACTAL-1;

	//-------------------------------------------------------------------------------
	// fit the fractal to the window
	//-------------------------------------------------------------------------------
	double xvmax=0; // largest single vector movement in the +x direction
	double xvmin=0; // largest single vector movement in the -x direction
	double yvmax=0; // largest single vector movement in the +y direction
	double yvmin=0; // largest single vector movement in the -y direction

	double xemax=0; // largest single exit movement in the +x direction
	double xemin=0; // largest single exit movement in the -x direction
	double yemax=0; // largest single exit movement in the +y direction
	double yemin=0; // largest single exit movement in the -y direction

	double xmax=0; // total movement in the +x direction
	double xmin=0; // total movement in the -x direction
	double ymax=0; // total movement in the +y direction
	double ymin=0; // total movement in the -y direction

	int i;
	for(i=0; i<f->numbVectors; i++){
		if(f->vects[i].x+f->vects[i].x0 > xvmax) xvmax = f->vects[i].x+f->vects[i].x0;
		if(f->vects[i].y+f->vects[i].y0 > yvmax) yvmax = f->vects[i].y+f->vects[i].y0;
		if(f->vects[i].x+f->vects[i].x0 < xvmin) xvmin = f->vects[i].x+f->vects[i].x0;
		if(f->vects[i].y+f->vects[i].y0 < yvmin) yvmin = f->vects[i].y+f->vects[i].y0;
	}

	for(i=0; i<f->numbExits; i++){
		if(f->exits[i].x > xemax) xemax = f->exits[i].x;
		if(f->exits[i].y > yemax) yemax = f->exits[i].y;
		if(f->exits[i].x < xemin) xemin = f->exits[i].x;
		if(f->exits[i].y < yemin) yemin = f->exits[i].y;
	}


	double scale;
	for(i=1, scale=1; i<=f->iterations; i++, scale*=f->scale){
		xmax += (xvmax+xemax)*scale;
		ymax += (yvmax+yemax)*scale;
		xmin += (xvmin+xemin)*scale;
		ymin += (yvmin+yemin)*scale;
	}

	// the maximum/minimum cannot pass over the origin because the maximum/minimum is at LEAST the origin.
	if(xmin>0)xmin = 0.0f;
	if(ymin>0)ymin = 0.0f;
	if(xmax<0)xmax = 0.0f;
	if(ymax<0)ymax = 0.0f;

	double xdiff = xmax - xmin;
	double ydiff = ymax - ymin;

	double xscale = WINDOW_WIDTH/(xdiff);
	double yscale = WINDOW_WIDTH/(ydiff);

	if(xscale < yscale){
		scale = xscale;
	}
	else{
		scale = yscale;
	}

	// allocate space for calculating the fractals at every iteration size
	// I forgot why I called it fa. fractal "adjusted"? fractal "after rotation"? "all done"? I don't know... I'm beginning to doubt there ever was a reason...
	struct fractalData *fa = malloc(f->iterations * sizeof(struct fractalData));
	// declare an array of fractal pointers of the correct length
	struct fractalData **fp = malloc(sizeof(struct fractalData*)*f->iterations);

	if(fa == NULL || fp == NULL){
		apply_text(dest, WINDOW_WIDTH/2 - 150,WINDOW_HEIGHT/2,font16,"malloc returned NULL for either *fa or **fp", colorRed);
		return;
	}
	else{

		// initial starting values for the first fractal iteration
		fa[0].scale = f->scale;
		fa[0].thickness = f->thickness;

		// calculate all the fractals at each level
		for(i=0; i<f->iterations; i++){

			fa[i].color1 = f->color1;
			fa[i].color2 = f->color2;
			fa[i].colorScaler = f->colorScaler;
			fa[i].iterations = f->iterations;
			fa[i].numbVectors = f->numbVectors;
			fa[i].numbExits = f->numbExits;
			fa[i].twist = f->twist;
			if(i>0){
				fa[i].scale = fa[i-1].scale*f->scale;
				fa[i].thickness = fa[i-1].thickness*f->scale;
			}

			//rotate and scale the vectors
			for(j=0; j<f->numbVectors; j++){
				fa[i].vects[j].period = f->vects[j].period;
				fa[i].vects[j].wobbleStartTime = f->vects[j].wobbleStartTime;

				rotate_point(f->vects[j].x + f->vects[j].x0, f->vects[j].y + f->vects[j].y0, &fa[i].vects[j].x, &fa[i].vects[j].y, f->twist*i);
				rotate_point(f->vects[j].x0, 		f->vects[j].y0,			&fa[i].vects[j].x0,			&fa[i].vects[j].y0,			f->twist*i);
				rotate_point(f->vects[j].xorig, 	f->vects[j].yorig,		&fa[i].vects[j].xorig,		&fa[i].vects[j].yorig,		f->twist*i);
				rotate_point(f->vects[j].xWobble,	f->vects[j].yWobble,	&fa[i].vects[j].xWobble,	&fa[i].vects[j].yWobble,	f->twist*i);
				fa[i].vects[j].x -= fa[i].vects[j].x0;
				fa[i].vects[j].y -= fa[i].vects[j].y0;
				fa[i].vects[j].x *=			fa[i].scale;
				fa[i].vects[j].y *=			fa[i].scale;
				fa[i].vects[j].x0 *=		fa[i].scale;
				fa[i].vects[j].y0 *=		fa[i].scale;
				fa[i].vects[j].xorig *=		fa[i].scale;
				fa[i].vects[j].yorig *=		fa[i].scale;
				fa[i].vects[j].xWobble *=	fa[i].scale;
				fa[i].vects[j].yWobble *=	fa[i].scale;
			}

			// rotate and scale the exits
			for(j=0; j<f->numbExits; j++){

				rotate_point(f->exits[j].x, f->exits[j].y, &fa[i].exits[j].x, &fa[i].exits[j].y, f->twist*i);
				fa[i].exits[j].x *= fa[i].scale;
				fa[i].exits[j].y *= fa[i].scale;
			}


			for(j=0; j<f->iterations; j++){
				fp[j] = &fa[j];
			}
		}
		for(i=0; i<f->iterations; i++){
			fa[i].scale *= scale;
		}
		fractal_iteration(dest, fp, -((xmax+xmin)*scale)/2 + WINDOW_WIDTH/2, -((ymax+ymin)*scale)/2 + WINDOW_HEIGHT/2, 0);
		free(fp);
		free(fa);
	}

}


void fractal_wobble(struct fractalData *f, int wobbleEvent){
	int i;

	if(wobbleEvent==vw_evaluate){
		double wobbleAngle;
		for(i=0; i<f->numbVectors; i++){
			if(f->vects[i].period > 0){
				wobbleAngle = (2*PI*(SDL_GetTicks() - f->vects[i].wobbleStartTime))/f->vects[i].period;
				f->exits[i].x = f->vects[i].x = f->vects[i].xorig*(1.0 + f->vects[i].xWobble*cos(wobbleAngle));
				f->exits[i].y = f->vects[i].y = f->vects[i].yorig*(1.0 + f->vects[i].yWobble*sin(wobbleAngle));
			}
		}
	}


	if(wobbleEvent==vw_toggle){
		if(f->vects[currentVector].period == 0){
			f->vects[currentVector].wobbleStartTime = SDL_GetTicks();
			f->vects[currentVector].period = WOBBLE_DEFAULT_PERIOD;
			f->vects[currentVector].xorig = f->vects[currentVector].x;
			f->vects[currentVector].yorig = f->vects[currentVector].y;
			f->vects[currentVector].xWobble = WOBBLE_PERCENTAGE_OF_MAGNITUDE;
			f->vects[currentVector].yWobble = WOBBLE_PERCENTAGE_OF_MAGNITUDE;
		}
		else
			f->vects[currentVector].period = 0;
			f->exits[currentVector].x = f->vects[currentVector].x = f->vects[currentVector].xorig;
			f->exits[currentVector].y = f->vects[currentVector].y = f->vects[currentVector].yorig;

	}
}



void fractal_random(struct fractalData *f, int maxVects, int maxIterations){
	/*
    f->vects[0].x0= 0;
    f->vects[0].y0= 0;
    f->vects[0].x=  200;
    f->vects[0].y=  200;
    f->vects[1].x0= 0;
    f->vects[1].y0= 0;
    f->vects[1].x=  200;
    f->vects[1].y=  0;
    f->vects[2].x0= 0;
    f->vects[2].y0= 0;
    f->vects[2].x=  0;
    f->vects[2].y=  200;
    f->vects[3].x0= 0;
    f->vects[3].y0= 0;
    f->vects[3].x=  -200;
    f->vects[3].y=  200;

    f->numbVectors = 4;

    f->exits[0].x = 200;
    f->exits[0].y = 200;
    f->exits[1].x = 200;
    f->exits[1].y = 0;
    f->exits[2].x = 0;
    f->exits[2].y = 200;
    f->exits[3].x = -200;
    f->exits[3].y = 200;

    f->numbExits = 4;
    */
    f->iterations = maxIterations;
    int i;
    f->twist = 0;//PI/6;//f->twist = (((rand()%20001)-10000)*PI)/10000;
    f->numbVectors = f->numbExits = rand()%(maxVects-1)+ 2;
    for(i=0; i<f->numbVectors; i++){
		f->vects[i].x0 = 0;//rand()%400 + 20;
		f->vects[i].y0 = 0;//rand()%400 + 20;
		f->vects[i].x = (rand()%400 + 20)*(rand()%2-0.5)*2;
		f->vects[i].y = (rand()%400 + 20)*(rand()%2-0.5)*2;
		f->exits[i].x = f->vects[i].x0 + f->vects[i].x;
		f->exits[i].y = f->vects[i].y0 + f->vects[i].y;
		f->vects[i].period = 0; // not wobbling
		f->vects[i].wobbleStartTime = 0;
    }
}


void init_fractal_editor(){


}


// f if the fractal that we will print the vectors for
/// returns true if the user's mouse click or event happened inside of the fractal editor.
bool fractal_editor(SDL_Texture *dest, struct fractalData *f, int x, int y, int editorEvent){

	//these keep track of where the mouse was last time this function was called.
	//static int xlast;
	//static int ylast;
	// this is a general purpose rectangle.
	static SDL_Rect genRect;
	// a secondary general purpose rectangle
	static SDL_Rect genRect2;
	// this keeps track of whether or not this is the first time running through this function.
	static bool firstTime = true;
	// this is a rectangle than describes the position <x,y> and size <w,h> of the editor virtual window.
	static SDL_Rect editor;
	// this tells us if the editor window is open
	static bool editorOpen=true;
	// this tells us what button the editor is currently on.
	static int editorButton=EDITOR_BUTTONS_VECTORS;
	// this is the current width of the editor
	static int currentWidth = EDITOR_DEFAULT_WIDTH;
	// this is the scale at which the vectors in the editor window are printed to the window (pixels/vector-size)
	static double scale;
	// this keeps track of what state the right mouse button was/is in.
	static bool rightMouseButton=false;
	// these keep track of where the right mouse button was pressed down.
	static int xRightMouseInitial;
	static int yRightMouseInitial;
	static double xVectInit;
	static double yVectInit;
	static double dragSpeed = 0.1f;

	// this keeps track of whether or not we need to calculate the vector-to-pixel scaling factor
	bool calculateVectorScale=false;

	static SDL_Rect buttons[EDITOR_BUTTONS_NUMBER_OF];
	int i,b,v;


	// handle all first-time-through things
	if(firstTime){
		editor.x = 0;
		editor.y = 0;
		calculateVectorScale = true;
		for(i=0; i<EDITOR_BUTTONS_NUMBER_OF; i++){
			buttons[i].w = buttons[i].h = EDITOR_BUTTON_SIZE;
			buttons[i].y = i*EDITOR_BUTTON_SIZE;
		}
		firstTime = false;
	}
	//-------------------------------------------------------------------------------
	// adjust the position of the editor buttons
	//-------------------------------------------------------------------------------
	for(i=0; i<EDITOR_BUTTONS_NUMBER_OF; i++){
		buttons[i].x = (currentWidth+EDITOR_TITLE_BAR_HEIGHT)*editorOpen;
	}
	//-------------------------------------------------------------------------------
	// check for user input
	//-------------------------------------------------------------------------------
	//check for toggling
	if(editorEvent == ee_toggle) editorOpen^=1;
	//check for right click down
	if(editorEvent == ee_right_click_down){
		rightMouseButton = true;
		xRightMouseInitial = x;
		yRightMouseInitial = y;
		if(editorButton == EDITOR_BUTTONS_VECTORS){
			xVectInit = f->vects[currentVector].x;
			yVectInit = f->vects[currentVector].y;
		}
	}
	//check for right mouse button up
	else if(editorEvent == ee_right_click_up){
		rightMouseButton = false;
		if(editorButton == EDITOR_BUTTONS_VECTORS){
			f->exits[currentVector].x = f->vects[currentVector].x = xVectInit + ((x-xRightMouseInitial)*dragSpeed)/scale;
			f->exits[currentVector].y = f->vects[currentVector].y = yVectInit + ((y-yRightMouseInitial)*dragSpeed)/scale;
			calculateVectorScale=true;
		}
	}
	// check for left click down
	else if(editorEvent == ee_left_click_down){
		//check for selecting a vector
		if(editorOpen && x<currentWidth && y>=EDITOR_TITLE_BAR_HEIGHT){
			currentVector = ((y-EDITOR_TITLE_BAR_HEIGHT)/currentWidth)%f->numbVectors;
		}
		for(b=0; b<EDITOR_BUTTONS_NUMBER_OF; b++){
			if(within_rect(&buttons[b],x,y)){
				editorButton = b;
			}
		}
	}

	if(editorButton == EDITOR_BUTTONS_VECTORS || editorButton == EDITOR_BUTTONS_EXITPOINTS){
		//-------------------------------------------------------------------------------
		// check to see if we need to calculate the scaling factor for printing the editor window vectors
		//-------------------------------------------------------------------------------
		if(calculateVectorScale){
			//-------------------------------------------------------------------------------
			//determine the longest magnitude vector in this vector
			//-------------------------------------------------------------------------------
			double yMaxMag=0.0f;
			double xMaxMag=0.0f;
			for(i=0; i<f->numbVectors; i++){
				if(fabs(f->vects[i].x)>xMaxMag) xMaxMag = fabs(f->vects[i].x);
				if(fabs(f->vects[i].y)>yMaxMag) yMaxMag = fabs(f->vects[i].y);
			}
			//-------------------------------------------------------------------------------
			// set the scale of the vectors in the editor sidebar
			//-------------------------------------------------------------------------------
			// this is the scale at which the vectors of the fractal will be printed in the editor
			// scale is in units of (pixels/vector-size) so that it can be multiplied by a vector's size and the units work out to be pixels.
			if(xMaxMag > yMaxMag){
				scale = currentWidth/(xMaxMag*2);
			}
			else{
				scale = currentWidth/(yMaxMag*2);
			}
		}
	}


	//-------------------------------------------------------------------------------
	// check for scaling of the current vector
	//-------------------------------------------------------------------------------
	if(rightMouseButton){
		if(editorButton == EDITOR_BUTTONS_VECTORS){
			f->exits[currentVector].x = f->vects[currentVector].x = xVectInit + ((x-xRightMouseInitial)*dragSpeed)/scale;
			f->exits[currentVector].y = f->vects[currentVector].y = yVectInit + ((y-yRightMouseInitial)*dragSpeed)/scale;
		}
	}


	// if the destination surface is null, don't bother printing anything
	if(dest == NULL) return true;

	//update the size of the editor window
	editor.h = WINDOW_HEIGHT-editor.y;		// the editor window goes as far down as possible without going outside the window
	editor.w = currentWidth*editorOpen;		// the width of the editor is the currentWidth if the editor is open, and 0 if the editor is closed.

	//-------------------------------------------------------------------------------
	// printing stuff
	//-------------------------------------------------------------------------------
	// print buttons
	for(i=0; i<EDITOR_BUTTONS_NUMBER_OF; i++){
		SDL_FillRectTexture(dest, &buttons[i], EDITOR_COLOR_PRIMARY*((i%2)^1) + EDITOR_COLOR_SECONDARY*(i%2) );
		// apply the red outline to the current
		if(i==editorButton)
			apply_outline(dest, &buttons[i], EDITOR_OUTLINE_THICKNESS, EDITOR_CURRENT_BUTTON_OUTLINE_COLOR);
		// print the vectors on the button
		if(i == EDITOR_BUTTONS_VECTORS){
			for(v=0; v<f->numbVectors; v++){
				// draw each line in the EDITOR_BUTTONS_VECTOR button for an artistic effect
				draw_line_diff(dest, editor.x + editor.w + EDITOR_SCROLL_BAR_WIDTH + buttons[i].w/2.0, editor.y + EDITOR_BUTTON_SIZE*(i+0.5), (f->vects[v].x*scale*buttons[i].w)/((float)currentWidth), (f->vects[v].y*scale*buttons[i].h)/((float)currentWidth), f->thickness, f->color1);
			}
		}
	}


	if(editorOpen){
		// print the background colors of the editor
		SDL_FillRectTexture(dest, &editor, EDITOR_COLOR_PRIMARY);

		genRect.x=editor.x+currentWidth;
		genRect.y=editor.y;
		genRect.w = EDITOR_SCROLL_BAR_WIDTH;
		genRect.h = editor.h;
		SDL_FillRectTexture(dest, &genRect, EDITOR_COLOR_SCROLL_BAR_BACKGROUND);

		//-------------------------------------------------------------------------------
		// print the sidebar
		//-------------------------------------------------------------------------------
		// genRect is for the selected vector outlines and the even/odd vector background coloring
		genRect.x = editor.x;
		genRect.w = genRect.h = currentWidth;
		// genRect2 is for the wobble outlines
		genRect2.x = editor.x + editor.w/4;
		genRect2.w = editor.w/2;
		genRect2.h = genRect.h/2;
		for(i=0; i<f->numbVectors && i*currentWidth<WINDOW_HEIGHT; i++){

			// if i is an odd integer, print an secondary color background square
			if(i%2){
				genRect.y = editor.y + i*currentWidth + EDITOR_TITLE_BAR_HEIGHT;
				SDL_FillRectTexture(dest, &genRect, EDITOR_COLOR_SECONDARY);
			}
			// if the vector is wobbling, print a box around it
			if(f->vects[i].period > 0){
				genRect2.y = genRect.h*(0.25 + i) + EDITOR_TITLE_BAR_HEIGHT;
				apply_outline(dest, &genRect2, EDITOR_OUTLINE_THICKNESS, EDITOR_WOBBLE_OUTLINE_COLOR);
			}
			// print the vector
			if(i==currentVector){
				genRect.y = editor.y + i*currentWidth + EDITOR_TITLE_BAR_HEIGHT;
				apply_outline(dest, &genRect, EDITOR_OUTLINE_THICKNESS, EDITOR_CURRENT_VECTOR_OUTLINE_COLOR);
			}
			// draw each line in each box
			draw_line(dest, currentWidth/2, currentWidth*(i+0.5) + EDITOR_TITLE_BAR_HEIGHT, currentWidth/2 + f->vects[i].x*scale, currentWidth*(i+0.5) + EDITOR_TITLE_BAR_HEIGHT + f->vects[i].y*scale, f->thickness, f->color1);
		}
	}
	// the user did not interact with the editor.
	return false;
}

int fractal_save(struct fractalData *f, char *filename){
	if(f == NULL || filename == NULL) return false;
	FILE *savefile = fopen(filename, "w");
	if(savefile == NULL)return false;

	fprintf(savefile,"version = %f\n",programVersion);
	fprintf(savefile,"color1 = 0x%lx\n",f->color1);
	fprintf(savefile,"color2 = 0x%lx\n",f->color2);
	fprintf(savefile,"colorScaler = %lf\n",f->colorScaler);
	fprintf(savefile,"iterations = %d\n",f->iterations);
	fprintf(savefile,"scale = %lf\n",f->scale);
	fprintf(savefile,"twist = %lf\n",f->twist);
	fprintf(savefile,"numbVectors = %d\n", f->numbVectors);
	fprintf(savefile,"numbExits = %d\n", f->numbExits);
	int i;
	for(i=0; i<f->numbVectors; i++){
		fprintf(savefile,"x = %lf\n",f->vects[i].x);
		fprintf(savefile,"y = %lf\n",f->vects[i].y);
		fprintf(savefile,"x0 = %lf\n",f->vects[i].x0);
		fprintf(savefile,"y0 = %lf\n",f->vects[i].y0);
		fprintf(savefile,"xorig = %lf\n",f->vects[i].xorig);
		fprintf(savefile,"yorig = %lf\n",f->vects[i].yorig);
		fprintf(savefile,"xWobble = %lf\n",f->vects[i].xWobble);
		fprintf(savefile,"yWobble = %lf\n",f->vects[i].yWobble);
		fprintf(savefile,"period = %d\n",f->vects[i].period);
		if(f->vects[i].period > 0)
			fprintf(savefile,"wobbleStartTime = %d\n",f->vects[i].wobbleStartTime%f->vects[i].period);
		else
			fprintf(savefile,"wobbleStartTime = 0\n");
	}
	for(i=0; i<f->numbExits; i++){
		fprintf(savefile,"x = %lf\n",f->exits[i].x);
		fprintf(savefile,"y = %lf\n",f->exits[i].y);
	}
	fclose(savefile);
	return true;
}
int fractal_load(struct fractalData *f, char *filename){
	if(f == NULL || filename == NULL) return false;
	FILE *savefile = fopen(filename, "r");
	if(savefile == NULL)return false;

	float loadedVersion=0.0;

	fscanf(savefile,"version = %f\n",&loadedVersion);
	fscanf(savefile,"color1 = 0x%lx\n",&f->color1);
	fscanf(savefile,"color2 = 0x%lx\n",&f->color2);
	fscanf(savefile,"colorScaler = %lf\n",&f->colorScaler);
	fscanf(savefile,"iterations = %d\n",&f->iterations);
	fscanf(savefile,"scale = %lf\n",&f->scale);
	fscanf(savefile,"twist = %lf\n",&f->twist);
	fscanf(savefile,"numbVectors = %d\n", &f->numbVectors);
	fscanf(savefile,"numbExits = %d\n", &f->numbExits);
	int i;
	for(i=0; i<f->numbVectors; i++){
		fscanf(savefile,"x = %lf\n",&f->vects[i].x);
		fscanf(savefile,"y = %lf\n",&f->vects[i].y);
		fscanf(savefile,"x0 = %lf\n",&f->vects[i].x0);
		fscanf(savefile,"y0 = %lf\n",&f->vects[i].y0);
		fscanf(savefile,"xorig = %lf\n",&f->vects[i].xorig);
		fscanf(savefile,"yorig = %lf\n",&f->vects[i].yorig);
		fscanf(savefile,"xWobble = %lf\n",&f->vects[i].xWobble);
		fscanf(savefile,"yWobble = %lf\n",&f->vects[i].yWobble);
		fscanf(savefile,"period = %d\n",&f->vects[i].period);
		fscanf(savefile,"wobbleStartTime = %d\n",&f->vects[i].wobbleStartTime);
		if(f->vects[i].period > 0) f->vects[i].wobbleStartTime %= f->vects[i].period;
	}
	for(i=0; i<f->numbExits; i++){
		fscanf(savefile,"x = %lf\n",&f->exits[i].x);
		fscanf(savefile,"y = %lf\n",&f->exits[i].y);
	}
	fclose(savefile);
	return true;
}


int fractal_save_windows(struct fractalData *f){
	OPENFILENAME ofn;
	char szFileName[MAX_FRACTILE_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = "Fractal Files (*.fractal)\0*.fractal\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_FRACTILE_PATH;
	ofn.lpstrDefExt = "fractal";
	ofn.lpstrInitialDir = "saves\\";
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST;

	if(GetSaveFileName(&ofn))
	{
	    return fractal_save(f, szFileName);
	}
	return false;
}

int fractal_load_windows(struct fractalData *f){
	OPENFILENAME ofn;
	char szFileName[MAX_FRACTILE_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = "Fractal Files (*.fractal)\0*.fractal\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_FRACTILE_PATH;
	ofn.lpstrDefExt = "fractal";
	ofn.lpstrInitialDir = "saves\\";
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST;

	if(GetOpenFileName(&ofn))
	{
		return fractal_load(f, szFileName);
	}
	return false;
}










