#include "loader.h"
#include "movement.h"
#include "scans.h"

/**
 * Scan the dimensions for a board from the given file into the correct
 * members of the inputted stats structure.
 */
int scan_dimensions(FILE *level, struct stats *board) {

	// Scan the dimension of the board from the file into memory
	if (fscanf(level, " %hd %hd ", &board->size.num_rows, &board->size.num_cols) != 2) {
		fclose(level);
		return 1;
	}
	if (board->size.num_rows < MIN_BOARD_Y || board->size.num_rows > MAX_BOARD_Y
	    || board->size.num_cols < MIN_BOARD_X || board->size.num_cols > MAX_BOARD_X) {
		fclose(level);
		return 1;
	}

	return 0;
}

/**
 * Scan the exit location for a board from the given file into the correct
 * members of the inputted stats structure.
 */
int scan_exit(FILE *level, struct stats *board) {

	// Scan the relative position of the exit square from the file into memory
	if (fscanf(level, " %hd %hd %hd ", &board->exit.relation.row, &board->exit.relation.col, &board->exit.location) != 3) {
		fclose(level);
		return 1;
	}
	if (board->exit.relation.row < 0 || board->exit.relation.row >= board->size.num_rows
	    || board->exit.relation.col < 0 || board->exit.relation.col >= board->size.num_cols) {
		fclose(level);
		return 1;
	}

	// Make sure the scanned relative location is valid
	switch (board->exit.location) {
		case LEFT:
			if (board->exit.relation.col != 0) {
				fclose(level);
				return 1;
			}
			break;

		case RIGHT:
			if (board->exit.relation.col != (board->size.num_cols - 1)) {
				fclose(level);
				return 1;
			}
			break;

		case UP:
			if (board->exit.relation.row != 0) {
				fclose(level);
				return 1;
			}
			break;

		case DOWN:
			if (board->exit.relation.row != (board->size.num_rows - 1)) {
				fclose(level);
				return 1;
			}
			break;

		default:
			fclose(level);
			return 1;
	}

	return 0;
}

/**
 * Scan the board position of Theseus from the given file into the correct
 * members of the inputted stats structure.
 */
int scan_theseus(FILE *level, struct stats *board) {

	// Scan the starting position of Theseus on the board from the file into memory
	if (fscanf(level, " %hd %hd ", &board->theseus.row, &board->theseus.col) != 2) {
		fclose(level);
		return 1;
	}
	if (board->theseus.row < 0 || board->theseus.row >= board->size.num_rows
	    || board->theseus.col < 0 || board->theseus.col >= board->size.num_cols) {
		fclose(level);
		return 1;
	}

	return 0;
}

/**
 * Scan the board position of the Minotaur from the given file into the correct
 * members of the inputted stats structure.
 */
int scan_minotaur(FILE *level, struct stats *board) {

	// Scan the starting position of the Minotaur on the board from the file into memory
	if (fscanf(level, " %hd %hd ", &board->minotaur.row, &board->minotaur.col) != 2) {
		fclose(level);
		return 1;
	}
	if (board->minotaur.row < 0 || board->minotaur.row >= board->size.num_rows
	    || board->minotaur.col < 0 || board->minotaur.col >= board->size.num_cols) {
		fclose(level);
		return 1;
	}
	else if (board->minotaur.row == board->theseus.row && board->minotaur.col == board->theseus.col) {
		fclose(level);
		return 1;
	}

	return 0;
}

/**
 * Scan the board position each of wall from the given file into the correct
 * members of the inputted stats structure.
 */
int scan_walls(FILE *level, struct stats *board) {

	// Start a linked-list of Walls and check for invalid data
	int grab_val;
	if ((grab_val = grab_wall(level, &board->walls, board->size.num_rows, board->size.num_cols)) == 2) {
		board->walls->next = NULL;
		free_walls(board->walls);
		board->walls = NULL;

		fclose(level);
		return 1;
	}
	board->walls->next = NULL;

	if (grab_val == 1) {
		free_walls(board->walls);
		board->walls = NULL;

		return 0;
	}

	// Scan the rest of the walls from the file into the linked-list
	cell_rel *tracker = board->walls;
	while (true) {
		if ((grab_val = grab_wall(level, &tracker, board->size.num_rows, board->size.num_cols)) == 2) {
			free_walls(board->walls);
			board->walls = NULL;

			fclose(level);
			return 1;
		}
		else if (grab_val == 1) {
			free(tracker->next);
			tracker->next = NULL;
			break;
		}

		tracker = tracker->next;
	}

	return 0;
}
