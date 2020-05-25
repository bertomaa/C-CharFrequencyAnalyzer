#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

char* pipename = "/tmp/mypipe.pipe";

int main()
{
    mkfifo(pipename, 0666);
    int fd = open(pipename, O_WRONLY);
    printf("Opened!\n");
    char* a = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaalaaaaaaaaaaaaaaak";
    char* b = "b";
    char* c = "c";
    write(fd, a, strlen(a) + 1);
    printf("written : %s\n", a);
    write(fd, b, strlen(b) + 1);
    printf("written : %s\n", b);
    write(fd, c, strlen(c) + 1);
    printf("written : %s\n", c);
    printf("Written!\n");
    close(fd);
    printf("Closed!\n");
    return 0;
}