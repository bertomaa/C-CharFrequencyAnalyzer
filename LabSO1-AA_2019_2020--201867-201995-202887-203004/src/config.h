#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <tgmath.h>
#include "commons.h"
#include "wrapper.h"
#include "stats.h"

typedef struct config
{
    int dim;
    int n;
    int m;
    int filesCount;
    char **files;
} config;

typedef struct confAndStats
{
    config *conf;
    stats *stats;
} confAndStats;

void initConfig(config *conf);

void removeFileFromConfigByIndex(config *c, int i);

void removeFileFromConfigByName(config *c, char *name);

int getFileIndexInConfig(config *c, const char *name);

void removePathFromConfAndStats(confAndStats *c, char *path);

void removePathFromConfig(config *c, char *path);

void joinConfigs(config *c1, config *c2);

void addFileToConfig(config *c, const char *file);

void deallocConfig(config*c);

void printFiles(const config *c);

char **exportAsArguments(const config *c, char *arg0);

config *checkDirectories(config *conf);

#endif