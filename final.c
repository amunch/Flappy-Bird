//final.c
//"Floppy Bird", a flappy bird clone.
//By: Andy Munch

#define _BSD_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "gfx4.h"

#define XSIZE 800
#define YSIZE 800 //Size of the graphics window.
//Declared score as a global variable for easier access in all parts of the program
static int score_num = 0, score_time = 50;
static char score_string[10];

int draw_screen(double *x_pos, double *y_pos, double *time_since_rise, double pipe_center[][5]);
void draw_background();
void snowflake(int x, int y, int l);
void draw_bird(double *x_pos, double *y_pos, double time_since_rise); 
void bird_rise(double *x_pos, double *y_pos, double pipe_center[][5]);
int bird_check(int y_check, int side_length);
void initialize_pipe_array(double pipe_center[][5]);
void pipe_position(double pipe_center[][5]);
void score(double *x_pos, double pipe_center[][5]);
int death_check(double *x_pos, double *y_pos, double pipe_center[][5]);
int death_sequence(double *x_pos, double *y_pos, double pipe_center[][5]);
void reset(double *x_pos, double *y_pos, double pipe_center[][5]);
int opening_screen();
void draw_pipe(int i, double pipe_center[][5]);
void paint_bird(double x_pos, double y_pos);

int main(void) {
	char c; //Hold the user input.
	int i, j;
	int loop = 1, death = 0, do_next = 0;
	double x_pos = 200, y_pos = 400;
	double time_since_rise = 0; //Time since the user has pressed space.
	double pipe_center[5][5];
	//Open the graphics window.
	gfx_open(XSIZE, YSIZE, "Floppy Bird");
	
	gfx_clear();
	//Initialize the array that will hold the position of the pipes.
	initialize_pipe_array(pipe_center);
	//Display the loading screen	
	opening_screen();
	//Loops until the user presses q to quit
	while(loop) {
		gfx_clear();
		//If death = 0, the bird has gone in an invalid position.
		death = draw_screen(&x_pos, &y_pos, &time_since_rise, pipe_center);	
		
      		gfx_flush();

		if(death) {
			//do_next tells main what the user entered at the game over screen.
			do_next = death_sequence(&x_pos, &y_pos, pipe_center);
			if(do_next) { //If they hit to continue, reset the game
                        	reset(&x_pos, &y_pos, pipe_center);
                	} //Else, terminate the program.
               		else {
                        	return 0;
                	}	
		}
	
		if(gfx_event_waiting()) {
			c = gfx_wait();
			if(c == 'q') { 
				loop = 0; //Terminate the program.
			}
			if(c == ' ') { //Make the bird jump, time_since_rise is used for velocity.
				bird_rise(&x_pos, &y_pos, pipe_center);
				time_since_rise = 0;
			}
		}
	}
	return 0;
}

int draw_screen(double *x_pos, double *y_pos, double *time_since_rise, double pipe_center[][5]) {
	gfx_clear();
	draw_background(); //Draw the Christmas themed background first.
	draw_bird(x_pos, y_pos, *time_since_rise); //Draw the bird over the background.
	*time_since_rise += .008; //Used to find the velocity of bird due to acceleration.
	//This function passes the pipe array to the function to scroll the pipes across the screen.
	pipe_position(pipe_center); //Will, in another function, draw the pipes. 
	//If the bird is in an invalid position, lead to the death sequence.
	if(death_check(x_pos, y_pos, pipe_center)) {
		return 1;
	}
	//Due to integer rounding, I was having problems with scores being double counted.
	//This assures that the score is not doublecounted.
	if(score_time >= 50) {
		score(x_pos, pipe_center); //Check if the score needs to be updated.
        }
	gfx_changefont("-adobe-utopia-bold-i-normal--33-240-100-100-p-186-iso8859-14");
        gfx_color(0, 0, 0);
        snprintf(score_string, 10, "%d", score_num); //Print the score over everything.
        gfx_text(380, 100, score_string);
	score_time += 1; //Used for delay in checking if the user has scored another point.

	gfx_flush();
	
	usleep(8000); //8000 was a good speed for Flappy Bird.
	return 0;
}

