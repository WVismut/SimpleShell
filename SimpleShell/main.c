// header files
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

// maximum number of arguments for a command
#define MAX_ARGS 20

#define RESET "\033[0m"
#define CYAN "\033[36m"
#define GREEN "\033[32m"
#define BOLD "\033[1m"

int main() {
	char cwd[256];
	char input[256];
	char *args[MAX_ARGS + 1];
	char *commands[MAX_ARGS + 1];
	char *q_toks[MAX_ARGS + 1];
	bool is_in_quotes[MAX_ARGS + 1];
	char *token;
	int amm_commands;
	int amm_q_tokens;
	int i;
	bool disown;
	bool q_state;
	bool previous_q_state;
	pid_t to_wait;

	while (1) {
		start_of_cycle: // this label helps with errors handling
		getcwd(cwd, sizeof(cwd));
		printf(GREEN BOLD "cshell " RESET CYAN BOLD "%s" RESET GREEN BOLD " > " RESET, cwd);
		if (fgets(input, sizeof(input), stdin) == 0)
			break; // EOF -> close
		if (strlen(input) == 0)
			continue; // skip cycle if user input is empty
		input[strcspn(input, "\n")] = 0; // change \n to \0

		// this part of code tokenize input into separate commands
		amm_commands = 0;
		token = strtok(input, "|");
		while (token != NULL) {
			commands[amm_commands] = token;
			amm_commands++;
			token = strtok(NULL, "|");
			if (amm_commands > MAX_ARGS) {
				printf("Too much arguments!\n");
				goto start_of_cycle;
			}
		}

		for (int command_i = 0; command_i < amm_commands; command_i++) {
			// Now, when commands[] contains all separate commands from input, it's time to tokenize them
			// Next part of code tokenizes command based on quotation marks
			i = 0;
			q_state = false;
			previous_q_state = false;
			token = strtok(commands[command_i], "\"");
			while (token != NULL) {

				// this thing skips empty tokens which appear in between two arguments in quotation marks
				if (strcmp(token, " ") != 0) {
					q_toks[i] = token + previous_q_state;
					is_in_quotes[i] = q_state;
					i++;

					if (i > MAX_ARGS) {
						printf("Too much arguments!\n");
						goto start_of_cycle;
					}
				}

				// update q_state
				previous_q_state = q_state;
				if (q_state)
					q_state = false;
				else
					q_state = true;

				// get a new token
				token = strtok(NULL, "\"");
			}
			amm_q_tokens = i;

			// tokenize arguments
			disown = false;
			i = 0;
			for (int l = 0; l < amm_q_tokens; l++) {
				if (!is_in_quotes[l]) {
					token = strtok(q_toks[l], " ");
					while (token != NULL) {

						// disown option
						if ((strcmp(token, "disown") == 0) & (i == 0)) {
							disown = true;
						}

						args[i - disown] = token;
						i++;
						token = strtok(NULL, " ");

						if (i > MAX_ARGS) {
							printf("Too much arguments!\n");
							goto start_of_cycle;
						}
					}
				} else {
					args[i - disown] = q_toks[l];
					i++;
				}
				args[i - disown] = NULL;
			}
			
			if (strcmp(args[0], "exit") == 0) {
				return 0;
			} else if (strcmp(args[0], "cd") == 0) {
				if (chdir(args[1]) != 0) {
					perror("cd error");
				}
				continue;
			}

			// start command execution
			pid_t pid = fork();

			if (!disown & (pid > 0))
				to_wait = pid;

			if (pid == 0) {
				execvp(args[0], args);
				perror("Command failed");
				exit(1);
			} else if (pid > 0) {
				waitpid(to_wait, NULL, 0);
			} else {
				perror("Something went wrong");
				return 1;
			}
		}
	}
}