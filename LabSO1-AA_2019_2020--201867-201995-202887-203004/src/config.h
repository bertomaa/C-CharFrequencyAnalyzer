#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <tgmath.h>

#define MAX_PATH_LEN 1024
#define INITIAL_CONFIG_SIZE 128

typedef struct config
{
    int dim;
    long n;
    long m;
    int filesCount;
    char **files;
} config;

void initConfig(config *conf)
{
    conf->dim = INITIAL_CONFIG_SIZE;
    conf->n = 0;
    conf->m = 0;
    conf->filesCount = 0;
    int error = allocWrapper(INITIAL_CONFIG_SIZE, sizeof(char *), (void **)&(conf->files));
    if (error)
    {
        exit(1);
    }
}

void deallocConfig(config *c)
{
    int i;
    for (i = 0; i < c->filesCount; i++)
    {
        free(c->files[i]);
    }
    free(c->files);
}

void removeFileFromConfig(config *c, int i)
{
    if (c->filesCount == 0)
        return;
    free(c->files[i]);
    if (c->filesCount > 1)
        c->files[i] = c->files[c->filesCount - 1];
    c->filesCount--;
}

void joinConfigs(config *c1, config *c2)
{
    int totalFiles = c1->filesCount + c2->filesCount, i;
    if (totalFiles > c1->dim)
    {
        c1->files = realloc(c1->files, totalFiles + INITIAL_CONFIG_SIZE);
        c1->dim = totalFiles + INITIAL_CONFIG_SIZE;
    }
    for (i = 0; i < c2->filesCount; i++)
    {
        if (c2->files[i] != NULL)
        {
            //TODO: eccoci qua
            int error = allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&(c1->files[c1->filesCount]));
            strcpy(c1->files[c1->filesCount], c2->files[i]);
            c1->filesCount++;
        }
    }
    deallocConfig(c2);
}

void addFileToConfig(config *c, const char *file)
{
    //todo se il file ha spazi controllare lo escape
    int strLen = strlen(file);
    //TODO: eccoci qua
    int error = allocWrapper(strLen + 2, sizeof(char), (void **)&(c->files[c->filesCount])); //Non so perchè strlen+2, forse poteva essere 1 ma non voglio rischiarmela
    strcpy(c->files[c->filesCount], file);
    if (c->files[c->filesCount][strLen - 1] == '/')
        c->files[c->filesCount][strLen - 1] = '\0';
    else
        c->files[c->filesCount][strLen] = '\0';
    c->filesCount++;
}

void printFiles(const config *c)
{
    if (c->filesCount == 0)
    {
        printf("there are no files loaded\n");
        return;
    }
    printf("files ready to be analyzed are: ");
    int i;
    for (i = 0; i < c->filesCount; i++)
    {
        printf("%s", c->files[i]);
        if (i != c->filesCount - 1)
            printf(", ");
    }
    printf("\n");
}

char **exportAsArguments(const config *c)
{
    //TODO: definire meglio quanto allocare
    char **res;
    int error = allocWrapper(c->dim + 5, sizeof(char), (void **)&(res)); //TODO: esatto, per tutti
    if(e)
    error = allocWrapper(40, sizeof(char), (void **)&(res[0]));
    error = allocWrapper(40, sizeof(char), (void **)&(res[1]));
    error = allocWrapper(40, sizeof(char), (void **)&(res[2]));
    sprintf(res[1], "%ld", c->n);
    sprintf(res[2], "%ld", c->m);
    printf("\n\n\n\n\n\n\n0: %s, 1: %s", res[0], res[1]);
    int i;
    for (i = 3; i < c->filesCount + 3; i++)
    {
        res[i] = c->files[i - 3];
    }
    return res;
}

char *getCommandOutput(const char *cmd)
{
    //TODO:dite che ce lo lascia usare?
    char buffer[MAX_PIPE_CHARACTERS];
    char *ret;
    int size = 0;
    int error = allocWrapper(MAX_PIPE_CHARACTERS, sizeof(char), (void **)&ret);
    if (error)
    {
        //TODO: gestisci errore
        exit(1);
    }
    FILE *fp = popen(cmd, "r");
    if (fp == NULL)
    {
        //TODO: gestisci errore
        exit(1);
    }
    while (fgets(buffer, MAX_PIPE_CHARACTERS, fp) != NULL)
    {
        size += strlen(buffer) + 1;
        if (size >= MAX_PIPE_CHARACTERS) //TODO: gestisci errore, stringa più lunga del massimo
            exit(1);
        strcat(ret, buffer);
    };
    return ret;
}

//returns boolean values 0 is file 1 is directory if doesnt exists
int isPathDirectory(const char *path)
{
    int pathLen = strlen(path);
    char command[MAX_PIPE_CHARACTERS];
    strcat(command, "ls -la ");
    strcat(command, path);
    char *output = getCommandOutput(command);
    int ret = 0;
    switch (output[0])
    {
    case '-':
        ret = 0;
        break;
    case 't':
        ret = 1;
        break;
    default:
        ret = -1;
        //TODO: exit
        break;
    }
    free((void *)output);
    return ret;
}

config getElementsInDirectory(char *path, int recursive)
{
    int count;
    char buffer[MAX_PIPE_CHARACTERS];
    config conf;
    initConfig(&conf);

    buffer[0] = '\0';
    strcat(buffer, "ls ");
    strcat(buffer, path);
    char *output = getCommandOutput(buffer);
    //printf("ls output = %s.\n\n", output);

    //retrieve all entries
    char *newPath = strtok(output, "\n");
    //loop through the string to extract all other tokens
    char newFullPath[MAX_PATH_LEN];

    while (newPath != NULL)
    {
        strcat(newFullPath, path);
        strcat(newFullPath, "/");
        strcat(newFullPath, newPath);
        if (isPathDirectory(newFullPath) && recursive)
        {
            config recursiveRes = getElementsInDirectory(newFullPath, recursive);
            joinConfigs(&conf, &recursiveRes);
        }
        else
        {
            addFileToConfig(&conf, newFullPath);
        }
        newPath = strtok(NULL, "\n");
    }

    free((void *)output);
    return conf;
}

config checkDirectories(config conf, int recursive)
{
    config res;
    initConfig(&res);
    int i = 0;
    int count = conf.filesCount;
    for (i = 0; i < count; i++)
    {
        if (isPathDirectory(conf.files[i]))
        {
            //TODO: ? ask if the user wants to scan down this directory recursively and change recursive
            printf("opening directory %s\n", conf.files[i]);
            removeFileFromConfig(&res, i);
            //get all the files in the directory
            config recursiveRes = getElementsInDirectory(conf.files[i], recursive);
            joinConfigs(&res, &recursiveRes);
        }
    }
    return res;
}

#endif