void draw_background() {
	gfx_color(153, 204, 255); //Pale blue / grey sky.
        gfx_fill_rectangle(0, 0, XSIZE, YSIZE);
        gfx_color(229, 232, 218); //Grey ground.
        gfx_fill_rectangle(0, 760, XSIZE, 40);
	snowflake(50, 50, 20); //A few snowflakes drawn recursively, slightly grey like the ground.
	snowflake(100, 300, 50);
	snowflake(100, 700, 40);
	snowflake(600, 400, 50);
	snowflake(700, 100, 40);
	snowflake(250, 500, 50);
	snowflake(300, 100, 40);
	snowflake(700, 600, 30);
}

void snowflake(int x, int y, int l) {
	int i;
	double theta = M_PI / 10;
	//Base Case
	if(l < 3) {
		return;
	}
	//Draw the lines
	for(i = 0; i < 5; i++) {
		theta += 2 * M_PI / 5; //Draw 5 lines radially outward
		gfx_line(x, y, x + l * cos(theta), y + l * sin(theta));
	}
	theta = M_PI / 10;
	//Recursive calls
	for(i = 0; i < 5; i++) {
		theta += 2 * M_PI / 5; //Draw five lines around the recursively defined point
		snowflake(x + l * cos(theta), y + l * sin(theta), l / 3);
	}
}
void draw_bird(double *x_pos, double *y_pos, double time_since_rise) {
	gfx_color(255, 255, 255);
	double velocity, acceleration = 30; //Acceleration tweaked for a good falling amount.
	int side_length = 50;	
	int y_check;

	y_check = *y_pos; //Used to check if the bird is at the top or bottom of the class.	

	velocity = acceleration * time_since_rise; //Velocity of the bird downward.
	
	*y_pos += velocity * .2; //New position of the bird based on velocity.
	
	if(bird_check(y_check, side_length) == 1) { //Case where the bird is at the top of the screen.
		*y_pos = 0;
	}
	else if(bird_check(y_check, side_length) == 2) { //Case where the bird is at the bottom of the screen.
		*y_pos = 800 - side_length;
	}
	
	paint_bird(*x_pos, *y_pos); //Finally, draw the bird at the x and y position.
}

void bird_rise(double *x_pos, double *y_pos, double pipe_center[][5]) {
	int i, side_length = 20;
	int y_check;	

	y_check = *y_pos; //

	for(i = 0; i < 10; i += 1) {
		double time = -2; 

		*y_pos -= 2; //Make the bird move upwards every iteration.
		//Make sure the bird does not go out of bounds.
		if(bird_check(y_check, side_length) == 1) {
			*y_pos = 0; //Set the bird at the edge if it did go out of bounds
		}
		else if(bird_check(y_check, side_length) == 2) {
			*y_pos = 800 - side_length;
		}
		//Draw the screen every iteration of the jump.
		draw_screen(x_pos, y_pos, &time, pipe_center);		
	}
}

int bird_check(int y_check, int side_length) {
	if(y_check < 0){
		return 1; //Return 1 or 2, depending on the top or bottom, if the bird goes out the window.
	}
	else if(y_check > (800 - side_length)) {
		return 2;
	}
	
	return 0;
}

void pipe_position(double pipe_center[][5]) {
	int deltax = 2, space = 400;
	int i;
	//Move the pipes 2 pixels to the left.
	for(i = 0; i < 5; i++) {
		pipe_center[i][0] -= deltax;
	}
	//Draw every pipe.
	for(i = 0; i < 5; i++) {
		draw_pipe(i, pipe_center);
	}
	//Regenerate the pipe if it goes out of the screen.
	for(i = 0; i < 5; i++) {
		if(pipe_center[i][0] < -100) {
			if(i == 0) {
				pipe_center[0][0] = pipe_center[4][0] + 300;
				pipe_center[i][1] = rand() % (500 + 1 - 100) + 100;
				pipe_center[i][2] = pipe_center[i][1] + 200; //Top left position of bottom 
                		pipe_center[i][3] = 800 - pipe_center[i][1] - 200;
			}
			else {
				pipe_center[i][0] = pipe_center[i - 1][0] + 300;
				pipe_center[i][1] = rand() % (500 + 1 - 100) + 100;
				pipe_center[i][2] = pipe_center[i][1] + 200; //Top left position of bottom 
                		pipe_center[i][3] = 800 - pipe_center[i][1] - 200;
			}
		}
	}
	gfx_flush();
}

