#include "game.h"

/**
 * Take in a string value representing a file path for a level, and start a Theseus and
 * the Minotaur Game. If the contents of the level file pointed to by the given file path
 * are valid, then the game will be initialized with details from the file. Otherwise an
 * error code will be returned. The game will stop when the user presses the key defined by
 * the global constant - 'EXIT'. The game will also stop if the user wins or loses the game.
 * The user plays from the point of view of Theseus, and tries to escape the Minotaur by
 * reaching the exit square.
 *
 * 'file_path' specifies the file path for a level file to be used in the game.
 * 'last_level' specifies whether the current level file is the last in the sequence.
 *
 * Return Values:
 *	0 - The user quit the game.
 *	1 - The user wants to return to the Main Menu (lost game).
 *	2 - The user wants to return to the Main Menu (won game).
 *	3 - The user wants to continue to the next level.
 *	4 - The user wants to start a new game.
 *
 * Error Codes:
 *	5 - The inputted value for 'file_path' was NULL.
 *	6 - The file specified by 'file_path' could not be opened for reading.
 *	7 - The data from the file specified by 'file_path' was invalid.
 *	8 - The game quit unexpectedly (user didn't win, lose, or press EXIT key).
 */
int play_game(const char *file_path, bool last_level) {

	// Make sure that the inputted file path isn't NULL
	if (file_path == NULL) return 5;

	// Load data from the file into a stats structure
	struct stats board_stats;
	board_stats.walls = NULL;
	int result = read_level_file(file_path, &board_stats);

	// Return proper error value if there was a load failure
	if (result != 0) {
		if (result == 1) return 6;
		else return 7;
	}

	int num_squares = board_stats.size.num_rows * board_stats.size.num_cols;

	int theseus_board_pos = (board_stats.theseus.row * board_stats.size.num_cols) + board_stats.theseus.col;
	int minotaur_board_pos = (board_stats.minotaur.row * board_stats.size.num_cols) + board_stats.minotaur.col;
	int exit_board_pos = (board_stats.exit.relation.row * board_stats.size.num_cols) + board_stats.exit.relation.col;

	WINDOW *exit_win;
	short theseus_win_pair, minotaur_win_pair, exit_win_pair;
	bool move_made, escaped, caught;
	int key, mod_key, theseus_move_result, minotaur_move_result;

	// Allocate memory for an array of WINDOW pointers
	board_square *wins = malloc(sizeof(board_square) * num_squares);

	// Initialize color pairs for the board
	init_pair(PAIR_1, COLOR_BLACK, COLOR_WHITE);
	init_pair(PAIR_2, COLOR_BLACK, COLOR_CYAN);

	init_pair(THESEUS_PAIR, COLOR_BLACK, COLOR_BLACK);
	init_pair(MINOTAUR_PAIR, COLOR_RED, COLOR_BLACK);
	init_pair(EXIT_PAIR, COLOR_MAGENTA, COLOR_BLACK);

	// Initialize variables to keep track of the pair values of the current WINDOWs occupied by Theseus, the Minotaur, and the Exit
	if (board_stats.theseus.row & 1)
		theseus_win_pair = (board_stats.theseus.col & 1) ? PAIR_1 : PAIR_2;
	else theseus_win_pair = (board_stats.theseus.col & 1) ? PAIR_2 : PAIR_1;

	if (board_stats.minotaur.row & 1)
		minotaur_win_pair = (board_stats.minotaur.col & 1) ? PAIR_1 : PAIR_2;
	else minotaur_win_pair = (board_stats.minotaur.col & 1) ? PAIR_2 : PAIR_1;

	if (board_stats.exit.relation.row & 1)
		exit_win_pair = (board_stats.exit.relation.col & 1) ? PAIR_2 : PAIR_1;
	else exit_win_pair = (board_stats.exit.relation.col & 1) ? PAIR_1 : PAIR_2;

	// Draw the board to the screen
	refresh();
	win_layout(wins, board_stats.size.num_rows, board_stats.size.num_cols, HEIGHT, WIDTH);
	
	for(cell_rel *temp = board_stats.walls; temp != NULL; temp = temp->next)
		draw_wall(wins[(temp->relation.row * board_stats.size.num_cols) + temp->relation.col].win, temp->location);

	exit_win = place_win(wins[exit_board_pos].win, board_stats.exit.location, HEIGHT, WIDTH);
	win_draw_image(exit_win, exit_image, EXIT_SIZE, EXIT_PAIR, exit_win_pair);

	win_draw_image(wins[theseus_board_pos].win, theseus_image, THESEUS_SIZE, THESEUS_PAIR, theseus_win_pair);
	win_draw_image(wins[minotaur_board_pos].win, minotaur_image, MINOTAUR_SIZE, MINOTAUR_PAIR, minotaur_win_pair);
	doupdate();

	// Disable moves through walls and off the board
	set_moves(&board_stats, wins);

	escaped = false;
	caught = false;

	// Accept input (moves/commands) from the user
	while (true) {
		move_made = false;
		
		// Detect the key press and check if EXIT key was pressed
		mod_key = (key = getch()) | ('a' - 'A');
		if (mod_key == EXIT || mod_key == RESTART || mod_key == MAIN_MENU) {

			// Show correct confirmation message based on key press
			if (mod_key == EXIT) {
				if (show_message(0, 0, "Are you sure you want to quit?", " YES ", " NO ")) break;
			}
			else if (mod_key == RESTART) {
				if (show_message(0, 50, "Are you sure you want to restart the level?", " YES ", " NO ")) break;
			}
			else if (show_message(0, 55, "Are you sure you want to return to the Main Menu?", " YES ", " NO ")) break;
			
			// Reprint the board to the screen
			refresh();
			for(int i = 0; i < num_squares; i++) {
				touchwin(wins[i].win);			/* Mark the entire WINDOW as changed (causes all of the WINDOW to be redrawn) */
				wnoutrefresh(wins[i].win);
			}
			touchwin(exit_win);
			wnoutrefresh(exit_win);
			doupdate();
			
			continue;
		}
		
		// Determine which key the user pressed
		switch (key) {
			case KEY_LEFT:
				if ((theseus_move_result = move_theseus(&board_stats, wins, exit_win, &theseus_win_pair, M_LEFT)) == 1)
					move_made = true;

				else if (theseus_move_result == 2) escaped = true;
				else if (theseus_move_result == 3) caught = true;

				break;

			case KEY_RIGHT:
				if ((theseus_move_result = move_theseus(&board_stats, wins, exit_win, &theseus_win_pair, M_RIGHT)) == 1)
					move_made = true;

				else if (theseus_move_result == 2) escaped = true;
				else if (theseus_move_result == 3) caught = true;

				break;

			case KEY_UP:
				if ((theseus_move_result = move_theseus(&board_stats, wins, exit_win, &theseus_win_pair, M_UP)) == 1)
					move_made = true;

				else if (theseus_move_result == 2) escaped = true;
				else if (theseus_move_result == 3) caught = true;

				break;

			case KEY_DOWN:
				if ((theseus_move_result = move_theseus(&board_stats, wins, exit_win, &theseus_win_pair, M_DOWN)) == 1)
					move_made = true;

				else if (theseus_move_result == 2) escaped = true;
				else if (theseus_move_result == 3) caught = true;

				break;

			case SKIP_TURN:
				move_made = true;
				break;
		}

		// Check if Theseus escaped (user won)
		if (escaped || caught) break;

		// Skip the Minotaur's move if no move was made
		if (!move_made) continue;

		usleep(PAUSE_TIME);

		// Determine and make the Minotaur's two moves
		if ((minotaur_move_result = move_minotaur(&board_stats, wins, &minotaur_win_pair)) == 1) {
			usleep(PAUSE_TIME);
			
			if ((minotaur_move_result = move_minotaur(&board_stats, wins, &minotaur_win_pair)) == 2) {
				caught = true;
				break;
			}
		}
		else if (minotaur_move_result == 2) {
			caught = true;
			break;
		}
	}

	// Clear the virtual screen and free all allocated memory
	for(int i = 0, n = board_stats.size.num_rows * board_stats.size.num_cols; i < n; i++)
		delwin(wins[i].win);
	free(wins);
	delwin(exit_win);

	free_walls(board_stats.walls);

	// Return the correct value based on the results of the game
	if (mod_key == EXIT) return 0;
	if (mod_key == RESTART) {
		clear();
		return play_game(file_path, last_level);
	}
	if (mod_key == MAIN_MENU) {
		clear();
		return 1;
	}

	usleep(PAUSE_TIME);
	if (caught) {
		if (show_message(0, 0, "Ouch! You lost!", "Return to Menu", "Restart Game")) {
			clear();
			return 1;
		}
		clear();

		return play_game(file_path, last_level);
	}
	else if (escaped) {
		if (!last_level) {
			if (show_message(0, 0, "Congratulations! You Won!", "Return to Menu", "Next Level")) {
				clear();
				return 2;
			}
			clear();
			
			return 3;
		}
		else if (show_message(7, 60, "Congratulations! You Won the Game! Theseus Escaped!", "Return to Menu", "New Game")) {
			clear();
			return 1;
		}
		clear();

		return 4;
	}
	clear();

	return 8;
}

