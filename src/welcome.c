#include "welcome.h"

/**
 * Draw a menu WINDOW to the screen with any number of menu items to choose from.
 * Highlight the current menu item as the user presses the up and down arrow keys.
 * Return the item number (starting from 0) that the user selects.
 *
 * 'height' specifies the height for the menu WINDOW.
 * 'width' specifies the width for the menu WINDOW.
 * 'title' specifies a title for the menu WINDOW.
 * 'size_array' is the number of strings that 'opt_strings' argument holds.
 * 'opt_strings' is an array of strings to use as the menu items.
 * 'num_extra' is the number of extra menu items to add to the menu.
 *
 * Return Value:
 *	The function returns the selected menu item number (starting from 0).
 */
int show_menu(int height, int width, const char *title, int size_array, char **opt_strings, int num_extra, ...) {
	WINDOW *menu;
	int key, cur_choice = 0;
	char item_options[size_array + num_extra][MAX_OPT_LENGTH + 1];

	va_list extra_items;		/* Define a variable argument list to hold extra menu items */

	// Initialize a WINDOW to hold the menu
	refresh();
	menu = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
	box(menu, 0, 0);
	mvwprintw(menu, 1, (width - strlen(title)) / 2, "%s", title);

	// Draw all the menu options to the WINDOW
	wattron(menu, A_REVERSE);
	for(int i = 0; i < size_array; i++) {
		strncpy(item_options[i], opt_strings[i], sizeof(char) * MAX_OPT_LENGTH);
		item_options[i][MAX_OPT_LENGTH] = '\0';

		mvwprintw(menu, i + 3, PADDING_LEFT, "%s", item_options[i]);
		if (i == 0) wattroff(menu, A_REVERSE);
	}

	va_start(extra_items, num_extra);	/* Initialize the varaible argument list with arguments coming after 'num_extra' */
	for(int i = 0; i < num_extra; i++) {
		strncpy(item_options[size_array + i], va_arg(extra_items, char *), sizeof(char) * MAX_OPT_LENGTH);
		item_options[size_array + i][MAX_OPT_LENGTH] = '\0';

		mvwprintw(menu, size_array + i + 3, PADDING_LEFT, "%s", item_options[size_array + i]);
	}
	va_end(extra_items);
	wrefresh(menu);

	// Allow user to select any of the options from the menu
	while ((key = getch()) != '\n') {
		
		switch (key) {
			case KEY_UP:
				mvwchgat(menu, cur_choice + 3, PADDING_LEFT, strlen(item_options[cur_choice]), A_NORMAL, 0, NULL);
				cur_choice = (cur_choice == 0) ? (size_array + num_extra) - 1 : cur_choice - 1;

				mvwchgat(menu, cur_choice + 3, PADDING_LEFT, strlen(item_options[cur_choice]), A_REVERSE, 0, NULL);
				wrefresh(menu);
				break;

			case KEY_DOWN:
				mvwchgat(menu, cur_choice + 3, PADDING_LEFT, strlen(item_options[cur_choice]), A_NORMAL, 0, NULL);
				cur_choice = (cur_choice + 1) % (size_array + num_extra);

				mvwchgat(menu, cur_choice + 3, PADDING_LEFT, strlen(item_options[cur_choice]), A_REVERSE, 0, NULL);
				wrefresh(menu);
				break;
		}
	}
	delwin(menu);
	clear();

	return cur_choice;
}

/**
 * Show a manual page for how to play the Theseus and the Minotaur Game, depending on
 * the page number entered as an argument.
 *
 * 'page' specifies which manual page to display to the user.
 *
 * Return Value:
 *	Function returns the number (starting from 0) of the menu option selected from
 *	the current manual page.
 *
 * Error Values:
 *	-1 - The specified manual page does not exist.
 */
int show_manual(int page) {
	WINDOW *manual_page;
	int height, width;

	refresh();

	// Show Manual Page #1
	if (page == 1) {
		const char *title = "Introduction";
		height = 17;
		width = 80;

		// Initialize the manual page WINDOW and draw it to the screen
		manual_page = create_window(height, width, 3, 11,
				"The goal of the game is to maneuver Theseus through the maze to the exit",
				"without getting caught by the Minotaur.",
				"",
				"Rules:",
				"\t- Theseus gets one move per turn.",
				"\t- The Minotaur gets two moves per turn.",
				"\t- Neither character can move through walls.",
				"",
				"Important - Even though the Minotaur can make two moves per turn, he is",
				"            quite stupid and is unable to move around walls, causing him",
				"            to get stuck in certain places quite easily."
		);
		mvwprintw(manual_page, 1, (width - strlen(title)) / 2, "%s", title);

		// Add menu options and wait for user to select one
		return add_options(manual_page, 2, "Back to Menu", "Next");
	}
	
	// Show Manual Page #2
	else if (page == 2) {
		const char *title = "Characters and Symbols";
		height = 14;
		width = 75;

		manual_page = create_window(height, width, 3, 7,
				"                        ///\\\\\\            And finally...",
				"Meet Theseus! -        (  OO  )           The Exit! -        * EXIT *",
				"                        \\ -- /",
				"",
				"                        <(--)>",
				"The Minotaur! -          |**|",
				"                          \\/"

		);
		mvwprintw(manual_page, 1, (width - strlen(title)) / 2, "%s", title);

		return add_options(manual_page, 3, "Prev", "Back to Menu", "Next");
	}

	// Show Manual Page #3
	else if (page == 3) {
		const char *title = "Controls";
		height = 16;
		width = 80;

		manual_page = create_window(height, width, 3, 9,
				"Move Theseus through the maze using the arrow keys. You can skip a turn",
				"by pressing the space bar, which is actually quite useful at times.",
				"",
				"To return to the Main Menu:     Press \"m\".",
				"To quit the game:               Press \"q\".",
				"To restart the level:           Press \"r\".",
				"",
				"Ok, so it looks like you know everything to play the game. Now go",
				"help Theseus escape from the Minotaur!"
		);
		mvwprintw(manual_page, 1, (width - strlen(title)) / 2, "%s", title);

		return add_options(manual_page, 2, "Prev", "Back to Menu");
	}

	return -1;
}

