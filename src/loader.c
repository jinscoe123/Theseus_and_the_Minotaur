#include "loader.h"
#include "movement.h"
#include "scans.h"

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
 *	7 - Unknown function exited with return value of 1.
 */
int read_level_file(const char *file_path, struct stats *board) {

	// Create a pointer to the specified file
	FILE *level_file = fopen(file_path, "r");
	if (level_file == NULL) return 1;

	// Create an array of pointers to the scanner functions
	int (*scanners[])(FILE *, struct stats *) = {scan_dimensions, scan_exit, scan_theseus, scan_minotaur, scan_walls};

	// Perform each scanner function in order
	for(int i = 0; i < NUM_SCANS; i++) {
		if (scanners[i](level_file, board) == 1) {
			
			// Return the correct error code based on failed scanner function
			switch (i) {
				case DIMENSIONS:
					return 2;

				case EXIT:
					return 3;

				case THESEUS:
					return 4;

				case MINOTAUR:
					return 5;

				case WALLS:
					return 6;

				default:
					return 7;
			}
		}
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
int grab_wall(FILE *infile, cell_rel **last_wall, short nrows, short ncols) {
	int verdict;

	// Check if current wall will be beginning of linked-list
	if (*last_wall == NULL) {
		*last_wall = malloc(sizeof(cell_rel));
		(*last_wall)->next = *last_wall;
	}
	else (*last_wall)->next = malloc(sizeof(cell_rel));
	
	// Scan the data from the given file into the new cell_rel structure
	if ((verdict = fscanf(infile, " %hd %hd %hd ", &(*last_wall)->next->relation.row, &(*last_wall)->next->relation.col, &(*last_wall)->next->location)) == EOF) return 1;

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
	cell_rel *temp;

	// Navigate through linked-list, freeing all memory
	while (cur_wall) {
		temp = cur_wall->next;

		free(cur_wall);
		cur_wall = temp;
	}

	return;
}
