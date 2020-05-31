#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <tgmath.h>
#include "commons.h"
#include "wrapper.h"

typedef struct config
{
    int dim;
    int n;
    int m;
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

// void deallocConfig(config *c)
// {
//     int i;
//     for (i = 0; i < c->filesCount; i++)
//     {
//         // free(c->files[i]);
//     }
//     // free(c->files);
// }

void removeFileFromConfigByIndex(config *c, int i)
{
    if (c->filesCount == 0)
        return;
    // free(c->files[i]);
    if (c->filesCount > 1)
        c->files[i] = c->files[c->filesCount - 1];
    c->filesCount--;
}

void removeFileFromConfigByName(config *c, char* name)
{
    int i;
    for(i = 0; i < c->filesCount; i++){
        if(strcmp(c->files[i], name) == 0)
        {
            removeFileFromConfigByIndex(c, i);
            return;
        }
    }
}

void removePathFromConfig(config *c, char *path)
{
    char *buffer;
    int error = allocWrapper(MAX_PATH_LEN, sizeof(char), (void **) &buffer);
    //TODO: pupù
    char cmd[MAX_COMMAND_LEN];
    char *cmdOutput;
    allocWrapper(MAX_PIPE_CHARACTERS, sizeof(char), (void **)&cmdOutput); //TODO: piglia e gestisici errore
    addDoubleQuotes(buffer, path);
    sprintf(cmd, "find %s -type f", buffer);
    cmdOutput = getCommandOutput(cmd);
    buffer = splitString(buffer, &cmdOutput, '\n');
    while (buffer != NULL)
    {
        printf("remove file: %s.\n", buffer);
        removeFileFromConfigByName(c, buffer);
        buffer = splitString(buffer, &cmdOutput, '\n');
    }
}

void joinConfigs(config *c1, config *c2)
{
    int totalFiles = c1->filesCount + c2->filesCount, i;
    if (totalFiles > c1->dim)
    {
        reallocWrapper((void **)&(c1->files), totalFiles + INITIAL_CONFIG_SIZE * sizeof(char *));
        c1->dim = totalFiles + INITIAL_CONFIG_SIZE;
    }
    for (i = 0; i < c2->filesCount; i++)
    {
        if (c2->files[i] != NULL)
        {
            int error = allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&(c1->files[c1->filesCount]));
            //TODO: eccoci qua
            strcpy(c1->files[c1->filesCount], c2->files[i]);
            c1->filesCount++;
        }
    }
    //printf("filesCount:%d\n", c1->filesCount);
    // deallocConfig(c2);
}

void addFileToConfig(config *c, const char *file)
{
    //todo se il file ha spazi controllare lo escape
    int stringLen = strlen(file);
    if (stringLen >= MAX_PATH_LEN)
    {
        fprintf(stdout, "Impossible to add file \"%s\" to configuration, file name too long. Max is %d.\n", file, MAX_PATH_LEN);
        return;
    }
    //TODO: eccoci qua
    //TODO: aggiungere al realloc
    if (c->filesCount == c->dim)
    {
        c->dim += INITIAL_CONFIG_SIZE;
        int error = reallocWrapper((void **)&(c->files), c->dim * sizeof(char *));
        //TODO: check error
    }

    int error = allocWrapper(stringLen + 2, sizeof(char), (void **)&(c->files[c->filesCount])); //Non so perchè stringLen+2, forse poteva essere 1 ma non voglio rischiarmela

    strcpy(c->files[c->filesCount], file);
    if (c->files[c->filesCount][stringLen - 1] == '/')
        c->files[c->filesCount][stringLen - 1] = '\0';
    else
        c->files[c->filesCount][stringLen] = '\0';
    c->filesCount++;
}

void printFiles(const config *c)
{
    if (c->filesCount == 0)
    {
        printf("there are no files loaded\n");
        return;
    }
    printf("files ready to be analyzed are:\n");
    int i;
    for (i = 0; i < c->filesCount; i++)
    {
        printf("%d : %s", i, c->files[i]);
        // if (i != c->filesCount - 1)
        //     printf(", ");
        printf("\n");
    }
    printf("\n");
}

char **exportAsArguments(const config *c, char* arg0)
{
    //TODO: definire meglio quanto allocare
    char **res;
    int error = allocWrapper(c->dim + 5, sizeof(char), (void **)&(res)); //TODO: esatto, per tutti
    error = allocWrapper(40, sizeof(char), (void **)&(res[0]));
    error = allocWrapper(40, sizeof(char), (void **)&(res[1]));
    error = allocWrapper(40, sizeof(char), (void **)&(res[2]));
    sprintf(res[0], "%s", arg0);
    sprintf(res[1], "%d", c->n);
    sprintf(res[2], "%d", c->m);
    int i;
    for (i = 3; i < c->filesCount + 3; i++)
    {
        res[i] = c->files[i - 3];
    }
    return res;
}

