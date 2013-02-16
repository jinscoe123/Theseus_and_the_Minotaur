#ifndef _GAME_H
#define _GAME_H

#define _BSD_SOURCE
#include <unistd.h>

#include "board.h"
#include "loader.h"
#include "movement.h"

#define PAUSE_TIME 200000

#define MAIN_MENU 'm'	/* Command key to return to the Main Menu */
#define EXIT 'q'	/* Command key to quit game */
#define RESTART 'r'	/* Command key to restart game */
#define SKIP_TURN ' '	/* Command key to skip turn */

#define MESSAGE_HEIGHT 7
#define MESSAGE_WIDTH 45

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
 *      0 - The user quit the game.
 *      1 - The user wants to return to the Main Menu (lost game).
 *	2 - The user wants to return to the Main Menu (won game).
 *      3 - The user wants to continue to the next level.
 *	4 - The user wants to start a new game.
 *
 * Error Codes:
 *      5 - The inputted value for 'file_path' was NULL.
	6 - The file specified by 'file_path' could not be opened for reading.
 *      7 - The data from the file specified by 'file_path' was invalid.
 *      8 - The game quit unexpectedly (user didn't win, lose, or press EXIT key).
 */
int play_game(const char *file_path, bool last_level);

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
 *      true - 'true_choice' was selected by the user.
 *      false - 'false_choice' was selected by the user.
 */
bool show_message(int win_height, int win_width, const char *title, const char *true_choice, const char *false_choice);

#endif	    // _GAME_H
