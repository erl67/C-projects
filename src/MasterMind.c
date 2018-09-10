/*
 ============================================================================
 Name        : MasterMind.c
 Author      : ERL67
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

int colors;
char str_input[6];



 int main() {
	 int input = 0;
	 puts("\nMasterMind\n\n");

	 do {

		 printf("Enter the number of colors[3-6]: ");

		 fgets(str_input, 3, stdin);

//		 input = str_input[0];
		 input = atoi(str_input);

		 printf("\nYou entered s: %s", str_input);
		 printf("\nYou entered u: %u", input);
		 printf("\nYou entered i: %i\n\n", input);

	 } while (input > 6 || input < 3 );








     return 0;
 }
