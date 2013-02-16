#ifndef _WELCOME_H
#define _WELCOME_H

#include <ncurses.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_HEIGHT 5
#define MAX_OPT_LENGTH 50
#define PADDING_LEFT 3

#define MAX_TITLE_LENGTH 50

#define MIN_MAN_OPTIONS 1
#define MAX_MAN_OPTIONS 3
#define NUM_MAN_PAGES 3

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
 *      The function returns the selected menu item number (starting from 0).
 */
int show_menu(int height, int width, const char *title, int size_array, char **opt_strings, int num_extra, ...);

/**
 * Show a manual page for how to play the Theseus and the Minotaur Game, depending on
 * the page number entered as an argument.
 *
 * 'page' specifies which manual page to display to the user.
 *
 * Return Value:
 *      Function returns the number (starting from 0) of the menu option selected from
 *      the current manual page.
 *
 * Error Values:
 *	-1 - The specified manual page does not exist.
 */
int show_manual(int page);

/**
 * Initialize a WINDOW of specified dimensions and print some inputted strings of text
 * to a number of the WINDOW's lines.
 *
 * 'win_height' specifies the height for the new WINDOW.
 * 'win_width' specifies the width for the new WINDOW.
 * 'start_line' signals which line to start printing given
 *
 * Return Value:
 *      Function return a pointer to a WINDOW if successful.
 *      Otherwise function returns NULL.
 */
WINDOW *create_window(int win_height, int win_width, int start_line, int num_lines, ...);

/**
 * Add a specified number of menu options to a give WINDOW and wait for the user
 * to choose one, by navigating with the left and right arrow keys. Note that the
 * 'num_options' parameter must be less than MAX_MAN_OPTIONS and less than
 * MIN_MAN_OPTIONS. These two constants are by default set to 1 and 3 respectively
 * and shouldn't be changed without altering the actual function as well to support
 * different numbers of menu options.
 *
 * 'win' specifies which WINDOW to add menu options to.
 * 'num_options' specifies how many menu options to add the the WINDOW.
 *
 * Return Value:
 *      Function returns the number (starting from 0) of the menu option selected.
 */
int add_options(WINDOW *win, int num_options, ...);

#endif	    // _WELCOME_H
