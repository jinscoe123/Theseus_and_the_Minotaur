#ifndef _SCANS_H
#define _SCANS_H

#include <stdio.h>

#define NUM_SCANS 5

// Enumerated values to represent scanner functions
enum {
	DIMENSIONS,
	EXIT,
	THESEUS,
	MINOTAUR,
	WALLS
}
scanner_funcs;

/**
 * Scan the dimensions for a board from the given file into the correct
 * members of the inputted stats structure.
 */
int scan_dimensions(FILE *level, struct stats *board);

/**
 * Scan the exit location for a board from the given file into the correct
 * members of the inputted stats structure.
 */
int scan_exit(FILE *level, struct stats *board);

/**
 * Scan the board position of Theseus from the given file into the correct
 * members of the inputted stats structure.
 */
int scan_theseus(FILE *level, struct stats *board);

/**
 * Scan the board position of the Minotaur from the given file into the correct
 * members of the inputted stats structure.
 */
int scan_minotaur(FILE *level, struct stats *board);

/**
 * Scan the board position each of wall from the given file into the correct
 * members of the inputted stats structure.
 */
int scan_walls(FILE *level, struct stats *board);

#endif	    /* _SCANS_H */
