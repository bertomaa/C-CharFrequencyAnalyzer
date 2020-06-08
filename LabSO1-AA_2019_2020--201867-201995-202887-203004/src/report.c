#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "wrapper.h"
#include "stats.h"
#include "config.h"
#include "commons.h"
#include "forkHandler.h"

char *analyzerToReportPipe = "/tmp/analyzerToReport.pipe";
char *mainToReportPipe = "/tmp/mainToReport.pipe";
int FDanalyzer;

void readAnalyzer(confAndStats *conf);

// void printTable(int start, int finish, char *name, stats resultStats);

void print(stats stat);

void showHelp()
{
    printf("Allowed actions:\n");
    printf("show [-f --frequencies] [-e --group] <file1> [directory1] ...      - Shows the report of the specified files or directories.\n");
    printf("showall [-f --frequencies] [-e --group]                            - Shows the report of all files analyzed.\n");
    printf("help                                         - Show this help message\n");
    printf("remove <file or directory 1> ...             - Removes files (or directories) from the list to be analyzed\n");
    printf("read                                         - Read data from analyzer, this action is not required if report was created by main\n");
}

void show(char *files, confAndStats *cs)
{
    int i;
    char *buffer;
    allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&buffer);
    buffer = splitStringWithQuotes(buffer, &files, ' ');
    int every = 1;
    int 
    int match = 0;
    while (buffer != NULL)
    {
        int index = getFileIndexInConfig(cs->conf, buffer);
        if (index != -1)
        {
            printf("File %s:\n", cs->conf->files[index]);
            print(cs->stats[index]);
            buffer = splitStringWithQuotes(buffer, &files, ' ');
            continue;
        }
        match = 0;
        stats tmpStat;
        initStats(&tmpStat, -1);
        for (i = 0; i < cs->conf->filesCount; i++)
        {
            if (i == 0 && strcmp("--group", buffer) == 0)
            {
                every = 0;
                break;
            }
            if (does1StringMatch2(buffer, cs->conf->files[i]))
            {
                match = 1;
                if (every)
                {
                    printf("File %s:\n", cs->conf->files[i]);
                    print(cs->stats[i]);
                    //printStats(cs->stats[i]);
                }
                else
                {
                    sumStats(&tmpStat, &(cs->stats[i]));
                }
            }
        }
        if (!every && match)
        {
            if (strcmp(buffer, "\"\"") == 0)
                ;
            printf("Path %s:\n", buffer);
            print(tmpStat);
            //printStats(tmpStat);
        }
        buffer = splitStringWithQuotes(buffer, &files, ' ');
    }
}

void removeFiles(confAndStats *cs, char *arguments)
{
    char *buffer;
    allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&buffer);
    buffer = splitStringWithQuotes(buffer, &arguments, ' ');
    while (buffer != NULL)
    {
        printf("toRemove : %s.\n", buffer);
        removePathFromConfAndStats(cs, buffer);
        buffer = splitStringWithQuotes(buffer, &arguments, ' ');
    }
}

void showCommandNotFoundError(const char *arguments)
{
    if (arguments[0] != '\n')
        printf("Command %s is not valid, see usage with help\n", arguments);
}

int processCommand(char *command, confAndStats *cs)
{
    char *token = strtok(command, " ");
    if (token == NULL)
        return 1;

    //help, exit, read (richiama readAnalyzer), show (mostra il file/ i file nella directory) (showall mostra tutto),

    if (strcmp(token, "read") == 0)
    {
        readAnalyzer(cs);
    }
    else if (strcmp(token, "help") == 0)
    {
        showHelp();
    }
    else if (strcmp(token, "show") == 0)
    {
        show(command + 5, cs);
    }
    else if (strcmp(token, "showall") == 0)
    {
        char buffer[MAX_COMMAND_LEN];
        sprintf(buffer, "%s \"\"", command + 8);
        show(buffer, cs);
    }
    else if (strcmp(token, "remove") == 0)
    {
        removeFiles(cs, command + 7);
    }
    else if (strcmp(token, "exit") == 0 || strcmp(token, "quit") == 0 || strcmp(token, "q") == 0)
    {
        return 0;
    }
    else if (strcmp(token, "files") == 0)
    {
        printFiles(cs->conf);
    }
    else
    {
        showCommandNotFoundError(token);
    }
    return 1;
}

char *readStringFromPipe(int bufferLen, int fd)
{
    char *buffer;
    allocWrapper(bufferLen, sizeof(char), (void **)&buffer);
    char c = '7';
    int i = 0;
    while (c != 0)
    {
        read(fd, &c, 1);
        // printf("%c ", c);
        buffer[i] = c;
        i++;
    }
    buffer[i] = '\0';
    return buffer;
}

