#include "game.h"
#include "welcome.h"

#define NAME_LENGTH 50
#define MAX_LEVELS 50

#define MAIN_MENU_ITEMS 4

int main(void) {
	char *level_list[MAX_LEVELS];
	int level_index = 0;

	// Try to open the levellist.txt file and read from it
	FILE *list_file = fopen("./Levels/levellist.txt", "r");
	if (list_file != NULL) {

		bool name_error;
		int c, index;
		char buffer[NAME_LENGTH + 1];

		// Read in as many level file paths from the levellist.txt file as possible
		while ((c = fgetc(list_file)) != EOF) {
			fseek(list_file, -1, SEEK_CUR);

			if (level_index > MAX_LEVELS) break;

			// Copy the name of the file path to a buffer
			name_error = false;
			index = 0;
			for(c = fgetc(list_file); c != '\n' && c != EOF; c = fgetc(list_file)) {
				if (index >= NAME_LENGTH) {
					name_error = true;
					break;
				}

				buffer[index++] = (char)c;
			}
			buffer[index] = '\0';

			if (name_error) break;

			// Copy the name of the file path from the buffer to an array of level file paths
			level_list[level_index] = malloc(sizeof(char) * (index + 1));
			strcpy(level_list[level_index++], buffer);
		}
		fclose(list_file);
	}

	int level_num = 0;
	int action_choice = 0;
       	int prev_action = -1;
	int man_page = 1;
	
	char *main_menu[MAIN_MENU_ITEMS] = {
		"Play Game",
		"Levels",
		"How to Play",
		"Exit"
	};

	int menu_choice, game_result, manual_choice;
	bool exit_game = false;

	// Initalize the terminal screen in curses mode
	initscr();
	start_color();

	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	
	while (true) {

		switch (action_choice) {
			case 0:
				prev_action = action_choice;
				menu_choice = show_menu(MAIN_MENU_ITEMS + MIN_HEIGHT, 50, "Welcome to Theseus and the Minotaur!", MAIN_MENU_ITEMS, main_menu, 0);

				// Decide action to take based on selected menu item
				switch (menu_choice) {
					case 0:
						action_choice = 1;
						break;

					case 1:
						action_choice = 2;
						break;

					case 2:
						action_choice = 3;
						break;

					case 3:
						action_choice = 4;
						break;
				}
				break;

			case 1:
				if (level_num > 0 && prev_action != 1 && prev_action != 2) {
					if (show_message(7, 55, "Would you like to continue from the last level?", "Restart", "Continue"))
						level_num = 0;
				}
				prev_action = action_choice;
				game_result = play_game(level_list[level_num], level_num == (level_index - 1));

				// Handle any errors that may have occurred while initializing/playing the game
				switch (game_result) {
					case 0:
						action_choice = 4;
						break;

					case 1:
						action_choice = 0;
						break;

					case 2:
						action_choice = 0;
						if (level_num != (level_index - 1)) level_num++;
						break;

					case 3:
						level_num++;
						break;

					case 4:
						level_num = 0;
						break;

					case 5:
					case 6:
						if (show_message(7, 60, "Error: Level file could not be opened for reading.", "Exit Game", "Return to Menu"))
							action_choice = 4;
						else action_choice = 0;
						break;

					case 7:
						if (show_message(7, 50, "Error: Level file contains invalid data.", "Exit Game", "Return to Menu"))
							action_choice = 4;
						else action_choice = 0;
						break;

					case 8:
						if (show_message(0, 0, "Error: The game quit unexpectedly.", "Exit Game", "Return to Menu"))
							action_choice = 4;
						else action_choice = 0;
						break;
				}
				break;

			case 2:
				prev_action = action_choice;

				// Show a list of all levels from levellist.txt file
				if ((menu_choice = show_menu(level_index + 1 + MIN_HEIGHT, NAME_LENGTH + (2 * PADDING_LEFT), "Choose Level File", level_index, level_list, 1, "Back")) != level_index) {
					level_num = menu_choice;
					action_choice = 1;
				}
				else action_choice = 0;
				
				break;

			case 3:
				prev_action = action_choice;

				// Alternate through different pages of the manual based on user input
				while (action_choice != 0) {
					manual_choice = show_manual(man_page);

					switch (man_page) {
						case 1:
							if (manual_choice == 1) man_page++;
							else action_choice = 0;

							break;

						case NUM_MAN_PAGES:
							if (manual_choice == 0) man_page--;
							else action_choice = 0;
							
							break;

						default:
							if (manual_choice == 0) man_page--;
							else if (manual_choice == 2) man_page++;
							else action_choice = 0;

							break;
					}
				}
				man_page = 1;

				break;

			case 4:
				exit_game = true;
				break;
		}

		if (exit_game) break;
	}

	// Exit out of curses mode
	endwin();

	// Free all allocated memory
	for(int i = 0; i < level_index; i++)
		free(level_list[i]);

	return 0;
}
