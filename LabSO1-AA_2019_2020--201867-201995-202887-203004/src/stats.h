#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>


#define READ 0
#define WRITE 1
#define ASCII_CHARACTERS 256
#define MAX_CHARACTERS 8128

typedef struct {
    int frequencies[ASCII_CHARACTERS];
    
    //returns 0 if decode has success
    bool decode(const char* str)
    {

    }

    char* encode()
    {
        char* buffer[MAX_CHARACTERS];
        int i;
        for(i = 0; i < ASCII_CHARACTERS; i++)
        {
            
        }
    }



} stats;