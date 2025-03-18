#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
// Macro to close all pipe ends in the current process
#define close_pipe do {\
    close(pipeAB[1]); \
    close(pipeBA[0]); \
    close(pipeAB[0]); \
    close(pipeBA[1]); \
} while(0)
int main() {
    pid_t pidA, pidB;
    int pipeAB[2];  // Pipe: A -> B
    int pipeBA[2];  // Pipe: B -> A

    // Create the pipes
    if (pipe(pipeAB) == -1 || pipe(pipeBA) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }
    printf("Pipes created successfully.\n");
    pidA = fork();
    if (pidA < 0) {
        perror("fork A");
        exit(EXIT_FAILURE);
    }
    if (pidA == 0) {
        // Child process A setup
        dup2(pipeBA[0], STDIN_FILENO);   // Read from pipeBA (B -> A)
        dup2(pipeAB[1], STDOUT_FILENO);  // Write to pipeAB (A -> B)
        close_pipe;

        char *argvA[] = { "./prgA", NULL };
        execvp(argvA[0], argvA);
        perror("execvp prgA failed");
        exit(1);
    }

    // Fork Program B
    pidB = fork();
    if (pidB == -1) {
        perror("fork failed for prgB");
        exit(EXIT_FAILURE);
    }

    if (pidB == 0) {
        // Child process B setup
        dup2(pipeAB[0], STDIN_FILENO);   // Read from pipeAB (A -> B)
        dup2(pipeBA[1], STDOUT_FILENO);  // Write to pipeBA (B -> A)
        close_pipe;

        char *argvB[] = { "./prgB", NULL };
        fprintf(stderr, "prgB about to exec...\n");
        execvp(argvB[0], argvB);
        perror("execvp prgB failed");
        exit(1);
    }

    // Parent closes unused file descriptors
    close_pipe;

    // Wait for both processes to finish
    waitpid(pidA, NULL, 0);
    waitpid(pidB, NULL, 0);

    return 0;
}