// //returns boolean values 0 is file 1 is directory if doesnt exists
// int isPathDirectory(const char *path)
// {
//     //printf("IsPathDirectory:\n");
//     int pathLen = strlen(path);
//     char command[MAX_PIPE_CHARACTERS];
//     command[0] = '\0';
//     strcat(command, "ls -la ");
//     strcat(command, path);
//     char *output = getCommandOutput(command);
//     //printf("output:%s\n", output);
//     int ret = 0;
//     switch (output[0])
//     {
//     case '-':
//         ret = 0;
//         break;
//     case 't':
//         ret = 1;
//         break;
//     default:
//         ret = -1;
//         //TODO: exit
//         break;
//     }
//     // free((void *)output);
//     return ret;
// }

// config getElementsInDirectory(char *path, int recursive)
// {
//     //printf("GetElementsInDirectory, path:%s\n", path);
//     int count;
//     char cmdBuffer[MAX_COMMAND_LEN];
//     char *newPath;
//     int error = allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&newPath);
//     //TODO: check error
//     config conf;
//     initConfig(&conf);

//     cmdBuffer[0] = '\0';
//     strcat(cmdBuffer, "ls ");
//     strcat(cmdBuffer, path);
//     char *output = getCommandOutput(cmdBuffer);
//     //printf("ls output = %s.\n\n", output);

//     //retrieve all entries
//     newPath = splitString(newPath, &output, '\n');
//     //printf("newpath0: %s\n", newPath);
//     //loop through the string to extract all other tokens
//     char newFullPath[MAX_PATH_LEN];
//     char pathWithoutQuotes[MAX_PATH_LEN];
//     char newFullPathWithoutQuotes[MAX_PATH_LEN];
//     removeDoubleQuotes(pathWithoutQuotes, path);

//     while (newPath != NULL)
//     {
//         //printf("newpath1: %s\n", newPath);
//         newFullPath[0] = '\0';
//         strcat(newFullPath, "\"");
//         strcat(newFullPath, pathWithoutQuotes);
//         strcat(newFullPath, "/");
//         strcat(newFullPath, newPath);
//         strcat(newFullPath, "\"");
//         //printf("newFullPath:%s\n", newFullPath);
//         int isDirectory = isPathDirectory(newFullPath);
//         if (isDirectory)
//         {
//             if (recursive)
//             {
//                 config recursiveRes = getElementsInDirectory(newFullPath, recursive);
//                 joinConfigs(&conf, &recursiveRes);
//             }
//         }
//         else
//         {
//             //printf("aggiunto file %s\n", newFullPath);
//             addFileToConfig(&conf, removeDoubleQuotes(newFullPathWithoutQuotes, newFullPath));
//         }
//         //printf("newpath1: %s\n", newPath);
//         newPath = splitString(newPath, &output, '\n');
//         //printf("newpath2: %s\n", newPath);
//     }
//     //printf("esci\n");

//     return conf;
// }

// config checkDirectories2(config *conf, int recursive)
// {
//     char buffer[MAX_PATH_LEN];
//     int i = 0;
//     int count = conf->filesCount;

//     for (i = 0; i < count; i++)
//     {
//         addDoubleQuotes(buffer, conf->files[i]);
//         if (isPathDirectory(buffer))
//         {
//             //TODO: ? ask if the user wants to scan down this directory recursively and change recursive
//             //printf("opening directory %s\n", buffer);
//             removeFileFromConfig(conf, i);
//             //get all the files in the directory
//             config recursiveRes = getElementsInDirectory(buffer, recursive);
//             joinConfigs(conf, &recursiveRes);
//         }
//         printFiles(conf);
//     }    
//     return *conf;
// }

config *checkDirectories(config *conf)
{
    config *res;
    allocWrapper(1, sizeof(config), (void **)&res);
    initConfig(res);
    res->n = conf->n;
    res->m = conf->m;
    char *buffer, *fullBuffer;
    int error = allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&fullBuffer);
    buffer = fullBuffer;
    //TODO: pupù
    char cmd[MAX_COMMAND_LEN];
    char *cmdOutput;
    allocWrapper(MAX_PIPE_CHARACTERS, sizeof(char), (void **)&cmdOutput); //TODO: piglia e gestisici errore
    int i = 0;
    int count = conf->filesCount;
    for (i = 0; i < count; i++)
    {
        addDoubleQuotes(buffer, conf->files[i]);
        sprintf(cmd, "find %s -type f", buffer);
        cmdOutput = getCommandOutput(cmd);
        printf("cmdo: %s\n", cmdOutput);
        buffer = splitString(buffer, &cmdOutput, '\n');
        while (buffer != NULL)
        {
            printf("file: %s.\n", buffer);
            addFileToConfig(res, buffer);
            buffer = splitString(buffer, &cmdOutput, '\n');
        }
        buffer = fullBuffer;
    }

    return res;
}
