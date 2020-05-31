#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <tgmath.h>
#include "commons.h"
#include "stats.h"


void initStats(stats *stat, int _fileID)
{
    stat->fileID = _fileID;
    int i;
    for (i = 0; i < ASCII_CHARACTERS; i++)
    {
        stat->frequencies[i] = 0;
    }
}

//returns 0 if decode has success
int decode(const char *str, stats *res, int *i)
{
    char c;
    int currentCharacter = 0, numberStartingIndex = *i;
    int index = *i;
    char buffer[20];
    for (index = index; str[index] != '-' && str[index] != '\0'; index++)
    {
        if (currentCharacter >= ASCII_CHARACTERS)
        {
            fprintf(stderr, "String decode error, too many entries.\n");
            return 1;
        }
        if (str[index] == '.' || str[index] == '\0')
        {
            memcpy(buffer, &str[numberStartingIndex], index - numberStartingIndex);
            buffer[index - numberStartingIndex] = '\0';
            res->frequencies[currentCharacter] += atoi(buffer);
            numberStartingIndex = index + 1;
            currentCharacter++;
        }
    }
    if (currentCharacter < ASCII_CHARACTERS - 1)
    {
        fprintf(stderr, "String decode error, too few entries. There were %d.\n", currentCharacter);
        return 1;
    }
    *i = index;
    return 0;
}

int decodeMultiple(const char *str, stats *array)
{
    char buffer[20];
    int statCounter = 0, i, idStartingIndex = 0;
    int stringLen = strlen(str);
    for (i = 0; i < stringLen; i++)
    {
        if (str[i] == '-')
        {
            idStartingIndex = i + 1;
        }
        if (str[i] == '_')
        {
            memcpy(buffer, &str[idStartingIndex], i - idStartingIndex);
            buffer[i - idStartingIndex] = '\0';
            array[statCounter].fileID = atoi(buffer);
            i++;
            int decodeError = decode(str, array + statCounter, &i);
            idStartingIndex = i + 1;
            if (decodeError)
            {
                return 1;
            }
            statCounter++;
        }
    }
    return 0;
}

char *encode(const stats stat)
{
    //TODO: fai la free da qualche parte, allocare col wrapper
    char *buffer = (char *)calloc(MAX_PIPE_CHARACTERS, sizeof(char));
    int i, pointer;
    sprintf(buffer, "-%d_", stat.fileID);
    //printf("\nwrinting id: -%d_\n", stat.fileID);
    pointer = getDigits(stat.fileID) + 2;
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
    buffer[pointer + 1] = '\0';
    return buffer;
}

char *encodeMultiple(stats *statsArray, int len)
{
    char *res;
    int offset = 0, i;
    //TODO: check error
    int error = allocWrapper(MAX_PIPE_CHARACTERS * len, sizeof(char), (void **)&res);
    if (error)
    {
        printf("Errore allocazione encode multiple");
        exit(1);
    }
    for (i = 0; i < len; i++)
    {
        char *str = encode(statsArray[i]);
        int stringLen = strlen(str);
        memcpy(res + offset, str, stringLen);
        offset += stringLen;
    }
    return res;
}

//sums the content of first and second modifying first
void sumStats(stats *first, const stats *second)
{
    int i;
    for (i = 0; i < ASCII_CHARACTERS; i++)
    {
        first->frequencies[i] += second->frequencies[i];
    }
}

void printStats(const stats s)
{
    printf("stat:\n");
    int i;
    for (i = 0; i < ASCII_CHARACTERS; i++)
    {
        if (s.frequencies[i] != 0)
            printf("%c : %d\n", i, s.frequencies[i]);
    }
    printf("\n\n");
}

// void writeStatsToFile(const stats s)
// { 
//     FILE* fd;
//     int i;
//     remove("stats.txt");
//     fd = fopen("stats.txt", "w");
//     for(i = 0; i < ASCII_CHARACTERS; i++)
//     {
//         if(s.frequencies[i] != 0 && i != 13 && i != 10 && i != 34 && i != 39)
//             fprintf(fd, "\"%c\" : %d, ", i, s.frequencies[i]);
//     }
//     fseek(fd, -2, SEEK_END);
//     fprintf(fd, "}");
//     fclose(fd);
//     int i, fd;
//     fd = open("stats.txt", O_RDONLY);
//     char *str = encode(s);
//     sprintf(fd, str);
//     close(fd);
//     free(str);
// }