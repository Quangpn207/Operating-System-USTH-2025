#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(){
    pid_t pidA, pidB,pidC;
    int pipeAB[2],pipeBC[2];
    if (pipe(pipeAB) == -1 || pipe(pipeBC) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }
    printf("Pipes created successfully.\n");
    pidA = fork();
    if (pidA < 0) {
        perror("fork A");
        exit(EXIT_FAILURE);
    }
    if (pidA == 0){
        close(pipeAB[0]);
        dup2(pipeAB[1],1);
        close(pipeAB[1]);
        close(pipeBC[0]);
        close(pipeBC[1]);
        execlp("ps","ps","-ef",NULL);
        perror("execlp A");
        exit(EXIT_FAILURE);
    }
    pidB = fork();
    if (pidB < 0) {
        perror("fork B");
        exit(EXIT_FAILURE);
    }
    if (pidB == 0){
        dup2(pipeAB[0],0);
        dup2(pipeBC[1],1);
        close(pipeBC[1]);
        close(pipeBC[0]);
        close(pipeAB[0]);
        close(pipeAB[1]);
        execlp("grep","grep","firefox",NULL);
        perror("execlp B");
        exit(EXIT_FAILURE);
    }
    pidC = fork();
    if (pidC < 0) {
        perror("fork C");
        exit(EXIT_FAILURE);
    }
    if (pidC == 0){
        close(pipeAB[0]);
        close(pipeAB[1]);
        close(pipeBC[1]);
        dup2(pipeBC[0],0);
        close(pipeBC[0]);
        execlp("wc","wc","-l",NULL);
        perror("exec wc");
        exit(EXIT_FAILURE);
    }
    close(pipeAB[0]);
    close(pipeAB[1]);
    close(pipeBC[0]);
    close(pipeBC[1]);
    waitpid(pidA,NULL,0);
    waitpid(pidB,NULL,0);
    waitpid(pidC,NULL,0);
    printf("All child processes finished.\n");
    return 0;
}