/**
 * Initialize a WINDOW of specified dimensions and print some inputted strings of text
 * to a number of the WINDOW's lines.
 *
 * 'win_height' specifies the height for the new WINDOW.
 * 'win_width' specifies the width for the new WINDOW.
 * 'start_line' signals which line to start printing given
 *
 * Return Value:
 *	Function return a pointer to a WINDOW if successful.
 *	Otherwise function returns NULL.
 */
WINDOW *create_window(int win_height, int win_width, int start_line, int num_lines, ...) {
	WINDOW *win;

	va_list xlines;
	va_start(xlines, num_lines);

	// Initalize a WINDOW with specified dimensions at the center of the screen
	win = newwin(win_height, win_width, (LINES - win_height) / 2, (COLS - win_width) / 2);
	if (win == NULL) return (WINDOW *)0x00;
	box(win, 0, 0);

	// Print each inputted string of text to a new line
	for(int i = 0; i < num_lines; i++)
		mvwprintw(win, start_line + i, PADDING_LEFT, "%s", va_arg(xlines, char *));
	va_end(xlines);

	return win;
}

/**
 * Add a specified number of menu options to a give WINDOW and wait for the user
 * to choose one, by navigating with the left and right arrow keys. The function
 * deletes the current WINDOW and clears the screen when finished. Note that the
 * 'num_options' parameter must be less than MAX_MAN_OPTIONS and less than
 * MIN_MAN_OPTIONS. These two constants are by default set to 1 and 3 respectively
 * and shouldn't be changed without altering the actual function as well to support
 * different numbers of menu options.
 *
 * 'win' specifies which WINDOW to add menu options to.
 * 'num_options' specifies how many menu options to add the the WINDOW.
 *
 * Return Value:
 *	Function returns the number (starting from 0) of the menu option selected.
 */
int add_options(WINDOW *win, int num_options, ...) {
	int opt_lengths[num_options], opt_positions[num_options];
	int cur_choice = num_options - 1;
	
	// Make sure only valid number of options was inputted
	if (num_options < MIN_MAN_OPTIONS || num_options > MAX_MAN_OPTIONS) return -1;

	va_list option_items;
	va_start(option_items, num_options);

	// Grab information from each argument in variable argument list
	for(int i = 0; i < num_options; i++)
		opt_lengths[i] = strlen(va_arg(option_items, char *));		/* Get the length of current argument */
	va_end(option_items);

	// Get the dimensions of the WINDOW
	int max_y, max_x, key;
	getmaxyx(win, max_y, max_x);

	// Set the starting positions of each menu option based on how many there are
	if (num_options == 1)
		opt_positions[0] = (max_x - opt_lengths[0]) / 2;
	
	else if (num_options == 2) {
		opt_positions[0] = PADDING_LEFT;
		opt_positions[1] = max_x - PADDING_LEFT - opt_lengths[1];
	}
	else {
		opt_positions[0] = PADDING_LEFT;
		opt_positions[1] = (max_x - opt_lengths[1]) / 2;
		opt_positions[2] = max_x - PADDING_LEFT - opt_lengths[2];
	}

	// Print each menu option to the WINDOW (highlight last)
	va_start(option_items, num_options);
	for(int i = 0; i < num_options; i++) {
		if (i == (num_options - 1)) wattron(win, A_REVERSE);
		mvwprintw(win, max_y - 2, opt_positions[i], "%s", va_arg(option_items, char *));
		if (i == (num_options - 1)) wattroff(win, A_REVERSE);
	}
	va_end(option_items);
	wrefresh(win);

	// If there's more than one menu option, let user select one
	if (num_options != 1) {
		while ((key = getch()) != '\n') {
		
			switch (key) {
				case KEY_LEFT:
					if (cur_choice != 0) {
						mvwchgat(win, max_y - 2, opt_positions[cur_choice], opt_lengths[cur_choice], A_NORMAL, 0, NULL);
						cur_choice--;

						mvwchgat(win, max_y - 2, opt_positions[cur_choice], opt_lengths[cur_choice], A_REVERSE, 0, NULL);
						wrefresh(win);
					}
					break;

				case KEY_RIGHT:
					if (cur_choice != (num_options - 1)) {
						mvwchgat(win, max_y - 2, opt_positions[cur_choice], opt_lengths[cur_choice], A_NORMAL, 0, NULL);
						cur_choice++;

						mvwchgat(win, max_y - 2, opt_positions[cur_choice], opt_lengths[cur_choice], A_REVERSE, 0, NULL);
						wrefresh(win);
					}
					break;
			}
		}
	}

	// Otherwise wait for user to hit ENTER
	else while ((key = getch()) != '\n');

	// Delete the current WINDOW and clear the screen
	delwin(win);
	clear();

	return cur_choice;
}
