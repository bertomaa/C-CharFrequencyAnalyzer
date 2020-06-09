#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "stats.h"

void printDivider(int cellInCurrentRow, int maxDigits, int layer, int toSwitch)
{
    int j, k;
    switch (layer)
    {
    case 0:
        printf("\n┌");
        break;
    case 1:
        printf("\n├");
        break;
    case 2:
        printf("\n└");
        break;
    }
    for (j = 0; j < cellInCurrentRow; j++)
    {
        for (k = 0; k < maxDigits + 5; k++)
        {
            printf("─");
        }
        if (j != cellInCurrentRow - 1)
        {
            if (j >= toSwitch)
            {
                layer = 2;
            }
            switch (layer)
            {
            case 0:
                printf("┬");
                break;
            case 1:
                printf("┼");
                break;
            case 2:
                printf("┴");
                break;
            }
        }
    }
    if (toSwitch + 1 == cellInCurrentRow)
        layer = 2;
    switch (layer)
    {
    case 0:
        printf("┐");
        break;
    case 1:
        printf("┤");
        break;
    case 2:
        printf("┘");
        break;
    }
    printf("\n");
}

void printSpaces(int n)
{
    int i;
    for (i = 0; i < n; i++)
        printf(" ");
}

void printTable(char *categoryName, int maxDigits, int RowLength, int *indexes, stats *stat, int cellCount, int totLen, int frequencies, long totalChars)
{
    int linesWritten = 0;
    int cellInCurrentRow = 0;
    int i = 0;
    int cellsInRow = RowLength / (maxDigits + 6) < cellCount ? RowLength / (maxDigits + 6) : cellCount;
    printSpaces((RowLength / 2) - (strlen(categoryName) / 2));
    printf("%s\n", categoryName);
    printDivider(cellsInRow, maxDigits, 0, cellCount);
    printf("│");
    for (i = 0; i < totLen; i++)
    {
        if (indexes[i] == -1)
            continue;
        if ((cellInCurrentRow + 1) * (maxDigits + 6) > RowLength)
        {
            linesWritten++;
            int remainder = cellCount - linesWritten * cellsInRow;
            printDivider(cellInCurrentRow, maxDigits, 1, remainder);
            printf("│");
            cellInCurrentRow = 0;
        }
        if (frequencies)
            printf(" %c: %6.3f %% ", (char)indexes[i], (float)stat->frequencies[indexes[i]] * 100 / totalChars);
        else
        {
            printf(" %c: %d ", (char)indexes[i], stat->frequencies[indexes[i]]);
            printSpaces(maxDigits - getDigits(stat->frequencies[indexes[i]]));
        }
        printf("│");
        cellInCurrentRow++;
    }
    printDivider(cellInCurrentRow, maxDigits, 2, cellCount);
    printf("\n");
}

int getMaxDigits(stats s)
{
    int max = 0, i = 0, digits;
    for (i = 0; i < ASCII_CHARACTERS; i++)
    {
        digits = getDigits(s.frequencies[i]);
        if (digits > max)
        {
            max = digits;
        }
    }
    return max;
}

int removeZeroes(stats *s, int *indexes, int n)
{
    int count = 0;
    int i;
    for (i = 0; i < n; i++)
    {
        if(indexes[i] != -1)
        {
            if (s->frequencies[indexes[i]] == 0)
                indexes[i] = -1;
            else
            {
                count++;
            }
        }
    }
    return count;
}

void sortArrayByFrequencies(stats *s, int index[], int n, int order)
{
    pairIntInt resPairs[n];
    int i;
    for (i = 0; i < n; i++)
    {
        resPairs[i] = makePair(index[i], s->frequencies[index[i]]);
    }
    if (order != 0)
    {
        mergeSort(resPairs, 0, n - 1);
        if (order == -1)
            reverse(resPairs, n - 1);
    }
    for (i = 0; i < n; i++)
    {
        index[i] = resPairs[i].first;
    }
}

void printCategory(stats *stat, char *label, int index[], int size, int order, int frequencies, int maxDigits, int cols, int totalChars)
{
    int len = removeZeroes(stat, index, size);
    if (order != 0)
    {
        sortArrayByFrequencies(stat, index, size, order);
    }
    if (len > 0)
        printTable(label, maxDigits, cols, index, stat, len, size, frequencies, totalChars);
}

// toPrint indexes mean:
// 0: all
// 1: numbers
// 2: uppercase
// 3: lowercase
// 4: punctuation
// 5: symbols
// 6: others
// 7: non-printable
void print(stats stat, int frequencies, int order, int toPrint[])
{
    int i;
    char *endPtr;
    int maxDigits;
    if (frequencies)
        maxDigits = 8;
    else
        maxDigits = getMaxDigits(stat);
    char *size = getCommandOutput("tput cols", 40);
    int cols = (strtol(size, &endPtr, 10)) - 2;
    int totalChars = 0;
    if (frequencies)
    {
        for (i = 0; i < ASCII_CHARACTERS; i++)
            totalChars += stat.frequencies[i];
    }
    int nonPrintable[32];
    for (i = 0; i < 32; i++)
    {
        if (i != 11 && i != 12)
            nonPrintable[i] = i;
        else
            nonPrintable[i] = -1;
    }
    int symbols[22] = {35, 36, 37, 38, 42, 43, 44, 45, 47, 60, 61, 62, 64, 91, 92, 93, 94, 95, 123, 124, 125, 126};
    int numbers[10];
    for (i = 0; i < 10; i++)
    {
        numbers[i] = i + 48;
    }
    int uppercase[26];
    for (i = 0; i < 26; i++)
    {
        uppercase[i] = i + 65;
    }
    int lowercase[26];
    for (i = 0; i < 26; i++)
    {
        lowercase[i] = i + 97;
    }
    int punctuation[9] = {33, 34, 39, 40, 41, 58, 59, 63, 96};
    int others[127];
    for (i = 0; i < 127; i++)
    {
        others[i] = i + 128;
    }

    int all[ASCII_CHARACTERS - 33];
    for (i = 0; i < ASCII_CHARACTERS - 33; i++)
    {
        if (i + 32 != 127)
            all[i] = i + 32;
        else
            all[i] = -1;
    }

    int *allIndexes[7] = {all, numbers, uppercase, lowercase, punctuation, symbols, others};
    int sizes[7] = {ASCII_CHARACTERS - 33, 10, 26, 26, 9, 22, 127};
    char *labels[7] = {"ALL", "NUMBERS", "UPPERCASE", "LOWERCASE", "PUNCTUATION", "SYMBOLS", "OTHERS"};

    for (i = 0; i < 7; i++)
    {
        if(toPrint[i])
            printCategory(&stat, labels[i], allIndexes[i], sizes[i], order, frequencies, maxDigits, cols, totalChars);
    }

    int nonPrintableLen = removeZeroes(&stat, nonPrintable, 32);

    if (toPrint[7])
    {
        int npSum = 0;
        for (i = 0; i < 32; i++)
        {
            if (nonPrintable[i] >= 0)
                npSum += stat.frequencies[nonPrintable[i]];
        }
        if(npSum != 0)
        {
            if (frequencies)
                printf("\nThere are %d different non-printable characters, for a total of %6.3f %%.\n", nonPrintableLen, (float)npSum * 100 / totalChars);
            else
                printf("\nThere are %d different non-printable characters, for a total of %d occurrencies.\n", nonPrintableLen, npSum);
        }
    }
}