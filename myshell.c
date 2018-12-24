/*
 ============================================================================
 Name        : myshell.c
 Author		 : ERL67
 Compile     : gcc --std=c99 -Wall -Werror -o myshell myshell.c
 Test	     : ./myshell
 ============================================================================
 */
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "mycolors.h"
#include <stdbool.h>

#define PROMPT "\033[0;102m\033[1;90m myshell >\e[0m\033[40m "
#define CR printf("\n");
#define MAX_INPUT 300

int main(int argc, char** argv) {

	signal(SIGINT, SIG_IGN);

	if (argc > 1) {
		printf("Arguments not supported yet\n");
		return 0;
	}

	char input[MAX_INPUT];

	while (true) {

		printf(PROMPT);

		while (fgets(input, MAX_INPUT, stdin)) {

			signal(SIGINT, SIG_DFL);

			//jarrettbillingsley.github.io/teaching/classes/cs0449/projects/strtok.c
			int num_tokens = (strlen(input) / 2) + 1; // worst case...
			char* tokens[num_tokens];

			const char* delim = " \t\n\0";
			char* token = strtok(input, delim);
			int i;
			for (i = 0; token != NULL; i++) {
				tokens[i] = token;
				token = strtok(NULL, delim);
			}
			tokens[i] = NULL;
			int token_length = i;

			//for(i = 0; tokens[i] != NULL; i++)
			//	printf("token %d: '%s'\n", i, tokens[i]);

			//run builtin commands
			if (tokens[0] == NULL) {
				continue;
			} else if (strcmp(tokens[0], "exit") == 0) {
				exit(token_length == 1 ? 0 : atoi(tokens[1]));
			} else if (strcmp(tokens[0], "cd") == 0) {
				chdir(token_length == 1 ? "." : tokens[1]);
				memset(input, 0, sizeof(input));
				printf(PROMPT);
				continue;
			} else {

				int cpid = fork();

				if (cpid == 0) {	//child processes

					signal(SIGINT, SIG_DFL);

					//single command
					if (tokens[1] == NULL) {
						//always says A NULL argv[0] was passed through an exec system call.
						//passing the executable again works
						execvp(tokens[0], &tokens[0]);
					} else {
						//multiple commands

						//redirect outputs
						int inredirect = 0;
						int outredirect = 0;
						char* in_buffer;
						FILE* f;
						FILE* fout;
						_Bool failure = false;

						//tally redirections
						for (i = 0; tokens[i] != NULL; i++) {
							if (strcmp(tokens[i], "<") == 0) {
								inredirect++;
							} else if (strcmp(tokens[i], ">") == 0) {
								outredirect++;
							}
						}

						if (inredirect > 1 || outredirect > 1) {
							printf("Redirection overload. One each maximum.\n");
							failure = true;
						} else if (inredirect == 0 && outredirect == 0) {
							//no redirects, run command and all arguments
							execvp(tokens[0], &tokens[0]);
						} else {
							//iterate through tokens and catch redirects
							for (i = 0; tokens[i] != NULL; i++) {

								//for (int z = 0; tokens[z] != NULL; z++)
								//	printf(RED("i:%d  token %d: '%s'\n"), i, z, tokens[z]);

								if (strcmp(tokens[i], "<") == 0) {

									if (i == token_length) {
										printf("Must specify input file\n");
										failure = true;
									} else {
										f = fopen(tokens[i + 1], "r");
									}

									if (f == NULL) {
										fprintf(stderr, "Input File Not Found [%s]\n",
												tokens[i + 1]);
										failure = true;
									} else if (failure == false) {
										fseek(f, 0, SEEK_END);
										unsigned long in_len = ftell(f);
										fseek(f, 0, SEEK_SET);

										in_buffer = malloc(in_len);
										fread(in_buffer, in_len, 1, f);

										tokens[i] = in_buffer;

										//remove tokens that were parsed to files/stdout
										for (int j = i; tokens[j] != NULL; j++) {
											if (j + 1 == token_length) {
												tokens[j] = NULL;
											} else {
												tokens[j] = tokens[j + 1];
											}
										}

										//for(int z = 0; tokens[z] != NULL; z++)
										//	printf(GREEN("in %d: '%s'\n"), z, tokens[z]);
									}
								}

								if (strcmp(tokens[i], ">") == 0) {

									if (tokens[i + 1] == NULL) {
										printf("Must specify output file\n");
										failure = true;
									}

									if (failure == false) {
										fout = freopen(tokens[i + 1], "w+", stdout);
									}

									if (fout == NULL) {
										fprintf(stderr, "Error opening output file [%s]\n",
												tokens[i + 1]);
										failure = true;
									} else if (failure == false) {

										//remove tokens that were parsed to files/stdout
										for (int j = i; tokens[j] != NULL; j++) {
											if (j + 2 == token_length) {
												tokens[j] = NULL; //if last argument is output
												tokens[j + 1] = NULL;
											} else {
												tokens[j] = tokens[j + 2];
												tokens[j + 1] = tokens[j + 3];
											}
										}
										i = i - 2;

										//for(int z = 0; tokens[z] != NULL; z++)
										//	printf(YELLOW("out %d: '%s'\n"), z, tokens[z]);
									}
								}
							}

							if (failure == false) {

								//for (i = 0; tokens[i] != NULL; i++)
								//	printf(MAGENTA("token %d: '%s'\n"), i, tokens[i]);

								execvp(tokens[0], &tokens[0]);

								//free memory buffers
								if (inredirect == 1) {
									free(in_buffer);
									fclose(f);
								}
								if (outredirect == 1)
									fclose(fout);
							} else if (failure == true) {
								//if command was unparsable exit child with error
								exit(66);
							}
						}
					}

					//catch errors from exec()
					if (errno > 0) {
						perror("ERROR");
						int error = errno;
						exit(error);
					}
					exit(255); //child ends

				} else {
					//parent process
					signal(SIGINT, SIG_IGN);

					int child_status;
					waitpid(-1, &child_status, 0);

					//printf("----------\n");
					//print status
					if (WIFEXITED(child_status))
						printf(BLUE("Command exited normally with status %d.\n"),
								WEXITSTATUS(child_status));
					else if (WIFSIGNALED(child_status))
						printf(BLUE("Command exited due to signal '%s'.\n"),
								strsignal(WTERMSIG(child_status)));
					else
						printf(BLUE("Command exited.\n"));
				}

				//clear input and reprompt
				memset(input, 0, sizeof(input));
				printf(PROMPT);
				continue;
			}
		}

		memset(input, 0, sizeof(input));	
	}
	CR
	return 0;
}
