#include "board.h"
#include "movement.h"

// ASCII character image display for Theseus
const char *theseus_image[THESEUS_SIZE] = {
	" ///\\\\\\ ",
	"(  OO  )",
	" \\ -- / "
};

// ASCII character image display for the Minotaur
const char *minotaur_image[MINOTAUR_SIZE] = {
	"<(--)>",
	" |**| ",
	"  \\/  "
};

// ASCII character image display for the Exit
const char *exit_image[EXIT_SIZE] = {
	"* EXIT *"
};

// An array of blank strings with which to erase WINDOW images
const char *eraser[ERASER_SIZE] = {
	"          ",
	"          ",
	"          "
};

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
void win_layout(board_square *win_grid, int rows, int cols, int win_height, int win_width) {
	int cur_pair = PAIR_1;

	// Nested 'for' loops to iterate through board_square structures to initialize
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < cols; j++) {

			// Initialize WINDOW with specified height/width at next position on the screen
			win_grid[(i * cols) + j].win = newwin(win_height, win_width, (LINES - ((rows - (2 * i)) * win_height)) / 2, (COLS - ((cols - (2 * j)) * win_width)) / 2);
			wbkgd(win_grid[(i * cols) + j].win, COLOR_PAIR(cur_pair));

			win_grid[(i * cols) + j].move_mask = (moves[M_LEFT] | moves[M_RIGHT] | moves[M_UP] | moves[M_DOWN]);	/* Set default of move_mask element (all moves valid) */

			wnoutrefresh(win_grid[(i * cols) + j].win);		/* Update virtual screen only (causes only one burst of output -- when finished) */
			
			if ((cols & 1) || j != (cols - 1))
				cur_pair = (cur_pair == PAIR_1) ? PAIR_2 : PAIR_1;
		}
	}

	return;
}

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
 *	0 - Wall is on left side of WINDOW.
 *	1 - Wall is on right side of WINDOW.
 *	2 - Wall is on upper side of WINDOW.
 *	3 - Wall is on lower side of WINDOW.
 */
void draw_wall(WINDOW *win, int placement) {
	int max_y, max_x, y_val, x_val;
	getmaxyx(win, max_y, max_x);	    /* Get the maximum coordinates of the current WINDOW */

	// Perform necessary loop based on specified placement
	if (placement == M_LEFT || placement == M_RIGHT) {
		x_val = (placement == M_LEFT) ? 0 : max_x - 1;

		for(int i = 0; i < max_y; i++)
			mvwaddch(win, i, x_val, ' ' | A_REVERSE);
	}
	else if (placement == M_UP || placement == M_DOWN) {
		y_val = (placement == M_UP) ? 0 : max_y - 1;

		for(int i = 0; i < max_x; i++)
			mvwaddch(win, y_val, i, ' ' | A_REVERSE);
	}

	// Update virtual screen only (allows the placement of several walls without multiple bursts of output to the screen)
	wnoutrefresh(win);

	return;
}

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
void win_draw_image(WINDOW *win, const char **image, int img_height, short pair, short win_pair) {
	int max_y, max_x, start_y, end_y, start_x;

	// Adjust background color for Theseus and Minotaur pairs
	if (pair == THESEUS_PAIR || pair == MINOTAUR_PAIR || pair == EXIT_PAIR) {
		short win_fg, win_bg, pair_fg, pair_bg;

		pair_content(win_pair, &win_fg, &win_bg);	/* Extract the foreground and background colors from the WINDOW's color pair */
		pair_content(pair, &pair_fg, &pair_bg);

		init_pair(pair, pair_fg, win_bg);
	}

	getmaxyx(win, max_y, max_x);
	char *img_line = malloc(sizeof(char) * (max_x - 1));

	// Get interval of current WINDOW's rows to print the image lines on (center image vertically)
	start_y = (img_height < (max_y - 1)) ? (max_y - img_height) / 2 : 1;
	end_y = ((start_y + img_height) < max_y) ? start_y + img_height : max_y - 1;
	
	// Print as many lines of the image as possible given the dimensions of the current WINDOW
	wattron(win, COLOR_PAIR(pair));
	for(int i = 0, cur_y = start_y; cur_y < end_y; i++, cur_y++) {
		start_x = (strlen(image[i]) < (max_x - 1)) ? (max_x - strlen(image[i])) / 2 : 1;

		snprintf(img_line, max_x - 2, "%s", image[i]);	    /* Copy a maximum number of characters into a buffer to be printed to the current WINDOW */
		mvwprintw(win, cur_y, start_x, "%s", img_line);
	}
	wattroff(win, COLOR_PAIR(pair));
	free(img_line);

	wnoutrefresh(win);	/* Copy the WINDOW image to the virtual screen only */

	return;
}

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
 *	0 - Left of the neighboring WINDOW.
 *	1 - Right of the neighboring WINDOW.
 *	2 - Above the neighboring WINDOW.
 *	3 - Below the neighboring WINDOW.
 */
WINDOW *place_win(WINDOW *neighbor, int placement, int win_height, int win_width) {
	int start_y, start_x, max_y, max_x;
	int new_start_y, new_start_x;
	short win_pair;
	attr_t attrs;		/* Just to satisfy requirements of wattr_get function */

	// Get the starting coordinates and size of the soon to be adjacent WINDOW
	getbegyx(neighbor, start_y, start_x);
	getmaxyx(neighbor, max_y, max_x);

	// Determine start coordinates of new WINDOW based on inputted relation (left, right, up, down)
	switch (placement) {
		case M_LEFT:
			new_start_y = start_y;
			new_start_x = start_x - win_width;
			break;

		case M_RIGHT:
			new_start_y = start_y;
			new_start_x = start_x + max_x;
			break;

		case M_UP:
			new_start_y = start_y - win_height;
			new_start_x = start_x;
			break;

		case M_DOWN:
			new_start_y = start_y + max_y;
			new_start_x = start_x;
			break;

		default:
			return (WINDOW *)0x00;
	}

	// Maintain the board's color pattern by using a different color pair than adjacent WINDOW
	wattr_get(neighbor, &attrs, &win_pair, NULL);
	win_pair = (win_pair == PAIR_1) ? PAIR_2 : PAIR_1;

	// Create the new WINDOW and print it to the screen
	WINDOW *win = newwin(win_height, win_width, new_start_y, new_start_x);
	wbkgd(win, COLOR_PAIR(win_pair));
	wnoutrefresh(win);		    /* Copy the WINDOW only to the virtual screen */

	return win;
}
