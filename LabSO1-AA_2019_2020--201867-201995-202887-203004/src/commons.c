#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "commons.h"
#include "wrapper.h"
#include "forkHandler.h"

int isCollectingGarbage = 0;
garbageCollector gc;

/*
void printProgressBar(int now, int tot)
{
    int i = 0;
    char *sizePtr;
    char *size = getCommandOutput("tput cols", 10);
    int cols = (strtol(size, &sizePtr, 10)) - 2;
    printf("\r[");
    int progress = ((float)now / (float)tot) * cols;
    for (i = 0; i < progress; i++)
    {
        printf("#");
    }
    for (i = 0; i < (cols - progress); i++)
    {
        printf("-");
    }
    printf("]");
}*/

void fatalErrorHandler(char *message, int errorCode)
{
    fprintf(stderr, "%s\n", message);
    alertFather(1);
    // collectGarbage();
    // exit(errorCode);
}

int initGC()
{
    isCollectingGarbage = 0;
    gc.dim = 100;
    gc.garbageCount = 0;
    gc.garbage = (void **)calloc(gc.dim, sizeof(void *));
    int count = 20;
    while (gc.garbage == NULL && count > 0)
    {
        sleep(1);
        gc.garbage = (void **)calloc(gc.dim, sizeof(void *));
        count--;
    }
    if (gc.garbage == NULL)
    {
        fprintf(stderr, "Cannot allocate garbage collector in memory. Quit.\n");
        return 1;
    }
    return 0;
}

char *getLine()
{
    int size = MAX_COMMAND_LEN;
    int i = 0;
    char *line;
    allocWrapper(size, sizeof(char), (void **)&line);
    char c = 7;
    while (c != '\n')
    {
        scanf("%c", &c);
        if (i + 1 >= size)
        {
            size += MAX_COMMAND_LEN;
            reallocWrapper((void **)&line, size * sizeof(char));
        }
        line[i] = c;
        i++;
    }
    line[i - 1] = 0;
    return line;
}

void addToGC(void *garbage)
{
    int i;
    for (i = 0; i < gc.garbageCount; i++)
    {
        if (gc.garbage[i] == garbage)
        {
            return;
        }
    }
    if (gc.garbageCount == gc.dim - 10)
    {
        gc.dim += 100;
        gc.garbage = realloc(gc.garbage, gc.dim * sizeof(void *));
        int count = 20;
        while (gc.garbage == NULL && count > 0)
        {
            sleep(1);
            gc.garbage = realloc(gc.garbage, gc.dim * sizeof(void *));
            count--;
        }
        if (gc.garbage == NULL)
        {
            fatalErrorHandler("Cannot realloc memory for GC, Quit.", 1);
        }
    }
    gc.garbage[gc.garbageCount] = garbage;
    gc.garbageCount++;
}

void removeFromGC(void *p)
{
    int i;
    for (i = 0; i < gc.garbageCount; i++)
    {
        if (gc.garbage[i] == p)
        {
            gc.garbage[i] = gc.garbage[gc.garbageCount - 1];
            gc.garbageCount--;
            return;
        }
    }
    printf("not present in GC\n");
}

void removeFromGCAndFree(void *p)
{
    int i;
    for (i = 0; i < gc.garbageCount; i++)
    {
        if (gc.garbage[i] == p)
        {
            // printf("freeing %p\n", p);
            free(gc.garbage[i]);
            gc.garbage[i] = gc.garbage[gc.garbageCount - 1];
            gc.garbageCount--;
            return;
        }
    }
}

int getFilesCountInPath(char *path)
{
    char *sizeptr;
    char *cmdOut;
    char *command;
    allocWrapper(MAX_COMMAND_LEN, sizeof(char), (void **)&command);
    allocWrapper(40, sizeof(char), (void **)&cmdOut);
    sprintf(command, "find %s -type f | wc -l", path);
    cmdOut = getCommandOutput(command, 40);
    int ret =  strtol(cmdOut, &sizeptr, 10);
    //removeFromGCAndFree(command);
    //removeFromGCAndFree(cmdOut);
    return ret;
}

