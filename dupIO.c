#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

int main (int argc, char *argv[]) {
    char *src, *dest;
    int fd;

    if (argc != 3) {
        printf("usage: copy <src> <dest>\n");
        return 1;
    }

    src = argv[1];
    dest = argv[2];

    fd = open(dest, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("error open");
        exit(1);
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("error dup2");
        close(fd);
        exit(1);
    }

    close(fd); // good practice to close fd after dup2

    if (execlp("cat", "cat", src, NULL) == -1) {
        perror("error execl");
        exit(1);
    }

    return 0;
}
