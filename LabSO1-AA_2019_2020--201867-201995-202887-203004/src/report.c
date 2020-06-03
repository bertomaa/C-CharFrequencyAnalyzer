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

char *analyzerToReportPipe = "/tmp/analyzerToReport.pipe";
char *mainToReportPipe = "/tmp/mainToReport.pipe";
int FDanalyzer;

void readAnalyzer(confAndStats *conf);
void printTable(int start, int finish, char *name, stats resultStats)
{
    printf("\n %s\n", name); //prints the name of the section
    int k = start;           //k = counter for the ascii characters
    int max = 0;             //length of the biggest number
    int dim = 0;             //dimension of the "current" number
    int search = 0;          //i,j,k were already used
    for (search = 0; search < 256; search++)
    {
        if (resultStats.frequencies[search] > max)
        {
            max = resultStats.frequencies[search]; //search for the biggest number in frequencies
        }
    }
    //printf("\n %d \n", max);
    const int dimMax = getDigits(max); //save the digits of the biggest number
    // int dim = getDigits(arrayFrequencies[k]);
    int i = 0;

    //CHARACTERS
    do
    {

        if (resultStats.frequencies[k] != 0)
        {
            dim = getDigits(resultStats.frequencies[k]);
            //dim = getDigits(resultStats.frequencies[k]); //digits of the current number
            if (dim % 2 == 0) //even number of digits
            {
                for (i = 0; i < (dimMax / 2) - 1; i++) //prints spaces to the left of the CHARACTER
                    printf(" ");
                printf("%c", k);
                for (i = dimMax / 2; i < dimMax; i++) //prints spaces to the right of the CHARACTER
                    printf(" ");
            }
            else //odd number of digits
            {
                for (i = 0; i < (dimMax / 2); i++) //prints spaces to the left of the CHARACTER
                    printf(" ");
                printf("%c", k);
                for (i = (dimMax / 2) + 1; i < dimMax; i++) //prints spaces to the right of the CHARACTER
                    printf(" ");
            }
            printf("|");
        }
        k++;
    } while (k < finish);

    printf("\n");

    // NUMERI
    k = start;
    dim = 0;
    int d = 0;
    do
    {

        if (resultStats.frequencies[k] != 0)
        {
            dim = getDigits(resultStats.frequencies[k]);
            //d = dimMax - dim;
            if ((dimMax) % 2 == 0)
            {
                if (((dimMax - dim) / 2) - 1 == 0)
                {
                    printf(" ");
                }
                else
                {
                    for (i = 0; i < ((dimMax - dim) / 2) - 1; i++)
                        printf(" ");
                }
                printf("%d", resultStats.frequencies[k]);
                for (i = (((dimMax - dim) / 2) + dim); i < dimMax; i++)
                    printf(" ");
            }
            else
            {
                for (i = 0; i < (dimMax - dim) / 2; i++)
                    printf(" ");
                printf("%d", resultStats.frequencies[k]);
                for (i = (((dimMax - dim) / 2) + dim); i < dimMax; i++)
                    printf(" ");
            }
            printf("|");
        }
        k++;
    } while (k < finish);

    printf("\n");

    printf("\n");
    int j = 0;
    for (j = 0; j < dimMax * 6; j++)
    {
        //for(i = 0; i < 1; i++)
        printf(" ");
        //for(i = 0; i < 7; i++)
        printf("-"); // Prints a line of -
    }
    printf("\n");
}

void print(stats resultStats)
{
    printTable(32, 39, "Punctuation", resultStats);
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
    printTable(123, 128, "Other characters", resultStats); //some are divided in multiple row for style sake
}

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
            reallocWrapper((void **)&line, size);
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
            reallocWrapper((void **)&(cs->stats), cs->conf->dim + INITIAL_CONFIG_SIZE); //TODO: error
        }
    }
    printf("b\n");
    char *statString;
    allocWrapper(MAX_PIPE_CHARACTERS * cs->conf->filesCount, sizeof(char), (void **)&statString); //TODO: usa il wrapper,
    printf("b\n");
    read(FDanalyzer, statString, MAX_PIPE_CHARACTERS * cs->conf->filesCount);
    close(FDanalyzer);
    printf("b\n");
    for (i = lastStatsIndex; i < cs->conf->filesCount; i++)
    {
        printf("i %d, filesCount %d\n", i, cs->conf->filesCount);
        initStats(&(cs->stats[i]), i);
    }
    printf("b\n");
    //printf("\n\nstat string: %s\n\n", statString);
    decodeMultiple(statString, &(cs->stats[lastStatsIndex])); //TODO:check error
    printf("b\n");
    for (i = 0; i < cs->conf->filesCount; i++)
    {
        printf("In file %s was analyzed:\n", cs->conf->files[i]);
        //printTable(*resultStats);
        printStats(cs->stats[i]);
    }
    printf("b\n");
}

