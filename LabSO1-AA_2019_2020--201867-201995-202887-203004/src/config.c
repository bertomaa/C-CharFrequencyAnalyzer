#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <tgmath.h>
#include "commons.h"
#include "wrapper.h"
#include "config.h"
#include "stats.h"

void initConfig(config *conf)
{
    conf->dim = INITIAL_CONFIG_SIZE;
    conf->n = 0;
    conf->m = 0;
    conf->filesCount = 0;
    allocWrapper(INITIAL_CONFIG_SIZE, sizeof(char *), (void **)&(conf->files));
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
    if (c->filesCount > 1)
    {
        //strcpy(c->files[i], c->files[c->filesCount - 1]);
        c->files[i] = c->files[c->filesCount - 1];
    }
    c->filesCount--;
}

void removeFileFromConfigByName(config *c, char *name)
{
    int i;
    for (i = 0; i < c->filesCount; i++)
    {
        if (strcmp(c->files[i], name) == 0)
        {
            removeFileFromConfigByIndex(c, i);
            return;
        }
    }
}

int getFileIndexInConfig(config *c, const char *name)
{
    int i;
    for (i = 0; i < c->filesCount; i++)
    {
        if (strcmp(c->files[i], name) == 0)
        {
            return i;
        }
    }
    return -1;
}

void removePathFromConfig(config *c, char *path)
{
    char *buffer;
    allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&buffer);
    //TODO: pupù
    char cmd[MAX_COMMAND_LEN];
    char *cmdOutput;
    allocWrapper(MAX_PIPE_CHARACTERS, sizeof(char), (void **)&cmdOutput); //TODO: piglia e gestisici errore
    addDoubleQuotes(buffer, path);
    sprintf(cmd, "find %s -type f", buffer);
    int filesCount = getFilesCountInPath(buffer);
    cmdOutput = getCommandOutput(cmd, filesCount * MAX_PATH_LEN * sizeof(char));
    buffer = splitString(buffer, &cmdOutput, '\n');
    while (buffer != NULL)
    {
        //printf("remove file: %s.\n", buffer);
        removeFileFromConfigByName(c, buffer);
        buffer = splitString(buffer, &cmdOutput, '\n');
    }
    removeFromGCAndFree(buffer);
    removeFromGCAndFree(cmdOutput);
}

void removePathFromConfAndStats(confAndStats *c, char *path)
{
    char *buffer;
    allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&buffer);
    //TODO: pupù
    char cmd[MAX_COMMAND_LEN];
    char *cmdOutput;
    allocWrapper(MAX_PIPE_CHARACTERS, sizeof(char), (void **)&cmdOutput); //TODO: piglia e gestisici errore
    addDoubleQuotes(buffer, path);
    sprintf(cmd, "find %s -type f", buffer);
    int filesCount = getFilesCountInPath(buffer);
    cmdOutput = getCommandOutput(cmd, filesCount * MAX_PATH_LEN * sizeof(char));
    buffer = splitString(buffer, &cmdOutput, '\n');
    while (buffer != NULL)
    {
        int index = getFileIndexInConfig(c->conf, buffer);
        //printf("remove file: %s.\n", buffer);
        removeFileFromStatsArray(c->stats, index, c->conf->filesCount);
        removeFileFromConfigByIndex(c->conf, index);
        buffer = splitString(buffer, &cmdOutput, '\n');
    }
    removeFromGCAndFree(buffer);
    removeFromGCAndFree(cmdOutput);
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
            allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&(c1->files[c1->filesCount]));
            //TODO: eccoci qua
            strcpy(c1->files[c1->filesCount], c2->files[i]);
            c1->filesCount++;
        }
    }
    //printf("filesCount:%d\n", c1->filesCount);
}

void addFileToConfig(config *c, const char *file)
{
    if (getFileIndexInConfig(c, file) != -1)
    {
        return;
    }
    // printf("dim %d count %d\n", c->dim, c->filesCount);

    //TODO: se il file ha spazi controllare lo escape
    int stringLen = strlen(file);
    if (stringLen >= MAX_PATH_LEN)
    {
        fprintf(stdout, "Impossible to add file \"%s\" to configuration, file name too long. Max is %d.\n", file, MAX_PATH_LEN);
        return;
    }
    if (c->filesCount == c->dim)
    {
        c->dim += INITIAL_CONFIG_SIZE;
        reallocWrapper((void **)&(c->files), c->dim * sizeof(char *));
    }

    allocWrapper(stringLen + 2, sizeof(char), (void **)&(c->files[c->filesCount])); //Non so perchè stringLen+2, forse poteva essere 1 ma non voglio rischiarmela

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

char **exportAsArguments(const config *c, char *arg0)
{
    char **res;
    allocWrapper(c->filesCount + 5, sizeof(char *), (void **)&res);
    allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&(res[0]));
    allocWrapper(40, sizeof(char), (void **)&(res[1]));
    allocWrapper(40, sizeof(char), (void **)&(res[2]));
    sprintf(res[0], "%s", arg0);
    sprintf(res[1], "%d", c->n);
    sprintf(res[2], "%d", c->m);
    int i;
    for (i = 3; i < c->filesCount + 3; i++)
    {
        allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&(res[i]));
        res[i] = c->files[i - 3];
    }
    return res;
}

void deallocConfig(config *c)
{
    int i;
    for (i = 0; i < c->filesCount; i++)
    {
        removeFromGCAndFree(c->files[i]);
    }
    removeFromGCAndFree(c->files);
    removeFromGCAndFree(c);
}

config *checkDirectories(config *conf)
{
    config *res;
    allocWrapper(1, sizeof(config), (void **)&res);
    initConfig(res);
    res->n = conf->n;
    res->m = conf->m;
    char *buffer, *fullBuffer;
    allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&fullBuffer);
    buffer = fullBuffer;
    char cmd[MAX_COMMAND_LEN];
    char *cmdOutput;
    allocWrapper(MAX_PIPE_CHARACTERS, sizeof(char), (void **)&cmdOutput);
    int i = 0;
    int count = conf->filesCount;
    for (i = 0; i < count; i++)
    {
        if (isFileNameAcceptable(conf->files[i]))
        {
            addDoubleQuotes(buffer, conf->files[i]);
            sprintf(cmd, "find %s -type f", buffer);
            int filesCount = getFilesCountInPath(buffer);
            cmdOutput = getCommandOutput(cmd, filesCount * MAX_PATH_LEN);
            if (cmdOutput == NULL)
            {
                continue;
            }
            buffer = splitString(buffer, &cmdOutput, '\n');
            while (buffer != NULL)
            {
                if (isFileNameAcceptable(buffer))
                    addFileToConfig(res, buffer);
                else
                    fprintf(stderr, "File %s contains \" therefore it cannot be analyzed.\n", buffer);
                buffer = splitString(buffer, &cmdOutput, '\n');
            }
            buffer = fullBuffer;
        }
        else
        {
            fprintf(stderr, "Path %s contains \" therefore it cannot be analyzed.\n", conf->files[i]);
        }
    }
    deallocConfig(conf);
    removeFromGCAndFree(fullBuffer);
    return res;
}