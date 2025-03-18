#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    int fd[2]; // fd[0]: read end, fd[1]: write end of the pipe
    int pid;
    pipe(fd);
    // Step 1: Create a pipe
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }
    // Step 2: Fork to create the print process
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    if (pid == 0) {
        // Child process runs "print"
        // Step 3: Redirect stdin to read from the pipe
        close(fd[1]);                // Close write end (not needed here)
        dup2(fd[0], 0);   // Redirect stdin to fd[0]
        close(fd[0]);                // Close original fd[0]
        // Step 4: Execute print binary
        char *argv_print[] = { "./print", NULL };
        execvp(argv_print[0], argv_print);

        // If execvp fails
        perror("execvp print");
        exit(1);
    }

    // Parent process continues here (runs gendoc)

    // Step 5: Redirect stdout to write to the pipe
    close(fd[0]);                // Close read end (not needed here)
    dup2(fd[1], 1);  // Redirect stdout to fd[1]
    close(fd[1]);                // Close original fd[1]

    // Step 6: Execute gendoc binary
    char *argv_gendoc[] = { "./gendoc", NULL };
    execvp(argv_gendoc[0], argv_gendoc);

    // If execvp fails
    perror("execvp gendoc");
    exit(1);

    return 0;
}
