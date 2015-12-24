/*
A simple graphics library for CSE 20211 
Originally created by Prof. Doug Thain.
Modified Fall 2014 by Prof. Ramzi Bualuan
Edited Fall 2015 by Prof. Ramzi Bualuan
New functions for gfx2.h added 10/28/15 by Prof. Bualuan
New functions for gfx3.h added 11/5/15 by Prof. Bualuan
New functions for gfx4.h added 11/14/15 by Prof. Bualuan
*/

#ifndef GFX_H
#define GFX_H

#include <X11/Xlib.h>

/* Open a new graphics window. */
void gfx_open( int width, int height, const char *title );

/* Flush all previous output to the window. */
void gfx_flush();

/* Change the current drawing color. */
void gfx_color( int red, int green, int blue );

/* Clear the graphics window to the background color. */
void gfx_clear();

/* Change the current background color. */
void gfx_clear_color( int red, int green, int blue );

/* Check to see if an event is waiting. */
int gfx_event_waiting();

/* Wait for the user to press a key or mouse button. */
char gfx_wait();

/* Return the X and Y coordinates of the last event. */
int gfx_xpos();
int gfx_ypos();

/* Return the X and Y dimensions of the screen (monitor). */
int gfx_xsize();
int gfx_ysize();

/* Display a string at (x,y) */
void gfx_text( int x, int y , char *text );

/* Draw a point at (x,y) */
void gfx_point( int x, int y );

/* Draw a line from (x1,y1) to (x2,y2) */
void gfx_line( int x1, int y1, int x2, int y2 );

/* Draw a circle centered at (xc,yc) with radius r */
void gfx_circle( int xc, int yc, int r );

// new gfx2 functions added 10/28/15:

/* Draw an ellipse centered at (xc,yc) with radii r1 and r2 */
void gfx_ellipse( int xc, int yc, int r1, int r2 );

/* Draw an arc whose top left corner of its bounding rectangle is at (xc,yc),
 * with width w and height h, starting at angle a1 and sweeping an angle of a2 (degrees);
 * a1 is at the 3 O'Clock position, and the a2 sweep is positive counter-clockwise */
void gfx_arc( int xc, int yc, int w, int h, int a1, int a2 );

/* Draw a filled arc; similar to gfx_arc */
void gfx_fill_arc( int xc, int yc, int w, int h, int a1, int a2 );

/* Draw a rectangle with top-left corner at (x,y) with width w and height h */
void gfx_rectangle( int x, int y, int w, int h );

/* Draw a filled rectangle; similar to gfx_rectangle */
void gfx_fill_rectangle( int x, int y, int w, int h );

// new gfx3 functions added 11/5/15:

/* Draw a polygon whose corners ("num_pts" of them) are in the "pointsarr" array
 * (uses Xlib's XPoint struct) */
void gfx_polygon( XPoint *pointsarr, int num_pts );

/* Draw a filled polygon */
void gfx_fill_polygon( XPoint *pointsarr, int num_pts );

/* Change the font that gfx_text will use */
void gfx_changefont( char * );

/* Get width and height of screen (prior to opening a window) */
int gfx_screenwidth();
int gfx_screenheight();

// new gfx4 functions added 11/14/15:

/* Get width and height of the display window 
 *   (useful in case it's been resized) */
int gfx_windowwidth();
int gfx_windowheight();

/* get the red, green, or blue value of the current display's color */
int gfx_getred();
int gfx_getgreen();
int gfx_getblue();

/* find the pixel width or height of a string (text) in a given font (fontname) */
int gfx_textpixelwidth( char *text, char *fontname );
int gfx_textpixelheight( char *text, char *fontname );

/* Draw a filled circle centered at (xc,yc) with radius r */
void gfx_fill_circle( int xCtr, int yCtr, int r );

#endif

