#ifndef _LOADER_H
#define _LOADER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN_BOARD_X 3
#define MIN_BOARD_Y 3
#define MAX_BOARD_X 20
#define MAX_BOARD_Y 10

// Structure to hold dimensions of a board
struct dimensions {
	int num_rows;
	int num_cols;
};

// Structure to hold location of a square on the board
typedef struct {
	int row;
	int col;
}
cell_pos;

// Structure to hold a cell_pos structure and a relative location to that position
typedef struct cell_rel {
	cell_pos relation;
	int location;
	struct cell_rel *next;
}
cell_rel;

// Structure to hold all the information needed for a board
struct stats {
	struct dimensions size;
	cell_rel exit;

	cell_pos theseus;
	cell_pos minotaur;

	cell_rel *walls;
};

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
 *      0 - No error was encountered.
 *      1 - The file specified by the given file path could not be opened.
 *      2 - Invalid board dimensions were scanned.
 *      3 - Invalid relative position of exit square was scanned.
 *      4 - Invalid starting position for Theseus was scanned.
 *      5 - Invalid starting position for Minotaur was scanned.
 *      6 - Invalid relative position of a "wall" was scanned.
 */
int read_level_file(const char *file_path, struct stats *board);

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
 *      0 - No error was encountered.
 *      1 - End of file (EOF) was reached -- no more specified "walls".
 *      2 - Invalid data was encountered.
 */
int grab_wall(FILE *infile, cell_rel **last_wall, int nrows, int ncols);

/**
 * Free the memory of a cell_rel structure and all its children.
 */
void free_walls(cell_rel *cur_wall);

#endif    // _LOADER_H
