#ifndef _BOARD_H
#define _BOARD_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEIGHT 5
#define WIDTH 12

#define PAIR_1 1
#define PAIR_2 2
#define THESEUS_PAIR 3
#define MINOTAUR_PAIR 4
#define EXIT_PAIR 5

#define THESEUS_SIZE 3
#define MINOTAUR_SIZE 3
#define EXIT_SIZE 1
#define ERASER_SIZE 3

// Structure to hold a WINDOW in the board and a bit-mask of valid moves
typedef struct {
	WINDOW *win;
	unsigned char move_mask;
}
board_square;

// ASCII character image display for Theseus
extern const char *theseus_image[];

// ASCII character image display for the Minotaur
extern const char *minotaur_image[];

// ASCII character image display for the Exit
extern const char *exit_image[];

// An array of blank strings with which to erase WINDOW images
extern const char *eraser[];

/**
 * Create a grid of WINDOWs centered on the screen. The WINDOWs can be set to
 * have alternating background colors by using init_pair() function to initialize
 * PAIR_1 and PAIR_2 constants to different background color values. Or one color
 * can be set for all WINDOWs by initializing the pairs to the same color values.
 * Default value for move_mask element of board_square structures are also initialized.
 *
 * 'win_grid' specifies an array of uninitialized board_square structure pointers to initialize.
 * 'rows' specifies the number of rows of WINDOWs to create.
 * 'cols' specifies the number of columns to create in each row of WINDOWs.
 * 'win_height' specifies the height for each WINDOW.
 * 'win_width' specifies the width for each WINDOW.
 */
void win_layout(board_square *win_grid, short rows, short cols, short win_height, short win_width);

/**
 * Create a "wall" by reversing the foreground and background colors of
 * a specified position of a WINDOW. The "wall" is basically just a color
 * difference and is only visible if the foreground and background colors
 * of the specified WINDOW are different.
 *
 * 'win' specifies the WINDOW to create the "wall" on.
 * 'placement' is an integer that specifies where to place the "wall".
 *
 * Acceptable Values for Placement:
 *      0 - Wall is on left side of WINDOW.
 *      1 - Wall is on right side of WINDOW.
 *      2 - Wall is on upper side of WINDOW.
 *      3 - Wall is on lower side of WINDOW.
 */
void draw_wall(WINDOW *win, short placement);

/**
* Take in an array of strings, print those strings to a WINDOW, and center them 
* vertically and horizontally. Note that only as many characters will be printed to the
* WINDOW as can fit inside a one-character width padding on each side of the WINDOW.
* All other characters in the inputted array of strings will be disregarded. A color pair
* for the inputted array of strings to be formatted in can be specified by assigning the
* 'pair' argument a value of an already initialized color pair. Only the foreground color
* of the 'pair' argument is used. The background color comes from the 'win_pair' color
* pair argument.
*
* 'win' specifies the WINDOW to print the image on.
* 'image' specifies an array of strings to print to the WINDOW.
* 'img_height' is the number of lines in the array of strings.
* 'pair' specifies which color pair to format the image in.
* 'win_pair' is the color pair for the WINDOW (used to find the background color).
*/
void win_draw_image(WINDOW *win, const char **image, short img_height, short pair, short win_pair);

/**
 * Place a new WINDOW on a specified side of a WINDOW currently on the board. The color
 * pattern of the board is maintained by giving the new WINDOW a background color pair
 * different than that of it's neighboring WINDOW (i.e PAIR_1 if neighbor has PAIR_2 and
 * vice versa).
 *
 * 'neighbor' specifies the WINDOW to place a new WINDOW in relation to.
 * 'placement' specifies which side of the neighboring WINDOW to place the new WINDOW on.
 * 'win_height' specifies the height for the new WINDOW.
 * 'win_width' specifies the width for the new WINDOW.
 *
 * Acceptable Values for Placement:
 *      0 - Left of the neighboring WINDOW.
 *      1 - Right of the neighboring WINDOW.
 *      2 - Above the neighboring WINDOW.
 *      3 - Below the neighboring WINDOW.
 */
WINDOW *place_win(WINDOW *neighbor, short placement, short win_height, short win_width);

#endif    // _BOARD_H
