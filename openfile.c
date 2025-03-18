#include <stdio.h>
#include <fcntl.h>      // open(), O_CREAT, O_RDWR
#include <sys/stat.h>   // mode_t, quyền truy cập file
#include <unistd.h>     // close()
int main(){
    int fd,p;
    fd = open("test", O_RDWR | O_CREAT, 0700);
    p=write(fd,"Hello World",11);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }
    printf("File opened successfully! File descriptor: %d\n", fd);
    close(fd); 
    return 0;
}