void score(double *x_pos, double pipe_center[][5]) {
	int i;
	for(i = 0; i < 5; i++) {
		if(score_time < 50) {
			break;
		}
		else { //If the bird passes the pipe's x position, increment the score 1.
			if((int)*x_pos == (int)pipe_center[i][0]) {
				score_num += 1;
				score_time = 0;
			}
		}
	}
}

void initialize_pipe_array(double pipe_center[][5]) {
	int i, space = 300;
	//Make the pipe heights random.	
	for(i = 0; i < 5; i++) {
		pipe_center[i][0] = 900 + i * space; //x position of the pipe
		pipe_center[i][1] = rand() % (500 + 1 - 100) + 100; //Height of the top pipe.
	}
	
	for(i = 0; i < 5; i++) {
		pipe_center[i][2] = pipe_center[i][1] + 200; //Top left y position of bottom 
		pipe_center[i][3] = 800 - pipe_center[i][1] - 200; //Height of bottom.
	}
}	

int death_check(double *x_pos, double *y_pos, double pipe_center[][5]) {
	int i;
	//Check if the bird has entered an invalid position.	
	for(i = 0; i < 5; i++) { //Check every pipe and make sure the bird has not crossed it.
		if(*x_pos > pipe_center[i][0] - 5 && *x_pos < pipe_center[i][0] + 105) {
			if(*y_pos < pipe_center[i][1] || *y_pos > pipe_center[i][2] - 20) {
				return 1;
			}
		}
	} //Make sure the bird has not hit the ground 
	if(*y_pos + 50 > 800) {
		return 1;
	}

	return 0;
}

int death_sequence(double *x_pos, double *y_pos, double pipe_center[][5]) {
        double ypos = 770, time = 0;
	char c;
	//Sequence to be played when the bird dies.	
	gfx_clear();
	//Flash the screen white
	gfx_color(255, 255, 255);
        gfx_fill_rectangle(0, 0, XSIZE, YSIZE);
        gfx_flush();
        usleep(200000);
	draw_screen(x_pos, &ypos, &time, pipe_center);
	//Show the dead bird on the ground	
	gfx_color(0, 0, 0); //Write the score in the screen and game over.
        gfx_text(60, 100, "YOUR SCORE WAS: ");
	gfx_text(380, 100, score_string);
	gfx_text(60, 300, "GAME OVER");
        gfx_text(60, 350, "PRESS Q TO QUIT");
        gfx_text(60, 400, "PRESS SPACE TO PLAY AGAIN");	

	gfx_flush();
	
	score_time = 50;
	score_num = 0; //Reset the score to 0.

	c = gfx_wait();	
	switch(c) { //Wait for the user to input to quit or restart.
		case 1:
			return 1;
		case ' ':
			return 1;
		default:
			return 0;	
	}
}

void reset(double *x_pos, double *y_pos, double pipe_center[][5]) {
	int i, j;

	srand(time(NULL));
	//Reset the position of the bird
	*x_pos = 200;
	*y_pos = 200;
	//Reset the position and length of the pipes.
	initialize_pipe_array(pipe_center);
}