void readAnalyzer(confAndStats *cs)
{
    int lastStatsIndex = cs->conf->filesCount;
    FDanalyzer = open(analyzerToReportPipe, O_RDONLY);
    while (FDanalyzer == -1)
    {
        printf("Impossible to open pipe, did you run Analyzer first? Press anything to retry or q to quit\n");
        //int c = getchar();
        char c;
        scanf("%c", &c);
        if (c == 'q')
            fatalErrorHandler("", 1);
        FDanalyzer = open(analyzerToReportPipe, O_RDONLY);
    };
    printf("Report connected!\n");
    char *e;
    char *buffer = readStringFromPipe(40, FDanalyzer);
    int filesCount = strtol(buffer, &e, 10);
    int i;

    for (i = 0; i < filesCount; i++)
    {
        buffer = readStringFromPipe(MAX_PATH_LEN, FDanalyzer);
        // printf("%s\n", buffer);
        int index = getFileIndexInConfig(cs->conf, buffer);
        if (index != -1)
        {
            removeFileFromStatsArray(cs->stats, index, cs->conf->filesCount);
            removeFileFromConfigByIndex(cs->conf, index);
            lastStatsIndex--;
        }
        addFileToConfig(cs->conf, buffer);
        if (cs->conf->filesCount + 1 >= cs->conf->dim)
        {
            reallocWrapper((void **)&(cs->stats), (cs->conf->dim + INITIAL_CONFIG_SIZE) * sizeof(stats));
        }
    }
    char *statString;
    allocWrapper(MAX_PIPE_CHARACTERS * cs->conf->filesCount, sizeof(char), (void **)&statString);
    read(FDanalyzer, statString, MAX_PIPE_CHARACTERS * cs->conf->filesCount);
    close(FDanalyzer);
    for (i = lastStatsIndex; i < cs->conf->filesCount; i++)
    {
        initStats(&(cs->stats[i]), i);
    }
    //printf("\n\nstat string: %s\n\n", statString);
    decodeMultiple(statString, &(cs->stats[lastStatsIndex]));
    // for (i = 0; i < cs->conf->filesCount; i++)
    // {
    //     printf("In file %s was analyzed:\n", cs->conf->files[i]);
    //     //printStats(cs->stats[i]);
    //     print(cs->stats[i]);
    // }
}

int main(int argc, char *argv[])
{
    initGC();
    initProcess();
    mkfifo(analyzerToReportPipe, 0666);
    confAndStats cs;
    allocWrapper(1, sizeof(config), (void **)&(cs.conf));
    allocWrapper(INITIAL_CONFIG_SIZE, sizeof(stats), (void **)&(cs.stats));
    // TODO: terza marcia
    initConfig(cs.conf);
    printf("Waiting for Analyzer...\n");
    readAnalyzer(&cs);

    int fdFromMain;
    char *command;
    int goOn;
    if (argc > 1 && strcmp(argv[1], "--main") == 0)
    {
        do
        {
            fdFromMain = open(mainToReportPipe, O_RDONLY);
            while (fdFromMain == -1)
            {
                printf("Impossible to open pipe with main, did you run report --main from command line? Press anything to retry or q to quit\n");
                //int c = getchar();
                char c;
                scanf("%c", &c);
                if (c == 'q')
                    fatalErrorHandler("", 0);
                fdFromMain = open(mainToReportPipe, O_RDONLY);
            };
            //printf("report opened pipe\n");
            command = readStringFromPipe(MAX_COMMAND_LEN, fdFromMain);
            //printf("report cmd: %s\n", command);
            goOn = processCommand(command, &cs);
            close(fdFromMain);
        } while (goOn);
    }
    else
    {
        allocWrapper(MAX_COMMAND_LEN, sizeof(char), (void **)&command);
        do
        {
            printf("->");
            command = getLine();
            //printf("report cmd: %s\n", command);
            goOn = processCommand(command, &cs);
        } while (goOn);
    }

    printf("FINE REPORT!\n");
    collectGarbage();
    return 0;
}

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
            if(j >= toSwitch){
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
    if(toSwitch+1 == cellInCurrentRow)
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

void printTable(char *categoryName, int maxDigits, int RowLength, int *indexes, stats *stat, int cellCount, int totLen)
{
    int linesWritten = 0;
    int cellInCurrentRow = 0;
    int i = 0, j = 0;
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
            //printf("reminder: %d\n",remainder);
            printDivider(cellInCurrentRow, maxDigits, 1, remainder);
            printf("│");
            cellInCurrentRow = 0;
        }
        printf(" %c: %d ", (char)indexes[i], stat->frequencies[indexes[i]]);
        for (j = getDigits(stat->frequencies[indexes[i]]); j < maxDigits; j++)
        {
            printf(" ");
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
        if (s->frequencies[indexes[i]] == 0)
            indexes[i] = -1;
        else
            count++;
    }
    return count;
}

void print(stats stat)
{
    int i;
    char *endPtr;
    int maxDigits = getMaxDigits(stat);
    char *size = getCommandOutput("tput cols", 40);
    int cols = (strtol(size, &endPtr, 10)) - 2;

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

    int nonPrintableLen = removeZeroes(&stat, nonPrintable, 32);
    int symbolsLen = removeZeroes(&stat, symbols, 22);
    int numbersLen = removeZeroes(&stat, numbers, 10);
    int uppercaseLen = removeZeroes(&stat, uppercase, 26);
    int lowercaseLen = removeZeroes(&stat, lowercase, 26);
    int punctuationLen = removeZeroes(&stat, punctuation, 9);
    int othersLen = removeZeroes(&stat, others, 127);

    if (numbersLen > 0)
        printTable("NUMBERS", maxDigits, cols, numbers, &stat, numbersLen, 10);
    if (uppercaseLen > 0)
        printTable("UPPERCASE", maxDigits, cols, uppercase, &stat, uppercaseLen, 26);
    if (lowercaseLen > 0)
        printTable("LOWERCASE", maxDigits, cols, lowercase, &stat, lowercaseLen, 26);
    if (punctuationLen > 0)
        printTable("PUNCTUATION", maxDigits, cols, punctuation, &stat, punctuationLen, 9);
    if (symbolsLen > 0)
        printTable("SYMBOLS", maxDigits, cols, symbols, &stat, symbolsLen, 22);
    if (othersLen > 0)
        printTable("OTHERS", maxDigits, cols, others, &stat, othersLen, 127);
    int npSum = 0;
    for (i = 0; i < 32; i++)
    {
        if (nonPrintable[i] >= 0)
            npSum += stat.frequencies[nonPrintable[i]];
    }
    printf("\nThere are %d different non-printable characters, for a total of %d occurrencies.\n", nonPrintableLen, npSum);
}