#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>


#define READ 0
#define WRITE 1
#define ASCII_CHARACTERS 256
#define MAX_CHARACTERS 8128

typedef struct stats{
    int frequencies[ASCII_CHARACTERS];
} stats;

//returns 0 if decode has success
int decode(const char* str)
{
    char c;
    int currentCharacter, index, numberStartingIndex;
    char buffer[20];
    int stringLength = strlen(str);
    for(index = 0; index < stringLength, index++)
    {
        if(currentCharacter >= ASCII_CHARACTERS)
        {
            fprintf(stderr, "String decode error, too many entries.\n");
            return 1;
        }
        if(str[index] == '.')
        {
            char* currentNumber = memcpy( buffer, &str[numberStartingIndex], index - NumberStartingIndex);
            buffer[index - NumberStartingIndex] = '\0';
            frequencies[currentCharacter] = atoi(buffer);
            numberStartingIndex = index + 1;
            currentCharacter++;
        }
    }
    if(currentCharacter < ASCII_CHARACTERS)
    {
        fprintf(stderr, "String decode error, too few entries.\n");
        return 1;
    }
    return 0;
}

char* encode()
{
    char* buffer[MAX_CHARACTERS];
    int i;
    for(i = 0; i < ASCII_CHARACTERS; i++)
    {
        sprintf(buffer, "%d", frequencies[i]);
        if(i < ASCII_CHARACTERS - 1)
            sprintf(buffer, ".");  
    }
    sprintf(buffer,'\0');
    return buffer;
}

void sumStats(stats *other, int count)
{

}
