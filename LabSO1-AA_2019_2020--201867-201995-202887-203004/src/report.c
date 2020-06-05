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

void printTable(int start, int finish, char *name, stats resultStats);

void print(stats resultStats);

char *getLine()
{
    int size = MAX_COMMAND_LEN;
    int i = 0;
    char *line;
    allocWrapper(size, sizeof(char), (void **)&line);
    //TODO: error
    char c;
    while (c != '\n')
    {
        scanf("%c", &c);
        if (i + 1 >= size)
        {
            size += MAX_COMMAND_LEN;
            reallocWrapper((void **)&line, size*sizeof(char));
            //TODO: error
        }
        line[i] = c;
        i++;
    }
    line[i - 1] = 0;
    return line;
}

void showHelp()
{
    printf("Allowed actions:\n");
    printf("show <file1> [directory1] ...       - Shows the report of the specified files or directories.\n");
    printf("showall                             - Shows the report of all files analyzed.\n");
    printf("help                                - Show this help message\n");
    printf("read                                - Read data from analyzer, this action is not required if report was created by main\n");
}

void show(char *files, confAndStats *cs)
{
    int i;
    char *pch;
    char *input; //TODO: definire meglio size da allocare
    allocWrapper(MAX_COMMAND_LEN, sizeof(char), (void **)&input);
    strcpy(input, files);
    pch = strtok(input, " ");
    while (pch != NULL)
    {
        for (i = 0; i < cs->conf->filesCount; i++)
        {
            if (does1StringMatch2(pch, cs->conf->files[i]))
            {
                printf("File %s:\n", cs->conf->files[i]);
                //printTable(cs->stats[i]);
            }
        }
    }
}

void showAll(confAndStats *cs)
{
    int i;
    for (i = 0; i < cs->conf->filesCount; i++)
    {
        printf("File %s:\n", cs->conf->files[i]);
        //printTable(cs->stats[i]);
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

        showAll(cs);
    }
    else if (strcmp(token, "exit") == 0 || strcmp(token, "quit") == 0 || strcmp(token, "q") == 0)
    {
        return 0;
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
    allocWrapper(bufferLen, sizeof(char), (void **)&buffer); //TODO: usa il wrapper,
    char c = '7';
    int i = 0;
    while (c != 0)
    {
        read(fd, &c, 1);
        // printf("%c ", c);
        buffer[i] = c;
        i++;
    }
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
            exit(0);
        FDanalyzer = open(analyzerToReportPipe, O_RDONLY);
    };
    printf("connected!\n");
    char *e;
    char *buffer = readStringFromPipe(40, FDanalyzer);
    int filesCount = strtol(buffer, &e, 10);
    int i;
    printf("a\n");

    for (i = 0; i < filesCount; i++)
    {
        buffer = readStringFromPipe(MAX_PATH_LEN, FDanalyzer);
        printf("%s\n", buffer);
        addFileToConfig(cs->conf, buffer);
        if (cs->conf->filesCount + 1 >= cs->conf->dim)
        {
            reallocWrapper((void **)&(cs->stats), (cs->conf->dim + INITIAL_CONFIG_SIZE) * sizeof(stats)); //TODO: error
        }
    }
    char *statString;
    allocWrapper(MAX_PIPE_CHARACTERS * cs->conf->filesCount, sizeof(char), (void **)&statString); //TODO: usa il wrapper,
    read(FDanalyzer, statString, MAX_PIPE_CHARACTERS * cs->conf->filesCount);
    close(FDanalyzer);
    for (i = lastStatsIndex; i < cs->conf->filesCount; i++)
    {
        initStats(&(cs->stats[i]), i);
    }
    //printf("\n\nstat string: %s\n\n", statString);
    int error = decodeMultiple(statString, &(cs->stats[lastStatsIndex])); //TODO:check error
    if (error)
    {
        fatalErrorHandler("Error in decoding pipe, exit.", 1);
    }
    for (i = 0; i < cs->conf->filesCount; i++)
    {
        printf("In file %s was analyzed:\n", cs->conf->files[i]);
        //printStats(cs->stats[i]);
        print(cs->stats[i]);
    }
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
    printf("fine\n");

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
                    exit(0);
                fdFromMain = open(mainToReportPipe, O_RDONLY);
            };

            command = readStringFromPipe(MAX_COMMAND_LEN, fdFromMain);
            printf("report cmd: %s\n", command);
            goOn = processCommand(command, &cs);
            close(fdFromMain);
        } while (goOn);
    }
    else
    {
        allocWrapper(MAX_COMMAND_LEN, sizeof(char), (void **)&command); //TODO:
        do
        {
            scanf("%2048s", command);
            printf("report cmd: %s\n", command);
            goOn = processCommand(command, &cs);
        } while (goOn);
    }

    printf("FINE REPORT!\n");
    collectGarbage();
    return 0;
}

