/*
 ============================================================================
 Name        : craps.c
 Author		 : ERL67
 Make		 : craps for m32 game, craps64 for default, make ARCH=i386 for dice module
 Test	     : ./craps /dev/urandom
 ============================================================================
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mycolors.h"

#define SNAKEEYES 2
#define ACEDEUCE 3
#define SEVEN 7
#define SIXFIVE 11
#define BOXCARS 12

#define streq(a,b) (strcmp((a), (b)) == 0)
#define die_value(x) ((x) % 6 + 1)

int read_dice (FILE* f);
void get_line(char* input, int size);
FILE* checked_fopen(const char* name, const char* mode);

char username[100];
char choice[100];
unsigned char d1;
unsigned char d2;
_Bool casino_open = true;	//gameplay flag
_Bool gameover = false;		//turn flag

int read_dice (FILE* f) {
	if (!feof(f) && fread(&d1, 1, 1, f) && fread(&d2, 1, 1, f)) {
		d1 = die_value(d1);
		d2 = die_value(d2);
		printf("\aYou have rolled %d + %d = %d\n", d1, d2, d1 + d2);  //beeep
		return d1 + d2;
	} else {
		printf(CYAN("Dice are broken. Try Again.\n"));
		fclose(f);
		exit(3);
	}
}

int main(int argc, char** argv) {

	if (argc < 2) {
		printf("Argument not supplied. Must provide location of dice.\n");
		return 0;
	} else {

		FILE* f = checked_fopen(argv[1], "r");
		if (feof(f)) {
			printf(CYAN("Dice are empty. Try Again.\n"));
			fclose(f);
			exit(2);
		}

		printf("\nWelcome to ERL67's Casino! Please enter your name: ");
		get_line(username, sizeof(username));

		if (strlen(username) == 0) strcpy(username, "Player One");

		while (casino_open) {

			gameover = false;

			printf(MAGENTA("\n\%s, would you like to Play or Quit? "), username);
			get_line(choice, sizeof(choice));

			if (streq(choice, "Play") || streq(choice, "play") || streq(choice, "PLAY") || streq(choice, "yes") || streq(choice, "Yes")) {

				int total = read_dice(f);

				if (total == SEVEN || total == SIXFIVE) {
					printf(GREEN("You Win!"));
				} else if (total == SNAKEEYES || total == ACEDEUCE || total == BOXCARS) {
					printf(RED("You Lose!"));
				} else {
					int point = total;
					while (gameover == false) {
						total = read_dice(f);
						if (total == point) {
							printf(GREEN("You Win!"));
							gameover = true;
						} else if (total == SEVEN) {
							printf(RED("You Lose!"));
							gameover = true;
						}
					}
				}
			} else if (streq(choice, "Quit") || streq(choice, "quit") || streq(choice, "QUIT") || streq(choice, "no") || streq(choice, "No")) {
				casino_open = false;
				printf (YELLOW("Goodbye, %s!\n\n"), username);
			}
		}
		fclose(f);
	}
	return 0;
}

void get_line(char* input, int size) {
	fgets(input, size, stdin);
	int len = strlen(input);
	input[len - 1] = '\0';
}

FILE* checked_fopen(const char* name, const char* mode) {
	FILE* f = fopen(name, mode);
	if (f == NULL) {
		fprintf(stderr, RED("Dice Not Found '%s'\n"), name);
		exit(1);
	}
	return f;
}
