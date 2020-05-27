#ifndef __STATS_H__
#define __STATS_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <tgmath.h>
#include "commons.h"

typedef struct stats
{
    int frequencies[ASCII_CHARACTERS];
    int fileID;
} stats;

void initStats(stats *stat, int _fileID);

//returns 0 if decode has success
int decode(const char *str, stats *res, int *i);

int decodeMultiple(const char *str, stats *array);

char *encode(const stats stat);

char *encodeMultiple(stats *statsArray, int len);

//sums the content of first and second modifying first
void sumStats(stats *first, const stats *second);

void printStats(const stats s);

#endif