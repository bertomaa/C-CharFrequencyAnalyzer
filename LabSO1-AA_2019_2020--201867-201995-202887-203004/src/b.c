#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

char *pipename = "/tmp/mypipe.pipe";

int main()
{
    int fd = open(pipename, O_RDONLY);
    sleep(1);
    if (fd > 0)
    {
        int n;
        int i;
        char buffer[50];
        for(i = 0; i < 50; i++)
            buffer[i] = '\0';
        char buffer2[50];
        for(i = 0; i < 50; i++)
            buffer2[i] = '\0';
        char buffer3[50];
        for(i = 0; i < 50; i++)
            buffer3[i] = '\0';
        n = read(fd, buffer, 49);
        buffer[n] = '\0';
        printf("read 1 %d: %s.\n", n, buffer);
        sleep(1);

        n = read(fd, buffer2, 49);
        buffer2[n] = '\0';
        printf("read 2 %d: %s.\n", n, buffer2);
        sleep(1);

        n = read(fd, buffer3, 49);
        buffer3[n] = '\0';
        printf("read 3 %d: %s.\n", n, buffer3);
    }
    close(fd);
    return 0;
}