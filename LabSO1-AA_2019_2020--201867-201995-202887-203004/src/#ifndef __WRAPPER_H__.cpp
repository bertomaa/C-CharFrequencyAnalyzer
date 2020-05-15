#ifndef __WRAPPER_H__
#define __WRAPPER_H__

/*
    This file contains the functions that handle various type of errors:
    - if a file has been opened correctly
    - if memory has been allocated correctly
    - if read/write of a pipe has been executed correctly
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "commons.h"

//opening of a file
void openWrapper(const char *path, int *fd)
{
    *fd = open(path, O_RDONLY);
    int attempt = 0;
    if(*fd < 0)
    {
        while(*fd < 0 && attempt < 5)
        {
            attempt++;
            fprintf(stderr, "Cannot open file %s. Attempt number %d\n", path, attempt);
            wait(1);
            *fd = open(path, O_RDONLY);
        }

        if(attempt == 5)
        {
            fprintf(stderr, "Cannot open file %s. I tried 5 times. \n");
            exit(2);
        }
    }

    return 0;
}

//memory allocation
void allocWrapper(int num, int size, void** p)
{
    *p = calloc(num, size);
    int attempt = 0;
    if(*p == NULL)
    {
        while(*fd < 0 && attempt < 5)
        {
            attempt++;
            fprintf(stderr, "Cannot allocate %d * %d byte in memory. Attempt number %d \n", num, size, attempt);
            wait(1);
            *p = calloc(num, size);
        }

        if(attempt == 5)
        {
            fprintf(stderr, "Cannot allocate %d * %d byte in memory\n", num, size);
            exit(2);
        }   
    }
}

//read (pipe)
//If successful, return the non-negative number of bytes actually read, on failure -1
int readWrapper(int *pipes, int index, char *buf, int toRead)
{
    int attempt = 0;
    int read = read(pipes[getPipeIndex(index, READ)], buf, MAX_PIPE_CHARACTERS * toRead);
    if(read == -1)
    {
        while(read == -1 && attempt < 5)
        {
            attempt++;
            fprintf(stderr, "Cannot read the specified file. Attempt number %d \n", attempt);
            wait(1);
            read = read(pipes[getPipeIndex(index, READ)], buf, MAX_PIPE_CHARACTERS * toRead);
        }

        if(attempt == 5)
        {
            fprintf(stderr, "Cannot allocate %d * %d byte in memory\n", num, size);
            exit(2);
        }   
    }
}

//write (pipe)
//If successful, return the number of bytes actually written, on failure -1
int writeWrapper(int write)
{
    if(write == -1)
    {
        fprintf(stderr, "Cannot write the specified file \n");
        return 1;
    }
    return 0;
}

//close
//If successful, return the number of bytes actually written, on failure -1
int closeWrapper(int close)
{
    if(close == -1)
    {
        fprintf(stderr, "Cannot close the specified file \n");
        return 1;
    }
    return 0;
}



#endif