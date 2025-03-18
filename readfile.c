#include <stdio.h>
#include <fcntl.h>      // open(), O_CREAT, O_RDWR
#include <sys/stat.h>   // mode_t, quyền truy cập file
#include <unistd.h> 
int main(){
    char buff[100];
    int fd = open("test", O_RDONLY);
    int nb = read(fd, buff, 100);
    buff[nb] = 0;
    printf("> %s\n", buff);
}