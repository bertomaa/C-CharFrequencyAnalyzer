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

char *pipeToReportName = "/tmp/analyzerToReport.pipe";

void print(int start, int finish, char* type, stats resultStats)
{
    int cont = 0; //counts all the characters written to have adotted line of the same length
    int i = 0;
    int j = 0;

    /*printf("%s\n", name); //prints the name of the section
    int i = start;
    do
    {
        printf("%c\t", i); //prints the ascii character
        i++;
    } while (i < finish);

    printf("\n");

    for (int i = start; i < finish; i++)
    {
        printf("%d\t", arrayFrequencies[i]); //prints its frequency
    }
    printf("\n");*****/

    int k = start;
    printf("\n%s\n", type); //prints the name of the section
    do
    {
        if(resultStats.frequencies[k] != 0)
        {
            printf("  "); //2 spaces before the character
            cont = cont + 2;

            printf("%c", k); //prints the ascii character
            cont++;

            printf("  "); //2 spaces after the character
            cont = cont + 2;

            printf("|");
            cont++;
        }
        k++;

    } while (k < finish);

    printf("\n");

    for (i = start; i < finish; i++)
    {
        if (resultStats.frequencies[i] < 10 && resultStats.frequencies[i] > 0)
        {
            printf("  ");
        }
        if (resultStats.frequencies[i] < 100 && resultStats.frequencies[i] >= 10)
        {
            printf("  ");
        }
        if (resultStats.frequencies[i] < 1000 && resultStats.frequencies[i] >= 100)
        {
            printf(" ");
        }
        printf("%d", resultStats.frequencies[i]); //prints its frequency
        if (resultStats.frequencies[i] < 10 && resultStats.frequencies[i] > 0)
        {
            printf("  ");
        }
        if (resultStats.frequencies[i] < 100 && resultStats.frequencies[i] >= 10)
        {
            printf(" ");
        }
        if (resultStats.frequencies[i] < 1000 && resultStats.frequencies[i] >= 100)
        {
            printf(" ");
        }
        printf("|");
    }
    printf("\n");
    for (j = 0; j < cont; j++)
    {
        printf("-"); // Prints a line of -
    }
}

void printTable(stats resultStats)
{
    print(32, 48, "Punctuation", resultStats);
    print(48, 58, "Numbers", resultStats);
    print(58, 65, "Operators", resultStats);
    print(65, 78, "Uppercase letters", resultStats);
    print(78, 91, "Uppercase letters", resultStats); //letters are divided in two rows for style's sake
    print(91, 97, "Symbols", resultStats);
    print(97, 110, "Lowercase letters", resultStats);
    print(110, 123, "Lowercase letters", resultStats);
    print(123, 128, "Other characters", resultStats); //letters are divided in two rows for style's sake
}

char *getLine()
{
    int size = MAX_COMMAND_LEN;
    int i = 0;
    char *line;
    int error = allocWrapper(size, sizeof(char), (void **)&line);
    //TODO: error
    char c;
    while (c != '\n')
    {
        scanf("%c", &c);
        if (i + 1 >= size)
        {
            size += MAX_COMMAND_LEN;
            error = reallocWrapper((void **)&line, size);
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
    printf("report <file1> <file2> ...          - Shows the report of the specified files.\n");
    printf("report <directory1> <directory2>... - Shows the report of the file in the specified directoy.\n");
    printf("report <directory1> <directory2>... - Shows the report of the file in the specified directoy.\n");
    printf("config                              - Show the current configuration, with the added files \n");
}

void showCommandNotFoundError(const char *arguments)
{
    if (arguments[0] != '\n')
        printf("Command %s is not valid, see usage with help\n", arguments);
}

void showConfig(config *conf)
{
    printf("n = %d, m = %d\n", conf->n, conf->m);
    if (conf->n > 0 && conf->m > 0)
    {
        printf("Analyzer run with n = %d, m = %d\n", conf->n, conf->m);
        printFiles(conf);
    }
    else
    {
        printf("n and m not valid, see usage with help\n");
        printFiles(conf);
    }
}

void report(char *arguments, config *conf)
{
    char *endptr;
    char *pn = strtok(arguments, " ");
    char *pm = strtok(NULL, " ");
    if (pn != NULL && pm != NULL)
    {
        int n1 = strtol(pn, &endptr, 10);
        int m1 = strtol(pm, &endptr, 10);
        if (n1 > 0 && m1 > 0)
        {
            conf->n = n1;
            conf->m = m1;
        }
        else
        {
            printf("Command \"set %s\" is not valid, see usage with help\n", arguments);
        }
    }
    else
    {
        showHelp();
    }
}

int getAction(char *command, config *conf)
{
    char *token = strtok(command, " ");
    if(token == NULL)
        return 1;

    if (strcmp(token, "report") == 0)
    {
        //CHECK SE TOKEN VA BENE COME ARGOMENTO PASSATO: L'HO MESSO A CASO AFFINCHE' COMPILASSE CORRETTAMENTE
        report(token, conf);
    }
    else if (strcmp(token, "help") == 0)
    {
        showHelp();
    }
    else if (strcmp(token, "config") == 0 || strcmp(token, "c") == 0)
    {
        showConfig(conf); //works
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
    int error = allocWrapper(bufferLen, sizeof(char), (void **)&buffer); //TODO: usa il wrapper,
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

int main(int argc, char *argv[])
{
    initGC();
    config conf;
    initConfig(&conf);
    int filesCount;
    char *e;
    printf("Waiting for Analyzer...\n");
    int fd = open(pipeToReportName, O_RDONLY);
    printf("connected!\n");
    while(fd == -1)
    {
        printf("Impossible to open pipe, did you run Analyzer first? Press anything to retry or q to quit\n");
        //int c = getchar();
        char c;
        scanf("%c", &c);
        if(c == 'q')
            exit(0);
        fd = open(pipeToReportName, O_RDONLY);
    };
    char *buffer = readStringFromPipe(40, fd);
    filesCount = strtol(buffer, &e, 10);
    int i;
    printf("%s\n%p\n%d\n", buffer, buffer, filesCount);
    for (i = 0; i < filesCount; i++)
    {
        buffer = readStringFromPipe(MAX_PATH_LEN, fd);
        printf("%s\n", buffer);
        addFileToConfig(&conf, buffer);
    }
    printf("a\n");
    char *statString;
    int error = allocWrapper(MAX_PIPE_CHARACTERS * conf.filesCount, sizeof(char), (void **)&statString); //TODO: usa il wrapper,
    printf("a\n");
    read(fd, statString, MAX_PIPE_CHARACTERS * conf.filesCount);
    stats *resultStats;
    printf("a\n");
    error = allocWrapper(conf.filesCount, sizeof(stats), (void **)&resultStats); //TODO:trova una stima migliore

    printf("a\n");
    for (i = 0; i < conf.filesCount; i++)
    {
        initStats(&(resultStats[i]), i);
    }
    printf("a\n");
    //printf("\n\nstat string: %s\n\n", statString);
    decodeMultiple(statString, resultStats); //TODO:check error
    printf("a\n");

    /*
    int action;
    do
    {
        printf("-> ");
        char *command = getLine();
        action = getAction(command, &conf);
    } while (action);
    */
   
    for (i = 0; i < conf.filesCount; i++)
    {
        //TODO: mettere in british
        printf("In file %s was analyzed:\n", conf.files[i]);
        printTable(*resultStats);
        printStats(resultStats[i]);
    }
    printf("FINE!\n");
    return 0;
}

