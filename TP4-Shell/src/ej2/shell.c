#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define MAX_COMMANDS 200

int validar_sintaxis_pipes(char *input) {
    char copia[256];
    strncpy(copia, input, sizeof(copia));
    copia[sizeof(copia) - 1] = '\0';

    char *start = copia;
    while (*start == ' ') start++;
    char *end = start + strlen(start) - 1;
    while (end > start && *end == ' ') *end-- = '\0';

    if (*start == '|' || *end == '|') {return 0;}

    int last_was_pipe = 0;
    for (char *p = start; *p; p++) {
        if (*p == '|') {
            if (last_was_pipe) return 0;
            last_was_pipe = 1;} 
        else if (*p != ' ') { last_was_pipe = 0;}
    }

    return 1;
}

int validar_comillas(const char *input) {
    int in_single = 0, in_double = 0;
    for (; *input; input++) {
        if (*input == '\'' && !in_double) in_single = !in_single;
        else if (*input == '\"' && !in_single) in_double = !in_double;
    }
    return !(in_single || in_double);
}

char *tokenizar_arg(char **stringp) {
    char *start = *stringp;
    if (!start) return NULL;

    while (*start && (*start == ' ' || *start == '\t')) start++;
    if (*start == '\0') {
        *stringp = NULL;
        return NULL;
    }

    char *end = start;
    while (*end && *end != ' ' && *end != '\t') end++;

    if (*end) {
        *end = '\0';
        *stringp = end + 1;} 
    else {*stringp = NULL;}

    return start;
}

int main() {
    char command[256];
    char *commands[MAX_COMMANDS];
    int command_count = 0;

    while (1) {
        printf("Shell> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {break;}

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0) {break;}

        if (!validar_sintaxis_pipes(command)) {
            fprintf(stderr, "Error de sintaxis: uso incorrecto del pipe '|'\n");
            continue;
        }

        if (!validar_comillas(command)) {
            fprintf(stderr, "Error de sintaxis: comillas abiertas sin cerrar\n");
            continue;
        }

        command_count = 0;
        char *token = strtok(command, "|");
        while (token != NULL && command_count < MAX_COMMANDS) {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }

        int prev_pipe_fd[2];

        for (int i = 0; i < command_count; i++) {
            while (*commands[i] == ' ' || *commands[i] == '\t') commands[i]++;

            char *argv[100];
            int argc = 0;
            char *args_ptr = commands[i];
            char *arg;
            while ((arg = tokenizar_arg(&args_ptr)) != NULL && argc < 99) {
                argv[argc++] = arg;
            }
            argv[argc] = NULL;

            if (argc == 0) continue;

            int pipe_fd[2];
            if (i < command_count - 1) {
                if (pipe(pipe_fd) == -1) {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
            }

            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (pid == 0) {

                if (i > 0) {dup2(prev_pipe_fd[0], STDIN_FILENO);}

                if (i < command_count - 1) {dup2(pipe_fd[1], STDOUT_FILENO);}

                if (i > 0) {
                    close(prev_pipe_fd[0]);
                    close(prev_pipe_fd[1]);
                }
                if (i < command_count - 1) {
                    close(pipe_fd[0]);
                    close(pipe_fd[1]);
                }

                execvp(argv[0], argv);
                fprintf(stderr, "Error: comando '%s' inexistente\n", argv[0]);
                exit(EXIT_FAILURE);
            } 
        }

        for (int i = 0; i < command_count; i++) {
            wait(NULL);
        }
    }

    return 0;
}
