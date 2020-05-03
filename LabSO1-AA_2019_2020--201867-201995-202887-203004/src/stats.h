#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <tgmath.h>

#define READ 0
#define WRITE 1
#define ASCII_CHARACTERS 256
#define MAX_CHARACTERS 8128

typedef struct stats
{
    int frequencies[MAX_CHARACTERS];
} stats;


void initStats(stats stat){
    int i;
    for(i = 0; i < MAX_CHARACTERS; i++){
        stat.frequencies[i] = 0;
    }
}

//returns 0 if decode has success
int decode(const char *str, stats *res)
{
    char c;
    int currentCharacter, index, numberStartingIndex;
    char buffer[20];
    int stringLength = strlen(str);
    for (index = 0; index < stringLength; index++)
    {
        if (currentCharacter >= ASCII_CHARACTERS)
        {
            fprintf(stderr, "String decode error, too many entries.\n");
            return 1;
        }
        if (str[index] == '.' || str[index] == '\0')
        {
            char *currentNumber = memcpy(buffer, &str[numberStartingIndex], index - numberStartingIndex);
            buffer[index - numberStartingIndex] = '\0';
            res->frequencies[currentCharacter] = atoi(buffer);
            numberStartingIndex = index + 1;
            currentCharacter++;
        }
    }
    if (currentCharacter < ASCII_CHARACTERS - 1)
    {
        fprintf(stderr, "String decode error, too few entries. There were %d.\n", currentCharacter);
        return 1;
    }
    return 0;
}

int getDigits(int n) {
    if(n == 0)
        return 1;
    int count = 0;
    while (n != 0) {
        n /= 10;
        ++count;
    }
    return count;
}

char *encode(const stats stat)
{
    //TODO: fai la free da qualche parte
    char *buffer = (char *)calloc(MAX_CHARACTERS, sizeof(char));
    int i, pointer;
    pointer = 0;
    for (i = 0; i < ASCII_CHARACTERS; i++)
    {

        sprintf(buffer + pointer, "%d", stat.frequencies[i]);
        pointer += getDigits(stat.frequencies[i]);
        if (i < ASCII_CHARACTERS - 1)
        {
            sprintf(buffer + pointer, ".");
            pointer++;
        }
    }
    sprintf(buffer + pointer, "");
    return buffer;
}

//sums the content of first and second modifying first
stats sumStats(const stats first, const stats second)
{
    stats res;
    int i;
    for (i = 0; i < ASCII_CHARACTERS; i++)
    {
        res.frequencies[i] = first.frequencies[i] + second.frequencies[i];
    }
    return res;
}

void printStats(const stats s)
{
    printf("stat:\n");
    int i;
    for(i = 0; i < MAX_CHARACTERS; i++)
    {
        if(s.frequencies[i] != 0)
            printf("'%c' : %d, ", i, s.frequencies[i]);
    }
    printf("\n\n");
}