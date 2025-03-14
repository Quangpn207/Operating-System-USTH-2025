#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t pid;
char *av[2];
char cmd[20];

void doexec() {
    if (execvp(av[0], av) == -1) {
        perror("execvp failed");
        exit(0);
    }
}

int main() {
    for (;;) {
        printf(">");
        scanf("%s", cmd);
        av[0] = cmd;
        av[1] = NULL;
        
        switch (pid = fork()) {
            case -1:
                perror("fork");
                break;
            case 0:
                doexec();
                break;
            default:
                if (waitpid(pid, NULL, 0) == -1)
                    perror("waitpid failed");
        }
    }
}