/**
 * Display a message WINDOW to the screen, and prompt the user for a "yes or no" decision where
 * they can switch between the options using the left and right arrow keys. The message and options
 * to be chosen from are inputted as the function parameters.
 *
 * 'win_height' specifies the height of the message WINDOW.
 * 'win_width' specifies the width of the message WINDOW.
 * 'title' is a string of text to prompt the user for a choice.
 * 'true_choice' is the option that will cause the function to return true if chosen.
 * 'false_choice' is the option that will cause the function to return false if chosen.
 *
 * Return Values:
 *	true - 'true_choice' was selected by the user.
 *	false - 'false_choice' was selected by the user.
 */
bool show_message(int win_height, int win_width, const char *title, const char *true_choice, const char *false_choice) {
	int key;
	bool cur_choice = false;

	// Set default for WINDOW height and width if they're zero or less
	if (win_width <= 0)
		win_width = MESSAGE_WIDTH;
	if (win_height <= 0)
		win_height = MESSAGE_HEIGHT;

	// Create the message WINDOW and draw a box around it
	refresh();
	WINDOW *win = newwin(win_height, win_width, (LINES - win_height) / 2, (COLS - win_width) / 2);
	box(win, 0, 0);

	// Print the title and choices to the WINDOW
	mvwprintw(win, 1, (win_width - strlen(title)) / 2, "%s", title);
	mvwprintw(win, win_height - 3, (win_width - strlen(true_choice)) / 4, "%s", true_choice);

	wattron(win, A_REVERSE);
	mvwprintw(win, win_height - 3, ((win_width - strlen(false_choice)) * 3) / 4, "%s", false_choice);
	wattroff(win, A_REVERSE);

	wrefresh(win);
	
	// Select the option that is chosen by the user
	while ((key = getch()) != '\n') {
		
		switch (key) {
			case KEY_LEFT:
				if (!cur_choice) {
					mvwchgat(win, win_height - 3, ((win_width - strlen(false_choice)) * 3) / 4, strlen(false_choice), A_NORMAL, 0, NULL);
					mvwchgat(win, win_height - 3, (win_width - strlen(true_choice)) / 4, strlen(true_choice), A_REVERSE, 0, NULL);
					wrefresh(win);

					cur_choice = !cur_choice;
				}
				break;

			case KEY_RIGHT:
				if (cur_choice) {
					mvwchgat(win, win_height - 3, (win_width - strlen(true_choice)) / 4, strlen(true_choice), A_NORMAL, 0, NULL);
					mvwchgat(win, win_height - 3, ((win_width - strlen(false_choice)) * 3) / 4, strlen(false_choice), A_REVERSE, 0, NULL);
					wrefresh(win);

					cur_choice = !cur_choice;
				}
				break;
		}
	}

	// Erase the message WINDOW
	wclear(win);
	wnoutrefresh(win);
	delwin(win);

	return cur_choice;
}
