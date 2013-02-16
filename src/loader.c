#include "loader.h"
#include "movement.h"

/**
 * Take in a string of text referencing an external 'level' file for the game. Then
 * scan the entire file into a stats structure, making sure that all scanned data is
 * valid. The function scans in the dimensions for a board, the relative position of
 * an exit square, the starting positions of Theseus and the Minotaur, as well as the
 * relative positions of all "walls" found in the specified file.
 *
 * 'file_path' is a string literal that specifies the path of the file to be scanned.
 * 'board' is a stats structure in which to copy the scanned data.
 *
 * Error Codes:
 *	0 - No error was encountered.
 *	1 - The file specified by the given file path could not be opened.
 *	2 - Invalid board dimensions were scanned.
 *	3 - Invalid relative position of exit square was scanned.
 *	4 - Invalid starting position for Theseus was scanned.
 *	5 - Invalid starting position for Minotaur was scanned.
 *	6 - Invalid relative position of a "wall" was scanned.
 */
int read_level_file(const char *file_path, struct stats *board) {

	// Create a pointer to the specified file
	FILE *level_file = fopen(file_path, "r");
	if (level_file == NULL) return 1;

	// Scan the dimension of the board from the file into memory
	if (fscanf(level_file, " %d %d ", &board->size.num_rows, &board->size.num_cols) != 2) {
		fclose(level_file);
		return 2;
	}
	if (board->size.num_rows < MIN_BOARD_Y || board->size.num_rows > MAX_BOARD_Y
	    || board->size.num_cols < MIN_BOARD_X || board->size.num_cols > MAX_BOARD_X) {
		fclose(level_file);
		return 2;
	}

	// Scan the relative position of the exit square form the file into memory
	if (fscanf(level_file, " %d %d %d ", &board->exit.relation.row, &board->exit.relation.col, &board->exit.location) != 3) {
		fclose(level_file);
		return 3;
	}
	if (board->exit.relation.row < 0 || board->exit.relation.row >= board->size.num_rows
	    || board->exit.relation.col < 0 || board->exit.relation.col >= board->size.num_cols) {
		fclose(level_file);
		return 3;
	}
	
	switch (board->exit.location) {
		case M_LEFT:
			if (board->exit.relation.col != 0) {
				fclose(level_file);
				return 3;
			}
			break;

		case M_RIGHT:
			if (board->exit.relation.col != (board->size.num_cols - 1)) {
				fclose(level_file);
				return 3;
			}
			break;

		case M_UP:
			if (board->exit.relation.row != 0) {
				fclose(level_file);
				return 3;
			}
			break;

		case M_DOWN:
			if (board->exit.relation.row != (board->size.num_rows - 1)) {
				fclose(level_file);
				return 3;
			}
			break;

		default:
			fclose(level_file);
			return 3;
	}

	// Scan the starting position of Theseus on the board from the file into memory
	if (fscanf(level_file, " %d %d ", &board->theseus.row, &board->theseus.col) != 2) {
		fclose(level_file);
		return 4;
	}
	if (board->theseus.row < 0 || board->theseus.row >= board->size.num_rows
	    || board->theseus.col < 0 || board->theseus.col >= board->size.num_cols) {
		fclose(level_file);
		return 4;
	}

	// Scan the starting position of the Minotaur on the board from the file into memory
	if (fscanf(level_file, " %d %d ", &board->minotaur.row, &board->minotaur.col) != 2) {
		fclose(level_file);
		return 5;
	}
	if (board->minotaur.row < 0 || board->minotaur.row >= board->size.num_rows
	    || board->minotaur.col < 0 || board->minotaur.col >= board->size.num_cols) {
		fclose(level_file);
		return 5;
	}
	else if (board->minotaur.row == board->theseus.row && board->minotaur.col == board->theseus.col) {
		fclose(level_file);
		return 5;
	}

	// Start a linked-list of Walls and check for invalid data
	int grab_val;
	if ((grab_val = grab_wall(level_file, &board->walls, board->size.num_rows, board->size.num_cols)) == 2) {
		board->walls->next = NULL;
		free_walls(board->walls);
		board->walls = NULL;

		fclose(level_file);
		return 6;
	}
	board->walls->next = NULL;

	if (grab_val == 1) {
		free_walls(board->walls);
		board->walls = NULL;

		fclose(level_file);
		return 0;
	}

	// Scan the rest of the walls from the file into the linked-list
	cell_rel *tracker = board->walls;
	while (true) {
		if ((grab_val = grab_wall(level_file, &tracker, board->size.num_rows, board->size.num_cols)) == 2) {
			free_walls(board->walls);
			board->walls = NULL;

			fclose(level_file);
			return 6;
		}
		else if (grab_val == 1) {
			free(tracker->next);
			tracker->next = NULL;
			break;
		}

		tracker = tracker->next;
	}
	fclose(level_file);

	return 0;
}

/**
 * Scan data from a specified file into a single cell_rel structure. Also check for
 * invalid data. The validity of scanned data is determined by the given dimensions of
 * the board and what information a standard "wall" would need. NULL may used in place
 * of 'last_wall' argument to denote the start of a linked-list.
 *
 * 'infile' specifies the FILE pointer to scan data from.
 * 'last_wall' specifies the last element in a linked-list.
 * 'nrows' specifies the number of rows in a board.
 * 'ncols' specifies the number of columns in a board.
 *
 * Error Codes:
 *	0 - No error was encountered.
 *	1 - End of file (EOF) was reached -- no more specified "walls".
 *	2 - Invalid data was encountered.
 */
int grab_wall(FILE *infile, cell_rel **last_wall, int nrows, int ncols) {
	int verdict;

	// Check if current wall will be beginning of linked-list
	if (*last_wall == NULL) {
		*last_wall = malloc(sizeof(cell_rel));
		(*last_wall)->next = *last_wall;
	}
	else (*last_wall)->next = malloc(sizeof(cell_rel));
	
	// Scan the data from the given file into the new cell_rel structure
	if ((verdict = fscanf(infile, " %d %d %d ", &(*last_wall)->next->relation.row, &(*last_wall)->next->relation.col, &(*last_wall)->next->location)) == EOF) return 1;

	// Check that all scanned data is valid
	else if (verdict != 3) return 2;

	else if ((*last_wall)->next->relation.row < 0 || (*last_wall)->next->relation.row >= nrows
		 || (*last_wall)->next->relation.col < 0 || (*last_wall)->next->relation.col >= ncols) return 2;

	else if ((*last_wall)->next->location < 0 || (*last_wall)->next->location > (NUM_MOVES - 1)) return 2;

	return 0;
}

/**
 * Free the memory of a cell_rel structure and all its children.
 */
void free_walls(cell_rel *cur_wall) {
	
	// Base case: cur_wall is NULL (end of linked-list reached)
	if (cur_wall == NULL) return;

	// Keep going through linked-list until end is reached
	else if (cur_wall->next != NULL)
		free_walls(cur_wall->next);

	cur_wall->next = NULL;
	free(cur_wall);

	return;
}