void collectGarbage()
{
    isCollectingGarbage = 1;
    int i;
    for (i = gc.garbageCount - 1; i >= 0; i--)
    {
        // printf("%d: deleting %p\n", i, gc.garbage[i]);
        // printf("collecting %p\n", gc.garbage[i]);
        free(gc.garbage[i]);
        // printf("collecting\n");
    }
    free(gc.garbage);
    //printf("freed %d elements in memory\n", gc.garbageCount);
}

int isFileNameAcceptable(char *fileName)
{
    int i;
    int len = strlen(fileName);
    for (i = 0; i < len; i++)
    {
        if (fileName[i] == '\"')
        {
            return 0;
        }
    }
    return 1;
}

void addDoubleQuotes(char *buffer, char *path)
{
    buffer[0] = '\0';
    sprintf(buffer, "\"%s\"", path);
    // strcat(buffer, "\"");
    // strcat(buffer, path);
    // strcat(buffer, "\"");
}

char *removeDoubleQuotes(char *buffer, char *path)
{
    int i;
    for (i = 0; i < strlen(path) - 2; i++)
    {
        buffer[i] = path[i + 1];
    }
    buffer[i] = '\0';
    return buffer;
}

char *getCommandOutput(const char *cmd, int bufferSizeInBytes)
{
    //printf("cmd: %s\n", cmd);
    char cmdBuffer[MAX_COMMAND_LEN];
    char *ret;
    int size = 0;
    allocWrapper(bufferSizeInBytes, sizeof(char), (void **)&ret);
    FILE *fp = popen(cmd, "r");
    if (fp == NULL)
    {
        fatalErrorHandler("Error in executing bash command, quit.", 1);
    }
    int res = 0;
    while (fgets(cmdBuffer, bufferSizeInBytes, fp) != NULL)
    {
        res = 1;
        size += strlen(cmdBuffer) + 1;
        if (size >= bufferSizeInBytes)
        {
            reallocWrapper((void**) &ret, bufferSizeInBytes * 2);
            bufferSizeInBytes *= 2;
        }
        strcat(ret, cmdBuffer);
    };
    return res ? ret : NULL;
}

int getPipeIndex(int index, int type)
{
    return index * 2 + type;
}

char *splitString(char *buffer, char **str, char delimiter)
{
    //printf("buffer: %s, str: %s, delimiter: %c", buffer, *str, delimiter);
    int i;
    for (i = 0; i < strlen(*str) && (*str)[i] != delimiter; i++)
    {
        buffer[i] = (*str)[i];
    }

    //printf("buffer: %s, str: %s, delimiter: %c", buffer, *str, delimiter);
    buffer[i] = '\0';
    if ((*str)[i] == delimiter || i != 0)
    {
        *str = *str + i + 1;
        return buffer;
    }
    return NULL;
}

char *splitStringWithQuotes(char *buffer, char **str, char delimiter)
{
    if ((*str)[0] == '"' || (*str)[0] == '\'')
    {
        char *tmpStr = (*str) + 1;
        char *res = splitString(buffer, &tmpStr, (*str)[0]);
        if (res != NULL)
        {
            *str = tmpStr + 1;
            return res;
        }
    }
    return splitString(buffer, str, delimiter);
}

int getDigits(int n)
{
    if (n == 0)
        return 1;
    int count = 0;
    while (n != 0)
    {
        n /= 10;
        ++count;
    }
    return count;
}

//returns 1 if the first (length of s1) characters of s2 are equal to s1, 0 if they are not equal or s2 is shorter than s1
int does1StringMatch2(char *s1, char *s2)
{
    if (strlen(s1) > strlen(s2))
        return 0;
    int res = 1, i;
    for (i = 0; i < strlen(s1); i++)
    {
        if (s1[i] != s2[i])
        {
            res = 0;
            break;
        }
    }
    return res;
}

void reverse(pairIntInt arr[], int n)
{
    int i;
    pairIntInt tmp;
    for(i = 0; i < n/2; i++)
    {
        tmp = arr[n - i];
        arr[n - i] = arr[i];
        arr[i] = tmp;
    }
}

void merge(pairIntInt arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    pairIntInt L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2)
    {
        if (L[i].second <= R[j].second)
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(pairIntInt arr[], int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        merge(arr, l, m, r);
    }
}

pairIntInt makePair(int f, int s)
{
    pairIntInt res;
    res.first = f;
    res.second = s;
    return res;
}