#ifndef _MOVEMENT_H
#define _MOVEMENT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "loader.h"

#define NUM_MOVES 4

#define M_LEFT 0
#define M_RIGHT 1
#define M_UP 2
#define M_DOWN 3

// An array of numbers with which to build and manipulate bit_masks for valid moves on the board
extern const int moves[];

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
void set_moves(struct stats *board, board_square *win_grid);

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
 *      0 - No move was made for Theseus (invalid move).
 *      1 - A move was made to an ordinary square on the board.
 *      2 - A move was made to the exit WINDOW.
 *      3 - A move was made to a square occupied by the Minotaur (display image not copied over).
 */
int move_theseus(struct stats *board, board_square *win_grid, WINDOW *exit, short *win_pair, int move);

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
int move_minotaur(struct stats *board, board_square *win_grid, short *win_pair);

#endif    // _MOVEMENT_H
