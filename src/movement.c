#include "board.h"
#include "movement.h"

// An array of numbers with which to build and manipulate bit_masks for valid moves on the board
const short moves[NUM_MOVES] = {1, 2, 4, 8};

/**
 * Take in an array of board_square structures and a stats structure, holding
 * the board information, and compute bit-masks for each board_square structure.
 * These bit-masks determine the moves for each position on the board that are
 * valid. This function turns off moves that are going off the board, and also
 * moves between walls on both sides of each wall.
 *
 * 'board' specifies the stats structure that holds the board information.
 * 'win_grid' specifies the array of board_square structures to modify.
 */
void set_moves(struct stats *board, board_square *win_grid) {

	// Turn off moves that go off the board
	for(int i = 0; i < board->size.num_rows; i++) {
		if (i == 0 || i == (board->size.num_rows - 1)) {
			int bit_index = (i == 0) ? UP : DOWN;
			
			for(int j = 0; j < board->size.num_cols; j++) {
				win_grid[(i * board->size.num_cols) + j].move_mask &= ~(moves[bit_index]);
				
				// Turn off two bits (moves) of move_mask for corners of the board
				if (j == 0 || j == (board->size.num_cols - 1)) {
					int bit_index_corner = (j == 0) ? LEFT : RIGHT;
					win_grid[(i * board->size.num_cols) + j].move_mask &= ~(moves[bit_index_corner]);
				}
			}
		}
		else {
			int bit_index = LEFT;
			for(int j = 0; j < board->size.num_cols; j += (board->size.num_cols - 1)) {
				win_grid[(i * board->size.num_cols) + j].move_mask &= ~(moves[bit_index]);
				bit_index = RIGHT;
			}
		}
	}

	// Loop through linked-list of walls and turn off moves through walls on both sides of each wall
	for(cell_rel *temp = board->walls; temp != NULL; temp = temp->next) {
		win_grid[(temp->relation.row * board->size.num_cols) + temp->relation.col].move_mask &= ~(moves[temp->location]);
		
		switch (temp->location) {
			case LEFT:
				if (temp->relation.col > 0)
					win_grid[(temp->relation.row * board->size.num_cols) + temp->relation.col - 1].move_mask &= ~(moves[RIGHT]);
				break;

			case RIGHT:
				if (temp->relation.col < (board->size.num_cols - 1))
					win_grid[(temp->relation.row * board->size.num_cols) + temp->relation.col + 1].move_mask &= ~(moves[LEFT]);
				break;

			case UP:
				if (temp->relation.row > 0)
					win_grid[((temp->relation.row - 1) * board->size.num_cols) + temp->relation.col].move_mask &= ~(moves[DOWN]);
				break;

			case DOWN:
				if (temp->relation.row < (board->size.num_rows - 1))
					win_grid[((temp->relation.row + 1) * board->size.num_cols) + temp->relation.col].move_mask &= ~(moves[UP]);
				break;
		}
	}
	win_grid[(board->exit.relation.row * board->size.num_cols) + board->exit.relation.col].move_mask |= moves[board->exit.location];

	return;
}

/**
 * Move Theseus to a new WINDOW based on an inputted move direction. If the move is invalid for the
 * current square, then no move is made. If the move specified puts Theseus in the exit WINDOW, the
 * function handles the copying of the display image for Theseus over to the given WINDOW supplied
 * for 'exit' parameter. If the move specified puts Theseus in the same position as the Minotaur, then
 * the display image for Theseus is erased from the current WINDOW, but not copied over to the new
 * WINDOW. The position for Theseus is still updated, however.
 *
 * 'board' is a structure holding the inforamtion for the board.
 * 'win_grid' specifies an array containing a WINDOW and possible moves for each square on the board.
 * 'exit' specifies the exit WINDOW for the game.
 * 'win_pair' specifies the color pair of the current WINDOW occupied by the Minotaur.
 * 'move' is the direction which to move Theseus on the board.
 *
 * Return Values:
 *	0 - No move was made for Theseus (invalid move).
 *	1 - A move was made to an ordinary square on the board.
 *	2 - A move was made to the exit WINDOW.
 *	3 - A move was made to a square occupied by the Minotaur (display image not copied over).
 */
int move_theseus(struct stats *board, board_square *win_grid, WINDOW *exit, short *win_pair, short move) {
	int theseus_pos = (board->theseus.row * board->size.num_cols) + board->theseus.col;
	int minotaur_pos = (board->minotaur.row * board->size.num_cols) + board->minotaur.col;
	int exit_pos = (board->exit.relation.row * board->size.num_cols) + board->exit.relation.col;

	// Make sure the move is valid
	if (!(win_grid[theseus_pos].move_mask & moves[move])) return 0;

	// Erase the image from the WINDOW currently occupied by Theseus
	win_draw_image(win_grid[theseus_pos].win, eraser, ERASER_SIZE, *win_pair, *win_pair);
	*win_pair = (*win_pair == PAIR_1) ? PAIR_2 : PAIR_1;

	// Check if Theseus is moving to the exit WINDOW
	if (theseus_pos == exit_pos && board->exit.location == move) {
		win_draw_image(exit, theseus_image, THESEUS_SIZE, THESEUS_PAIR, *win_pair);
		doupdate();
		return 2;
	}

	// Update the position of Theseus based on the move chosen
	switch (move) {
		case LEFT:
			board->theseus.col--;
			theseus_pos--;
			break;

		case RIGHT:
			board->theseus.col++;
			theseus_pos++;
			break;

		case UP:
			board->theseus.row--;
			theseus_pos -= board->size.num_cols;
			break;

		case DOWN:
			board->theseus.row++;
			theseus_pos += board->size.num_cols;
			break;
	}

	// Check if Theseus moved to the Minotaur's position
	if (theseus_pos == minotaur_pos) {
		doupdate();
		return 3;
	}

	// Draw Theseus to the new WINDOW
	win_draw_image(win_grid[theseus_pos].win, theseus_image, THESEUS_SIZE, THESEUS_PAIR, *win_pair);
	doupdate();

	return 1;
}