int opening_screen() {
	char c;
	double x = 100, y = 100;	
	int loop = 1, sign = 1;
	//Opening sequence for the game.
	while(loop) {
		draw_background();	

		gfx_changefont("-adobe-utopia-bold-i-normal--33-240-100-100-p-186-iso8859-14");		
		//Write the title of the game
		gfx_color(0, 0, 0);
		gfx_text(300, 100, "FLOPPY BIRD");
		gfx_text(300, 150, "PRESS SPACE TO START");
		//Make the bird move up and down.
		y = y + sign;
		paint_bird(x, y);
		//Reverse the direction of the bird.
		if(y > 700 || y < 100) {
			sign = -sign;
		}

		usleep(8000);
		gfx_flush();
		//Once the user presses space, start the program		
		if(gfx_event_waiting()) {
			c = gfx_wait();
			if(c == ' '){
				loop = 0;
			}
		}
	}	
	return 0;
}
void draw_pipe(int i, double pipe_center[][5]) {
	//For the top pipe, pipe_center[i][0], 0 are the coordinates of the top left point.
	//pipe_center[i][1] is the height, 100 is the width.
	//For the bottom pipe, pipe_center[i][0], pipe_center[i][2] are the coordinates.
	//100 is the width, pipe_center[i][3] is the height.
	
	//Top pipe.
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(pipe_center[i][0], 0, 5, pipe_center[i][1]);
	gfx_color(153, 255, 204);
	gfx_fill_rectangle(pipe_center[i][0] + 5, 0, 5, pipe_center[i][1]);
	gfx_color(178, 255, 102);
	gfx_fill_rectangle(pipe_center[i][0] + 10, 0, 5, pipe_center[i][1]);
	gfx_color(153, 255, 204);
	gfx_fill_rectangle(pipe_center[i][0] + 15, 0, 15, pipe_center[i][1]);
	gfx_color(0, 255, 0);
	gfx_fill_rectangle(pipe_center[i][0] + 30, 0, 5, pipe_center[i][1]);
	gfx_color(153, 255, 204);
	gfx_fill_rectangle(pipe_center[i][0] + 35, 0, 5, pipe_center[i][1]);
	gfx_color(0, 255, 0);
	gfx_fill_rectangle(pipe_center[i][0] + 40, 0, 35, pipe_center[i][1]);
	gfx_color(0, 153, 0);
	gfx_fill_rectangle(pipe_center[i][0] + 75, 0, 5, pipe_center[i][1]);
	gfx_color(153, 255, 204);
	gfx_fill_rectangle(pipe_center[i][0] + 80, 0, 5, pipe_center[i][1]);
	gfx_color(0, 153, 0);
	gfx_fill_rectangle(pipe_center[i][0] + 85, 0, 10, pipe_center[i][1]);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(pipe_center[i][0] + 95, 0, 5, pipe_center[i][1]);

	//Bottom pipe.
	gfx_color(0, 0, 0);
        gfx_fill_rectangle(pipe_center[i][0], pipe_center[i][2], 5, pipe_center[i][3]);
        gfx_color(153, 255, 204);
        gfx_fill_rectangle(pipe_center[i][0] + 5, pipe_center[i][2], 5, pipe_center[i][3]);
        gfx_color(178, 255, 102);
        gfx_fill_rectangle(pipe_center[i][0] + 10, pipe_center[i][2], 5, pipe_center[i][3]);
        gfx_color(153, 255, 204);
        gfx_fill_rectangle(pipe_center[i][0] + 15, pipe_center[i][2], 15, pipe_center[i][3]);
        gfx_color(0, 255, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 30, pipe_center[i][2], 5, pipe_center[i][3]);
        gfx_color(153, 255, 204);
        gfx_fill_rectangle(pipe_center[i][0] + 35, pipe_center[i][2], 5, pipe_center[i][3]);
        gfx_color(0, 255, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 40, pipe_center[i][2], 35, pipe_center[i][3]);
        gfx_color(0, 153, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 75, pipe_center[i][2], 5, pipe_center[i][3]);
        gfx_color(153, 255, 204);
        gfx_fill_rectangle(pipe_center[i][0] + 80, pipe_center[i][2], 5, pipe_center[i][3]);
        gfx_color(0, 153, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 85, pipe_center[i][2], 10, pipe_center[i][3]);
        gfx_color(0, 0, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 95, pipe_center[i][2], 5, pipe_center[i][3]);

	//Top pipe ridge
	gfx_color(0, 0, 0);
        gfx_fill_rectangle(pipe_center[i][0] - 5, pipe_center[i][1] - 50, 5, 50);
        gfx_color(153, 255, 204);
        gfx_fill_rectangle(pipe_center[i][0], pipe_center[i][1] - 50, 5, 50);
        gfx_color(178, 255, 102);
        gfx_fill_rectangle(pipe_center[i][0] + 5, pipe_center[i][1] - 50, 5, 50);
        gfx_color(153, 255, 204);
        gfx_fill_rectangle(pipe_center[i][0] + 10, pipe_center[i][1] - 50, 15, 50);
        gfx_color(0, 255, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 25, pipe_center[i][1] - 50, 5, 50);
        gfx_color(153, 255, 204);
        gfx_fill_rectangle(pipe_center[i][0] + 30, pipe_center[i][1] - 50, 5, 50);
        gfx_color(0, 255, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 35, pipe_center[i][1] - 50, 45, 50);
        gfx_color(0, 153, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 80, pipe_center[i][1] - 50, 5, 50);
        gfx_color(153, 255, 204);
        gfx_fill_rectangle(pipe_center[i][0] + 85, pipe_center[i][1] - 50, 5, 50);
        gfx_color(0, 153, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 90, pipe_center[i][1] - 50, 10, 50);
        gfx_color(0, 0, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 100, pipe_center[i][1] - 50, 5, 50);
	gfx_fill_rectangle(pipe_center[i][0] - 5, pipe_center[i][1], 110, 5);
	gfx_fill_rectangle(pipe_center[i][0] - 5, pipe_center[i][1] - 50, 110, 5);

	//Bottom pipe ridge
	gfx_color(0, 0, 0);
        gfx_fill_rectangle(pipe_center[i][0] - 5, 800 - pipe_center[i][3], 5, 50);
        gfx_color(153, 255, 204);
        gfx_fill_rectangle(pipe_center[i][0], 800 - pipe_center[i][3], 5, 50);
        gfx_color(178, 255, 102);
        gfx_fill_rectangle(pipe_center[i][0] + 5, 800 - pipe_center[i][3], 5, 50);
        gfx_color(153, 255, 204);
        gfx_fill_rectangle(pipe_center[i][0] + 10, 800 - pipe_center[i][3], 15, 50);
        gfx_color(0, 255, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 25, 800 - pipe_center[i][3], 5, 50);
        gfx_color(153, 255, 204);
        gfx_fill_rectangle(pipe_center[i][0] + 30, 800 - pipe_center[i][3], 5, 50);
        gfx_color(0, 255, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 35, 800 - pipe_center[i][3], 45, 50);
        gfx_color(0, 153, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 80, 800 - pipe_center[i][3], 5, 50);
        gfx_color(153, 255, 204);
        gfx_fill_rectangle(pipe_center[i][0] + 85, 800 - pipe_center[i][3], 5, 50);
        gfx_color(0, 153, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 90, 800 - pipe_center[i][3], 10, 50);
        gfx_color(0, 0, 0);
        gfx_fill_rectangle(pipe_center[i][0] + 100, 800 - pipe_center[i][3], 5, 50);
        gfx_fill_rectangle(pipe_center[i][0] - 5, 800 - pipe_center[i][3], 110, 5);
        gfx_fill_rectangle(pipe_center[i][0] - 5, 800 - pipe_center[i][3] + 50, 110, 5);
}

