#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

int openWrapper(const char *path, int *fd)
{
    *fd = open(path, O_RDONLY);
    if(*fd < 0)
    {
        fprintf(stderr, "Impossibile aprire il file %s.\n", path);
        return 1;
    }
    return 0;
}

int allocWrapper(int num, int size, void** p)
{
    *p = calloc(num, size);
    if(*p == NULL)
    {
        fprintf(stderr, "Impossibile allocare %d * %d byte di memoria", num, size);
        return 1;
    }
    return 0;
}