void printTable(int start, int finish, char *name, stats resultStats)
{
    //check se la sezione è vuota
    int j = 0;
    int check = 0;
    for (j = start; j < finish; j++)
    {
        if (resultStats.frequencies[j] != 0)
        {
            check = 1;
            j = finish;
        }
    }
    if (check != 0)
    {
        printf("\n %s\n", name); //prints the name of the section
        int k = start;           //k = counter for the ascii characters
        int max = 0;             //length of the biggest number
        int dim = 0;             //dimension of the "current" number
        int search = 0;          //i,j,k were already used

        //search for the biggest number in frequencies from position START to FINISH
        for (search = start; search < finish; search++)
        {
            if (resultStats.frequencies[search] > max)
            {
                max = resultStats.frequencies[search];
            }
        }

        const int dimMax = getDigits(max); //save the digits of the biggest number

        int i = 0;

        //CHARACTERS
        do
        {
            if (resultStats.frequencies[k] != 0)
            {
                dim = getDigits(resultStats.frequencies[k]); //digits of the current number

                //first space
                printf(" ");

                if (dimMax == 2)
                {
                    if (dim == 2)
                        printf("%c", k);
                    if (dim == 1)
                    {
                        printf("%c", k);
                        printf(" ");
                    }
                }

                else if (dimMax == 3)
                {
                    if (dim == 3)
                    {
                        printf("%c", k);
                    }
                    if (dim == 2)
                    {
                        printf("%c", k);
                        printf(" ");
                    }
                    if (dim == 1)
                    {
                        printf(" ");
                        printf("%c", k);
                        printf(" ");
                    }
                }

                else if (dim % 2 == 0) //even number of digits
                {
                    for (i = 0; i < (dimMax / 2) - 1; i++) //prints spaces to the left of the CHARACTER
                        printf(" ");
                    printf("%c", k);
                    for (i = dimMax / 2; i < dimMax; i++) //prints spaces to the right of the CHARACTER
                        printf(" ");
                }
                else //odd number of digits
                {
                    for (i = 0; i < (dimMax / 2) - 1; i++) //prints spaces to the left of the CHARACTER
                        printf(" ");
                    printf("%c", k);
                    for (i = (dimMax / 2); i < dimMax; i++) //prints spaces to the right of the CHARACTER
                        printf(" ");
                }

                //last space
                if ((dimMax - dim) % 2 == 0)
                    printf(" ");
                printf("|");
            }
            k++;
        } while (k < finish);

        printf("\n");

        // NUMERI
        k = start;
        dim = 0;
        do
        {

            if (resultStats.frequencies[k] != 0)
            {
                dim = getDigits(resultStats.frequencies[k]);

                //first space
                printf(" ");

                if (dimMax == 2)
                {
                    if (dim == 2)
                        printf("%c", k);
                    if (dim == 1)
                    {
                        printf("%c", k);
                        printf(" ");
                    }
                }

                else if (dimMax == 3)
                {
                    if (dim == 3)
                    {
                        printf("%c", k);
                    }
                    if (dim == 2)
                    {
                        printf("%c", k);
                        printf(" ");
                    }
                    if (dim == 1)
                    {
                        printf(" ");
                        printf("%c", k);
                        printf(" ");
                    }
                }

                else if ((dimMax - dim) % 2 == 0)
                {
                    for (i = 0; i < (dimMax - dim) / 2; i++)
                        printf(" ");
                    printf("%d", resultStats.frequencies[k]);
                    for (i = 0; i < (dimMax - dim) / 2; i++)
                        printf(" ");
                }
                else if ((dimMax - dim) % 2 != 0)
                {
                    for (i = 0; i < (dimMax - dim) / 2; i++)
                        printf(" ");
                    printf("%d", resultStats.frequencies[k]);
                    for (i = 0; i < (dimMax - dim) / 2 + 1; i++)
                        printf(" ");
                }

                //last space
                if ((dimMax - dim) % 2 == 0)
                    printf(" ");
                printf("|");
            }
            k++;
        } while (k < finish);

        printf("\n");

        printf("\n");
        j = 0;
        for (j = 0; j < dimMax * 6; j++)
        {
            //for(i = 0; i < 1; i++)
            printf(" ");
            //for(i = 0; i < 7; i++)
            printf("-"); // Prints a line of -
        }
        printf("\n");
    }
}

void print(stats resultStats)
{
    int j = 0;
    int check = 0;
    for (j = 0; j < 256; j++)
    {
        if (resultStats.frequencies[j] != 0)
        {
            check = 1;
            j = 256;
        }
    }
    if (check == 0)
    {
        printf("Empty file\n");
        exit(0);
    }

    printf("\nNumber of tabs: %d", resultStats.frequencies[9]);
    printf("\nNumber of new lines: %d", resultStats.frequencies[11]);
    printf("\nNumber of spaces: %d", resultStats.frequencies[32]);
    printf("\nNumber of delete: %d \n", resultStats.frequencies[127]);

    printTable(33, 39, "Punctuation", resultStats);
    printTable(40, 48, "Punctuation", resultStats);
    printTable(48, 58, "Numbers", resultStats);
    printTable(58, 65, "Operators", resultStats);
    printTable(65, 73, "Uppercase letters", resultStats);
    printTable(73, 82, "Uppercase letters", resultStats);
    printTable(82, 91, "Uppercase letters", resultStats);
    printTable(91, 97, "Symbols", resultStats);
    printTable(97, 105, "Lowercase letters", resultStats);
    printTable(105, 114, "Lowercase letters", resultStats);
    printTable(114, 123, "Lowercase letters", resultStats);
    printTable(123, 127, "Other characters", resultStats); //some are divided in multiple row for style sake
}