/*
 ============================================================================
 Name        : mastermind.c
 Author      : ERL67 
 Compile     : gcc --std=c99 -Wall -Werror -o mastermind mastermind.c
 Colors      : https://misc.flogisoft.com/bash/tip_colors_and_formatting
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

char str_input[100];
char guess[6];
_Bool good_guess;
const int attempts = 10;

/*
typedef enum {
	Red, Orange, Yellow, Green, Blue, Purple
} Color;*/

typedef enum {
	r, o, y, g, b, p
} Letter;


void get_line(char* input, int size);
int random_range(int lower, int upper);
char *num_to_color(int num);
int letter_to_int(char name);
void quit();

int main() {
	int input = 0;
	srand((unsigned int) time(NULL));

	int board[4] = { r, r, r, r };

	do {
		printf("Enter the number of colors[3-6]: ");
		get_line(str_input, 6);
		input = atoi(str_input);
		//printf("\nYou entered s: %s  atoi: %i\n", str_input, input);
	} while (input > 6 || input < 3);

	//generate board
	for (int i = 0; i < 4; i++) {
		int rng = random_range(0, input - 1);
		board[i] = (char) rng;
		//printf("\nslot%d: %d  %s", i, board[i], num_to_color(board[i]));
	}

	//user input guess
	int trys = 1;
	for (trys = 1; trys < attempts; trys++) {

		do {
			good_guess = true;
			printf("\n\nEnter your guess: ");
			get_line(str_input, 100);

			strncpy(guess, str_input, 4); //only take first 4 chars to allow mashing

			printf("You guessed: %s\n", guess);

			int len = strlen(guess); //iterate string only looking for valid letters
			for (int i = 0; i < len; i++) {
				switch (guess[i]) {
				case 'R':
				case 'r':
					break;
				case 'O':
				case 'o':
					break;
				case 'Y':
				case 'y':
					break;
				case 'G':
				case 'g':
					if (input == 3) {
						printf("\033[01;31mNo G. \033[0m\t");
						good_guess = false;
					}
					break;
				case 'B':
				case 'b':
					if (input <= 4) {
						printf("\033[01;31mNo B. \033[0m\t");
						good_guess = false;
					}
					break;
				case 'P':
				case 'p':
					if (input <= 5) {
						printf("\033[01;31mNo P. \033[0m\t");
						good_guess = false;
					}
					break;
				default:
					printf("\033[01;31mError. \033[0m\t");
					good_guess = false;
					break;
				}
			}
		} while (good_guess != true);


		int white = 0;
		int black = 0;
		//compare guesses to board
		for (int i = 0; i < 4; i++) {
			int key = (int) tolower(guess[i]);
			for (int j = i; j < 4; j++) {

				//printf("\n[%d,%d]board:%d  guess:%c  guess:%d", i, j, board[j],guess[i], key);

				if ((key == 114 && board[j] == 0)
						|| (key == 111 && board[j] == 1)
						|| (key == 121 && board[j] == 2)
						|| (key == 103 && board[j] == 3)
						|| (key == 98 && board[j] == 4)
						|| (key == 112 && board[j] == 5)) {

					i == j ? black++ : white++;

					break;
				}

			}
		}

		//display results pegs
		printf("\nResults of try %d:   ", trys);
		for (int i = white; i > 0; i--) {
			printf("\e[47m\e[30mWhite\e[39m\e[49m\t");
		}
		for (int i = black; i > 0; i--) {
			printf("\e[100mBlack\e[49m\t");
		}

		//display win or loss
		if (black == 4) {
			printf("\n\n\e[104mYou Win! Score: %d  Solution: \e[49m\n", trys);
		} else if (trys == 9) {
			printf("\n\n\e[104mYou Lose! Solution: \e[49m ");
		}

		//print solution
		if (black == 4 || trys == 9) {
			for (int i = 0; i < 4; i++) {
				printf(" %s ", num_to_color(board[i]));
			}
			quit();
		}
	}

	return 0;
}

void get_line(char* input, int size) {
	fgets(input, size, stdin);
	int len = strlen(input);
	input[len - 1] = '\0';
}

int random_range(int lower, int upper) {
	return rand() % (upper - lower + 1) + lower;
}

char *num_to_color(int num) {
	if (num == 0)
		return "\033[22;31mRed\033[0m";
	if (num == 1)
		return "\033[22;33mOrange\033[0m";
	if (num == 2)
		return "\033[01;33mYellow\033[0m";
	if (num == 3)
		return "\033[22;32mGreen\033[0m";
	if (num == 4)
		return "\033[22;34mBlue\033[0m";
	if (num == 5)
		return "\033[22;35mPurple\033[0m";
	return "N/A";
}

void quit() {

	int confirm;

	while (confirm != EOF && (confirm != 121 || confirm != 110)) {
		printf("\nPlay Again? (y/n) ");
		confirm = getchar();

		switch (confirm) {
		case 'Y':
		case 'y':
			fflush(stdin);
			fflush(stdout);
			printf("\nNew Game\n");
			main();
			break;
		case 'N':
		case 'n':
			printf("\n\033[22;35mGoodbye.\033[0m\n\n");
			exit(1);
			break;
		default:
			printf("\033[01;31m \t y/n Try Again.\033[0m\n");
			break;
		}
	}

	printf("\n\033[22;35mGoodbye.\033[0m\n\n");
	exit(1);
}