void paint_bird(double x_pos, double y_pos) {
	//Draw the bird at the given x and y position.
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 18, y_pos, 18, 3);
	gfx_fill_rectangle(x_pos + 12, y_pos + 3, 6, 3);
	gfx_color(255, 255, 51);
	gfx_fill_rectangle(x_pos + 18, y_pos + 3, 9, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 27, y_pos + 3, 3, 3);
	gfx_color(255, 255, 255);
	gfx_fill_rectangle(x_pos + 30, y_pos + 3, 6, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 36, y_pos + 3, 3, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 9, y_pos + 6, 3, 3);
	gfx_color(255, 255, 51);
	gfx_fill_rectangle(x_pos + 12, y_pos + 6, 6, 3);
	gfx_color(255, 249, 98);
	gfx_fill_rectangle(x_pos + 18, y_pos + 6, 6, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 24, y_pos + 6, 3, 3);
	gfx_color(255, 255, 255);
	gfx_fill_rectangle(x_pos + 27, y_pos + 6, 12, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 39, y_pos + 6, 3, 3);
	gfx_fill_rectangle(x_pos + 3, y_pos + 9, 12, 3);
	gfx_color(255, 249, 98);
	gfx_fill_rectangle(x_pos + 15, y_pos + 9, 9, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 24, y_pos + 9, 3, 3);
	gfx_color(229, 232, 218);
	gfx_fill_rectangle(x_pos + 27, y_pos + 9, 3, 3);
	gfx_color(255, 255, 255);
	gfx_fill_rectangle(x_pos + 30, y_pos + 9, 6, 3);
	gfx_fill_rectangle(x_pos + 39, y_pos + 9, 3, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 36, y_pos + 9, 3, 3);
	gfx_fill_rectangle(x_pos + 42, y_pos + 9, 3, 3);
	gfx_fill_rectangle(x_pos, y_pos + 12, 3, 3);
	gfx_color(255, 255, 210);
	gfx_fill_rectangle(x_pos + 3, y_pos + 12, 12, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 15, y_pos + 12, 3, 3);
	gfx_color(255, 249, 98);
	gfx_fill_rectangle(x_pos + 18, y_pos + 12, 6, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 24, y_pos + 12, 3, 3);
	gfx_fill_rectangle(x_pos + 36, y_pos + 12, 3, 3);
	gfx_fill_rectangle(x_pos + 42, y_pos + 12, 3, 3);
	gfx_color(229, 232, 218);
	gfx_fill_rectangle(x_pos + 27, y_pos + 12, 3, 3);
	gfx_color(255, 255, 255);
	gfx_fill_rectangle(x_pos + 30, y_pos + 12, 6, 3);
	gfx_fill_rectangle(x_pos + 39, y_pos + 12, 3, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos, y_pos + 15, 3, 3);
	gfx_fill_rectangle(x_pos + 18, y_pos + 15, 3, 3);
	gfx_fill_rectangle(x_pos + 27, y_pos + 15, 3, 3);
	gfx_fill_rectangle(x_pos + 42, y_pos + 15, 3, 3);
	gfx_color(255, 255, 210);
	gfx_fill_rectangle(x_pos + 3, y_pos + 15, 15, 3);
	gfx_color(255, 249, 98);
	gfx_fill_rectangle(x_pos + 21, y_pos + 15, 6, 3);
	gfx_color(229, 232, 218);
	gfx_fill_rectangle(x_pos + 30, y_pos + 15, 3, 3);
	gfx_color(255, 255, 255);
	gfx_fill_rectangle(x_pos + 33, y_pos + 15, 9, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos, y_pos + 18, 3, 3);
	gfx_fill_rectangle(x_pos + 18, y_pos + 18, 3, 3);
	gfx_fill_rectangle(x_pos + 30, y_pos + 18, 18, 3);
	gfx_color(255, 255, 51);
	gfx_fill_rectangle(x_pos + 3, y_pos + 18, 3, 3);
	gfx_fill_rectangle(x_pos + 15, y_pos + 18, 3, 3);
	gfx_color(255, 255, 210);
	gfx_fill_rectangle(x_pos + 6, y_pos + 18, 9, 3);
	gfx_color(255, 249, 98);
	gfx_fill_rectangle(x_pos + 21, y_pos + 18, 9, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 3, y_pos + 21, 3, 3);
	gfx_fill_rectangle(x_pos + 15, y_pos + 21, 3, 3);
	gfx_fill_rectangle(x_pos + 27, y_pos + 21, 3, 3);
	gfx_fill_rectangle(x_pos + 48, y_pos + 21, 3, 3);
	gfx_color(255, 255, 51);
	gfx_fill_rectangle(x_pos + 6, y_pos + 21, 9, 3);
	gfx_color(255, 249, 98);
	gfx_fill_rectangle(x_pos + 18, y_pos + 21, 9, 3);
	gfx_color(255, 100, 100);
	gfx_fill_rectangle(x_pos + 30, y_pos + 21, 18, 3);	
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 6, y_pos + 24, 9, 3);
	gfx_fill_rectangle(x_pos + 24, y_pos + 24, 3, 3);
	gfx_fill_rectangle(x_pos + 30, y_pos + 24, 18, 3);
	gfx_color(255, 100, 100);
	gfx_fill_rectangle(x_pos + 27, y_pos + 24, 3, 3);
	gfx_color(255, 153, 51);
	gfx_fill_rectangle(x_pos + 15, y_pos + 24, 9, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 6, y_pos + 27, 3, 3);
	gfx_fill_rectangle(x_pos + 27, y_pos + 27, 3, 3);
	gfx_fill_rectangle(x_pos + 45, y_pos + 27, 3, 3);
	gfx_color(255, 153, 51);
	gfx_fill_rectangle(x_pos + 9, y_pos + 27, 18, 3);
	gfx_color(255, 100, 100);
	gfx_fill_rectangle(x_pos + 30, y_pos + 27, 15, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 9, y_pos + 30, 6, 3);
	gfx_fill_rectangle(x_pos + 30, y_pos + 30, 15, 3);
	gfx_color(255, 153, 51);
	gfx_fill_rectangle(x_pos + 15, y_pos + 30, 15, 3);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x_pos + 15, y_pos + 33, 15, 3);
}

