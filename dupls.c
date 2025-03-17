#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
int main(int argc, char *argv[]) {
    int fd;
        fd=open("foo.txt", O_WRONLY,0700);
        if(fd == -1) {
            perror("error opening file");
            exit(0);
        }
        if(dup2(fd, 1) == -1) {
            perror("error duplicating");
            close(fd);
            exit(0);
        }
        if(execlp("ls","ls",NULL)==-1){
            perror("error execl");
            exit(0);
            close(fd);
        }
    return 0;
}