#ifndef __PRINT_H__
#define __PRINT_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "stats.h"


void printDivider(int cellInCurrentRow, int maxDigits, int layer, int toSwitch);

void printSpaces(int n);

void printTable(char *categoryName, int maxDigits, int RowLength, int *indexes, stats *stat, int cellCount, int totLen, int frequencies, long totalChars);

int getMaxDigits(stats s);

int removeZeroes(stats *s, int *indexes, int n);

void sortArrayByFrequencies(stats *s, int index[], int n, int order);

void printCategory(stats *stat, char *label, int index[], int size, int order, int frequencies, int maxDigits, int cols, int totalChars);

void print(stats stat, int frequencies, int order, int toPrint[]);

#endif