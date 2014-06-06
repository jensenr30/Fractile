#include <stdlib.h>
#include <time.h>
#include "general_functions.h"
#include "graphics.h"
#include "globals.h"
#include "math_custom.h"
#include <math.h>
#include "fractile.h"
#include <string.h>


int main( int argc, char* argv[] )
{
	colorRed.r = 0xff;
	colorRed.g = colorRed.b = 0;
	//random seed
	srand(time(NULL));

	// initial window sizes
	WINDOW_WIDTH = DEFAULT_WINDOW_WIDTH;
	WINDOW_HEIGHT = DEFAULT_WINDOW_HEIGHT;

	//----------------------------------------------------
	// DEBUG THE ROTATE_POINT() function
	//----------------------------------------------------
	//double xtest=5, ytest=5, xresult, yresult, angle=-5*PI/2;
	//rotate_point(xtest,ytest,&xresult,&yresult,angle);

	//----------------------------------------------------
	// VARIABLES USED IN MAIN()
	//----------------------------------------------------
    int x=0, y=0;								// this is the location of the player's mouse

    int mouseLeft[2] = {0,0};					// this keeps track of the user's left  mouse button state. [0]=current, [1]=previous
    int mouseRight[2] = {0,0};					// this keeps track of the user's right mouse button state. [0]=current, [1]=previous

	int ticksSinceLastFPSUpdate=0;				// time since last FPS update (ideally, goes from 0 to 1000 milliseconds and then resets)
	int cumulativeFrames = 0;					// this counts how many Frames there have been since the last
	int currentTicks = 0;						// this is how many
	int quit = false;							// make sure the program waits for a quit
	int xtwist=0, ytwist=0;						// these store the user's coordinates when the user started twisting myfractal
	bool twisting=false;						// this keeps track of whether or not the user is twisting the fractal.
	double twistInitial=0;						// this is what the twist was for myfractal when the user started changing it.
	double twistRadiansPerPixel = (2*PI)/WINDOW_WIDTH;	// this is how fast the twist of a fractal is changed
	unsigned long int twistColor = 0xffff0000;	// the color of the twisting line

	int xscale=0, yscale=0;								// initial coordinates of the mouse when the user starts scaling
	bool scaling=false;									// this is true/false corresponding to if the user is currently scaling the fractal
	double scaleInitial = 0.5;							// this is the initial scale value when the user began scaling
	double scaleMultiplierPerPixel = 1.5/((float)WINDOW_WIDTH);	// this is how much the scale changes with the mouse pixel movement
	//----------------------------------------------------
	// initialize lots of stuff
	//----------------------------------------------------
    if( init() == false ) return 1;				// make sure you can boot up the necessary libraries
    if( load_files() == false ) return 2;		// make sure all files are loaded corre


    //----------------------------------------------------
	// testing fractal utilities
	//----------------------------------------------------
    struct fractalData myfractal;
    myfractal.scale = 0.5;
    myfractal.iterations = 3;
    myfractal.thickness = 5;
    myfractal.color1 = 0xff00ff00;
    fractal_random(&myfractal, 6, 5);
    bool ctrl=false;
    //bool shift;
    //bool alt;


    //----------------------------------------------------
	// enter the main while loop of the game.
	//----------------------------------------------------
    while(1){

    	//While there's an event to handle
    	while( SDL_PollEvent( &event ) ){

    		//If the user has Xed out the window
    		if( event.type == SDL_QUIT || quit == true ){
				//Quit the program
				quit_game(0);
			}

            if( event.type == SDL_MOUSEBUTTONDOWN ){						/// mouse down
				x = event.motion.x;
				y = event.motion.y;
                if( event.button.button == SDL_BUTTON_LEFT ){
                    mouseLeft[0] = true;
                    fractal_editor(NULL, &myfractal, x, y, ee_left_click_down);
                }
                else if( event.button.button == SDL_BUTTON_RIGHT ){
                    mouseRight[0] = true;
					fractal_editor(NULL, &myfractal, x, y, ee_right_click_down);
                }
            }
            else if(event.type == SDL_MOUSEWHEEL){
				// adjust the iterations of the fractal based on how many turns the user's mouse wheel has gone.
                myfractal.iterations += event.wheel.y;
            }
            else if(event.type == SDL_MOUSEBUTTONUP){						/// mouse up
				x = event.motion.x;
				y = event.motion.y;
				if(event.button.button == SDL_BUTTON_LEFT){
					mouseLeft[0] = false;
				}
				else if(event.button.button == SDL_BUTTON_RIGHT){
					fractal_editor(NULL, &myfractal, x, y, ee_right_click_up);
					mouseRight[0] = false;
				}
            }
            else if( event.type == SDL_MOUSEMOTION ){						/// mouse motion
				x = event.motion.x;
				y = event.motion.y;
				// update twist

            }
            /*else if(event.type == SDL_WINDOWEVENT_RESIZE){					/// window resize

//				float new_cell_size = CELL_SIZE * event.resize.h/((float)WINDOW_HEIGHT); // adjust the pixel size.
//				if(new_cell_size - ((int)new_cell_size) >= 0.5f) CELL_SIZE = new_cell_size + 1;
//				else CELL_SIZE = new_cell_size;
				WINDOW_WIDTH = event.resize.w;
				WINDOW_HEIGHT = event.resize.h;
				set_window_size(event.resize.w, event.resize.h);		// set window to correct dimensions
				windowRect.w = event.resize.w;
				windowRect.h = event.resize.h;
				twistRadiansPerPixel = (2*PI)/WINDOW_WIDTH;	// recalculate the twist per pixels scaler
			}*/

            if( event.type == SDL_KEYDOWN ){		///keyboard event
                switch( event.key.keysym.sym ){
				case SDLK_F5:	fractal_random(&myfractal, 6, 5); break;
				case SDLK_e: 	fractal_editor(NULL,&myfractal,x,y,ee_toggle); break;
				case SDLK_w:	fractal_wobble(&myfractal, vw_toggle); break;
				case SDLK_RCTRL:
				case SDLK_LCTRL: ctrl = true; break;
				case SDLK_s:
					if(ctrl){
						fractal_save_windows(&myfractal);
					}
					else{
						xscale = x;
						yscale = y;
						scaleInitial = myfractal.scale;
						scaling = true;
					}
					break;
				case SDLK_l: if(ctrl)fractal_load_windows(&myfractal); break;
				case SDLK_t:
					xtwist = x;
					ytwist = y;
					twistInitial = myfractal.twist;
					twisting = true;
					break;
				default: break;
				}
			}
			if( event.type == SDL_KEYUP ){								///keyboard event
                switch( event.key.keysym.sym ){
				case SDLK_RCTRL:
				case SDLK_LCTRL: ctrl = false; break;
				case SDLK_t:
					myfractal.twist = twistInitial + (y-ytwist)*twistRadiansPerPixel;
					twisting = false;
					break;
				case SDLK_s:
					myfractal.scale = scaleInitial*(1 - (yscale-y)*scaleMultiplierPerPixel);
					scaling = false;
					break;
				default: break;
				}
			}


    	} // end while(event)
		//no more events to handle at the moment.


		//draw_line(texture, -200,10,200,50,1,0xffff0000);
		//draw_line(texture, (int)(y*0.9),(int)(x*0.9),x,y,1,0xffff0000);
		//draw_line(texture, 1200,200,x,y,1,0xffff0000);
        //draw_circle(texture, 300.0, 300.0, 50, 0x00000000);


        //draw_line(texture, 200, 1200, 200,-200,1,0xffff0000);

        /*
        float theta;
        lines += 1;
        for(theta=0; theta<2*PI; theta = theta + 2*PI/lines){
			draw_line(texture,WINDOW_WIDTH/2,WINDOW_HEIGHT/2,WINDOW_WIDTH*cos(theta)+WINDOW_WIDTH/2, WINDOW_WIDTH*sin(theta)+WINDOW_HEIGHT/2, 1, 0xff0000ff);
        }
        SDL_Delay(50);
        */
		fractal_wobble(&myfractal, vw_evaluate);
        fractal_print(texture, &myfractal);
        fractal_editor(texture, &myfractal, x, y, ee_print);
        // print the twisting line
        if(twisting){

			myfractal.twist = twistInitial + (y-ytwist)*twistRadiansPerPixel;
			draw_line(texture, x, ytwist, x, y, 1, twistColor);
			while(myfractal.twist<0) myfractal.twist += 2*PI;
			while(myfractal.twist>2*PI) myfractal.twist -= 2*PI;

			char degStr[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
			double degrees = 180*myfractal.twist/PI;
			int n; // this is how many whole number digits there are in degrees.

			itoa(((int)degrees), degStr, 10);
			if(degrees < 10){
				n = 1;
			}
			else if(degrees < 100){
				n = 2;
			}
			else {
				n = 3;
			}

			degStr[n] = '.';

			degStr[n+1] = (degrees*10) - ((int)degrees)*10 + 48;

			strcat(&degStr[n+2], " degrees");

			apply_text(texture, x, (y+ytwist)/2, font16, degStr, colorRed);
        }
        if(scaling){
			myfractal.scale = scaleInitial*(1 - (yscale-y)*scaleMultiplierPerPixel);
        }
        
        SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

        //----------------------------------------------------------------------
		// FPS calculation and variable handling
		//----------------------------------------------------------------------
        currentTicks = SDL_GetTicks();
        // it is officially the next second
        if(currentTicks >= ticksSinceLastFPSUpdate + 1000){
			// calculate the FPS
			FPS = cumulativeFrames;//(cumulativeFrames*1000 ) / (currentTicks-ticksSinceLastFPSUpdate);
			cumulativeFrames=0;				// reset cumulative amount of frames
			ticksSinceLastFPSUpdate = currentTicks;	// reset the last FPS update to the number of ticks now.
        }
        cumulativeFrames++;
        mouseRight[1] = mouseRight[0];	// make the previous mouse state equal the current one (for the next loop iteration)
        mouseLeft[1]  = mouseLeft[0];	// ^


    }// end while(quit == false)


    // quit the game
    quit_game(0);

    return 0;
}