/**
 * Given statistics of the board and possible moves for each of the board's squares, check
 * if the Minotaur can make any moves, and if so then move him to that square and display the
 * move to the screen. Note that the Minotaur will only move if there is a valid move in the
 * direction of Theseus.
 *
 * 'board' holds information about the board.
 * 'win_grid' specifies an array containing a WINDOW and possible moves for each square on the board.
 * 'win_pair' specifies the color pair of the current WINDOW occupied by the Minotaur.
 *
 * Return Values:
 *	0 - No move was made.
 *	1 - A move was made, but Theseus was not caught.
 *	2 - A move was made and Theseus has been caught.
 */
int move_minotaur(struct stats *board, board_square *win_grid, short *win_pair) {
	int minotaur_pos = (board->minotaur.row * board->size.num_cols) + board->minotaur.col;
	bool move_made = false;
	bool theseus_caught = false;
	
	// Loop through all different moves possible on a board
	for(int i = 0; i < NUM_MOVES; i++) {

		// Check if move can be made by the Minotaur
		if (win_grid[minotaur_pos].move_mask & moves[i]) {

			// Don't move unless toward Theseus
			switch (i) {
				case LEFT:
					if (board->minotaur.col > board->theseus.col) {
						win_draw_image(win_grid[minotaur_pos].win, eraser, ERASER_SIZE, *win_pair, *win_pair);

						*win_pair = (*win_pair == PAIR_1) ? PAIR_2 : PAIR_1;	/* Adjust the color pair value for the next WINDOW to be occupied by the Minotaur */
						board->minotaur.col--;
						minotaur_pos--;

						if (board->minotaur.row == board->theseus.row
						    && board->minotaur.col == board->theseus.col) {
							win_draw_image(win_grid[minotaur_pos].win, eraser, ERASER_SIZE, *win_pair, *win_pair);
							theseus_caught = true;
						}

						win_draw_image(win_grid[minotaur_pos].win, minotaur_image, MINOTAUR_SIZE, MINOTAUR_PAIR, *win_pair);
						doupdate();
						move_made = true;
					}
					break;

				case RIGHT:
					if (board->minotaur.col < board->theseus.col) {
						win_draw_image(win_grid[minotaur_pos].win, eraser, ERASER_SIZE, *win_pair, *win_pair);

						*win_pair = (*win_pair == PAIR_1) ? PAIR_2 : PAIR_1;
						board->minotaur.col++;
						minotaur_pos++;

						if (board->minotaur.row == board->theseus.row
						    && board->minotaur.col == board->theseus.col) {
							win_draw_image(win_grid[minotaur_pos].win, eraser, ERASER_SIZE, *win_pair, *win_pair);
							theseus_caught = true;
						}

						win_draw_image(win_grid[minotaur_pos].win, minotaur_image, MINOTAUR_SIZE, MINOTAUR_PAIR, *win_pair);
						doupdate();
						move_made = true;
					}
					break;

				case UP:
					if (board->minotaur.row > board->theseus.row) {
						win_draw_image(win_grid[minotaur_pos].win, eraser, ERASER_SIZE, *win_pair, *win_pair);

						*win_pair = (*win_pair == PAIR_1) ? PAIR_2 : PAIR_1;
						board->minotaur.row--;
						minotaur_pos -= board->size.num_cols;

						if (board->minotaur.row == board->theseus.row
						    && board->minotaur.col == board->theseus.col) {
							win_draw_image(win_grid[minotaur_pos].win, eraser, ERASER_SIZE, *win_pair, *win_pair);
							theseus_caught = true;
						}

						win_draw_image(win_grid[minotaur_pos].win, minotaur_image, MINOTAUR_SIZE, MINOTAUR_PAIR, *win_pair);
						doupdate();
						move_made = true;
					}
					break;

				case DOWN:
					if (board->minotaur.row < board->theseus.row) {
						win_draw_image(win_grid[minotaur_pos].win, eraser, ERASER_SIZE, *win_pair, *win_pair);

						*win_pair = (*win_pair == PAIR_1) ? PAIR_2 : PAIR_1;
						board->minotaur.row++;
						minotaur_pos += board->size.num_cols;

						if (board->minotaur.row == board->theseus.row
						    && board->minotaur.col == board->theseus.col) {
							win_draw_image(win_grid[minotaur_pos].win, eraser, ERASER_SIZE, *win_pair, *win_pair);
							theseus_caught = true;
						}

						win_draw_image(win_grid[minotaur_pos].win, minotaur_image, MINOTAUR_SIZE, MINOTAUR_PAIR, *win_pair);
						doupdate();
						move_made = true;
					}
					break;
			}
		}

		if (theseus_caught) return 2;
		if (move_made) return 1;
	}

	return 0;
}
