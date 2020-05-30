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

int openWrapper(const char *path, int *fd);

//memory allocation
int allocWrapper(int num, int size, void **p);

int reallocWrapper(void **pointer, int size);

//read (pipe)
//If successful, return the non-negative number of bytes actually read, on failure -1
int readWrapper(int read);

//write (pipe)
//If successful, return the number of bytes actually written, on failure -1
int writeWrapper(int write);

//close
//If successful, return the number of bytes actually written, on failure -1
int closeWrapper(int close);

#endif