#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <n> <c> <s>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
	int value = atoi(argv[2]);
	int start = atoi(argv[3]);

    if (n < 3 || start < 0 || start >= n ) {
        fprintf(stderr, "Argumentos inválidos.\n");
        exit(EXIT_FAILURE);
    }

    int pipes[n][2];
    int pipe_padre[2];
    
	if (pipe(pipe_padre) < 0) { perror("pipe_padre"); exit(EXIT_FAILURE); }

    for (int i = 0; i < n; i++) {
        if (pipe(pipes[i]) < 0) { perror("pipe"); exit(EXIT_FAILURE); }
    }

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {  
            int prev = (i + n - 1) % n;

            for (int j = 0; j < n; j++) {
                if (j != prev) close(pipes[j][0]);
                if (j != i) close(pipes[j][1]);
            }

            close(pipe_padre[0]);  

            int val;
            read(pipes[prev][0], &val, sizeof(int));
            printf("Proceso %d (PID %d) recibió valor %d del proceso %d\n", i, getpid(), val, prev);

            if (i == start) {
                printf("Proceso %d (PID %d) es el start y envía valor %d al padre\n", i, getpid(), val);
                write(pipe_padre[1], &val, sizeof(int));
                close(pipe_padre[1]);
                close(pipes[prev][0]);
                exit(0);
            } else {
                val++;  
                printf("Proceso %d (PID %d) incrementa valor a %d y pasa al proceso %d\n", i, getpid(), val, i);
                write(pipes[i][1], &val, sizeof(int));
                close(pipes[i][1]);
                close(pipes[prev][0]);
                exit(0);
            }
        }
    }

    for (int i = 0; i < n; i++) {
        close(pipes[i][0]);
        if (i != start) close(pipes[i][1]);
    }
    close(pipe_padre[1]);

    printf("Padre escribe valor inicial %d al proceso %d\n", value, start);
    write(pipes[start][1], &value, sizeof(int));
    close(pipes[start][1]);

    int resultado_final;
    read(pipe_padre[0], &resultado_final, sizeof(int));
    close(pipe_padre[0]);

    printf("Padre recibe el valor final: %d\n", resultado_final);

    for (int i = 0; i < n; i++) wait(NULL);

    printf("Padre: todos los procesos terminaron.\n");

    return 0;
}
