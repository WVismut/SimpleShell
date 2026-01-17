#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#define MAX_ARGS 20
#define MAX_CHILDREN 10
#define RESET "\033[0m"
#define CYAN "\033[36m"
#define GREEN "\033[32m"
#define BOLD "\033[1m"

int main() {
	char cwd[256];
	char input[256];
	char *args[MAX_ARGS + 1];
	char *commands[MAX_ARGS + 1];
	char *token;
	int amm_commands; // ammount of commands in user input
	int i;
	bool disown;
	pid_t to_wait;

	while (1) {
		getcwd(cwd, sizeof(cwd));
		printf(GREEN BOLD "cshell " RESET CYAN BOLD "%s" RESET GREEN BOLD " > " RESET, cwd);
		if (fgets(input, sizeof(input), stdin) == 0)
			break; // EOF -> close
		if (strlen(input) == 0)
			continue; // skip cycle if user input is empty
		input[strcspn(input, "\n")] = 0; // change \n to \0

		// tokenize input into separate commands
		amm_commands = 0;
		token = strtok(input, "|");
		while (token != NULL) {
			commands[amm_commands] = token;
			amm_commands++;
			token = strtok(NULL, "|");
		}

		for (int command_i = 0; command_i < amm_commands; command_i++) {
			// tokenize arguments
			disown = false;
			i = 0;
			token = strtok(commands[command_i], " ");
			while (token != NULL) {
				if ((strcmp(token, "disown") == 0) & (i == 0)) {
					disown = true;
				}
				args[i - disown] = token;
				i++;
				token = strtok(NULL, " ");
			}
			args[i - disown] = NULL;

			if (strcmp(args[0], "exit") == 0) {
				return 0;
			} else if (strcmp(args[0], "cd") == 0) {
				if (chdir(args[1]) != 0) {
					perror("cd error");
				}
				continue;
			}

			// start command
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