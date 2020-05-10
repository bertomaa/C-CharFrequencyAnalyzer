#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <tgmath.h>

typedef struct config
{
    int n;
    int m;
    int filesCount;
    char **files;
} config;

void initConfig(config *conf)
{
    conf->n = 0;
    conf->m = 0;
    conf->filesCount = 0;
    int error = allocWrapper(100, sizeof(char *), (void **)&(conf->files));
    //TODO: eh già
}

void addFileToConfig(config *c, const char *file)
{
    int strLen = strlen(file);
    //TODO: eccoci qua
    int error = allocWrapper(strLen+2, sizeof(char), (void **)&(c->files[c->filesCount]));//Non so perchè strlen+2, forse poteva essere 1 ma non voglio rischiarmela
    strcpy(c->files[c->filesCount], file);
    c->files[c->filesCount][strLen + 1] = '\0';
    c->filesCount++;
}

void printFiles(const config *c)
{
    printf("files ready to be analyzed are: ");
    int i;
    for (i = 0; i < c->filesCount; i++)
    {
        printf("%s", c->files[i]);
        if(i != c->filesCount-1)
            printf(", ");
    }
    printf("\n");
}

#endif