int main(int argc, char *argv[])
{
    initGC();
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

// void print(int start, int finish, char *type, stats resultStats)
// {
//     int cont = 0; //counts all the characters written to have adotted line of the same length
//     int i = 0;
//     int j = 0;

//     /*
//     printf("%s\n", name); //prints the name of the section
//     int i = start;
//     do
//     {
//         printf("%c\t", i); //prints the ascii character
//         i++;
//     } while (i < finish);

//     printf("\n");

//     for (int i = start; i < finish; i++)
//     {
//         printf("%d\t", arrayFrequencies[i]); //prints its frequency
//     }
//     printf("\n");*****/

//     int k = start;
//     int dim = 0;
//     printf("\n%s\n", type); //prints the name of the section
//     do
//     {
//         if (resultStats.frequencies[k] != 0)
//         {
//             dim = getDigits(resultStats.frequencies[k]);
//             if (dim % 2 == 0)
//             {
//                 for (i = 0; i < dim / 2 - 1; i++)
//                     printf(" ");
//                 printf("%c", k);
//                 for (i = dim / 2; i < dim; i++)
//                     printf(" ");
//             }
//             else
//             {
//                 for (i = 0; i < dim / 2; i++)
//                     printf(" ");
//                 printf("%c", k);
//                 for (i = dim / 2 + 1; i < dim; i++)
//                     printf(" ");
//             }
//             cont += dim;
//             /*
//             printf("  "); //2 spaces before the character
//             cont = cont + 2;

//             printf("%c", k); //prints the ascii character
//             cont++;

//             printf("  "); //2 spaces after the character
//             cont = cont + 2;

//             printf("|");
//             cont++;
//             */
//         }
//         k++;

//     } while (k < finish);

//     printf("\n");

//     for (i = start; i < finish; i++)
//     {
//         if (resultStats.frequencies[i] < 10 && resultStats.frequencies[i] > 0)
//         {
//             printf("  ");
//         }
//         if (resultStats.frequencies[i] < 100 && resultStats.frequencies[i] >= 10)
//         {
//             printf("  ");
//         }
//         if (resultStats.frequencies[i] < 1000 && resultStats.frequencies[i] >= 100)
//         {
//             printf(" ");
//         }
//         if (resultStats.frequencies[i] > 0)
//         {
//             printf("%d", resultStats.frequencies[i]); //prints its frequency
//         }
//         if (resultStats.frequencies[i] < 10 && resultStats.frequencies[i] > 0)
//         {
//             printf("  ");
//         }
//         if (resultStats.frequencies[i] < 100 && resultStats.frequencies[i] >= 10)
//         {
//             printf(" ");
//         }
//         if (resultStats.frequencies[i] < 1000 && resultStats.frequencies[i] >= 100)
//         {
//             printf(" ");
//         }
//         printf("|");
//     }
//     printf("\n");
//     for (j = 0; j < cont; j++)
//     {
//         printf("-"); // Prints a line of -
//     }
// }

// void printTable(stats resultStats)
// {
//     print(32, 48, "Punctuation", resultStats);
//     print(48, 58, "Numbers", resultStats);
//     print(58, 65, "Operators", resultStats);
//     print(65, 78, "Uppercase letters", resultStats);
//     print(78, 91, "Uppercase letters", resultStats); //letters are divided in two rows for style's sake
//     print(91, 97, "Symbols", resultStats);
//     print(97, 110, "Lowercase letters", resultStats);
//     print(110, 123, "Lowercase letters", resultStats);
//     print(123, 128, "Other characters", resultStats); //letters are divided in two rows for style's sake
// }