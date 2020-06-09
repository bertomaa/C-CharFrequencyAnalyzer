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
#include "print.h"

char *analyzerToReportPipe = "/tmp/analyzerToReport.pipe";
char *mainToReportPipe = "/tmp/mainToReport.pipe";
int FDanalyzer;

void readAnalyzer(confAndStats *conf);

void showHelp()
{
    printf("\nReport allowed actions:\n\n");
    printf("   help                                 - Show this help message\n");
    printf("   show:                                - Shows the report of the specified paths. Options must be passed first.\n       [-f --frequencies] \n       [-g --group] \n       [-a --ascendend] \n       [-d --descendent] \n       <file or direcotry> <file or direcotry> ...  \n\n");
    printf("   showall:                             - Shows the report of all files analyzed. Options must be passed first.\n       [-f --frequencies] \n       [-g --group] \n       [-a --ascendend] \n       [-d --descendent] \n       \n\n");
    printf("   remove <file or directory 1> ...     - Removes files (or directories) from the list\n");
    printf("   read                                 - Read data from analyzer, not necessary if run by main\n");
    printf("   files                                - shows all files currently ready to be shown\n");
    printf("   q / quit / exit                      - Close the program\n");
}

void show(char *files, confAndStats *cs)
{
    while (files[0] == ' ')
        files = files + 1;
    int i;
    char *buffer;
    allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&buffer);
    buffer = splitStringWithQuotes(buffer, &files, ' ');
    int every = 1;
    int frequencies = 0;
    int order = 0;
    int hasArgs = 0;
    int printConf[7] = {1, 0, 0, 0, 0, 0, 0}; //0 true: una tabella con tutto, dal 1 in poi true se voglio vederli
    stats tmpStat;
    initStats(&tmpStat, -1);
    while (buffer != NULL)
    {
        int index = getFileIndexInConfig(cs->conf, buffer);
        if (index != -1 && every == 1)
        {
            printf("File %s:\n", cs->conf->files[index]);
            print(cs->stats[index], frequencies, order, printConf);
            buffer = splitStringWithQuotes(buffer, &files, ' ');
            continue;
        }
        //match = 0;
        for (i = 0; i < cs->conf->filesCount; i++)
        {
            if (strcmp("--group", buffer) == 0 || strcmp("-g", buffer) == 0)
            {
                every = 0;
                break;
            }
            if (strcmp("--frequencies", buffer) == 0 || strcmp("-f", buffer) == 0)
            {
                frequencies = 1;
                break;
            }
            if (strcmp("--ascending", buffer) == 0 || strcmp("-a", buffer) == 0)
            {
                order = 1;
                break;
            }
            if (strcmp("--descending", buffer) == 0 || strcmp("-d", buffer) == 0)
            {
                order = -1;
                break;
            }
            if (strcmp("--numbers", buffer) == 0)
            {
                if (hasArgs == 0)
                    printConf[0] = 0;
                hasArgs = 1;
                printConf[1] = 1;
                break;
            }
            if (strcmp("--upper-case", buffer) == 0)
            {
                if (hasArgs == 0)
                    printConf[0] = 0;
                hasArgs = 1;
                printConf[2] = 1;
                break;
            }
            if (strcmp("--lower-case", buffer) == 0)
            {
                if (hasArgs == 0)
                    printConf[0] = 0;
                hasArgs = 1;
                printConf[3] = 1;
                break;
            }
            if (strcmp("--punctuation", buffer) == 0)
            {
                if (hasArgs == 0)
                    printConf[0] = 0;
                hasArgs = 1;
                printConf[4] = 1;
                break;
            }
            if (strcmp("--symbols", buffer) == 0)
            {
                if (hasArgs == 0)
                    printConf[0] = 0;
                hasArgs = 1;
                printConf[5] = 1;
                break;
            }
            if (strcmp("--others", buffer) == 0)
            {
                if (hasArgs == 0)
                    printConf[0] = 0;
                hasArgs = 1;
                printConf[6] = 1;
                break;
            }
            if (strcmp("--not-printable", buffer) == 0)
            {
                if (hasArgs == 0)
                    printConf[0] = 0;
                hasArgs = 1;
                printConf[7] = 1;
                break;
            }
            if (strcmp("--all", buffer) == 0)
            {
                hasArgs = 1;
                printConf[0] = 1;
                break;
            }
            if (does1StringMatch2(buffer, cs->conf->files[i]))
            {
                if (every)
                {
                    printf("File %s:\n", cs->conf->files[i]);
                    print(cs->stats[i], frequencies, order, printConf);
                }
                else
                {
                    sumStats(&tmpStat, &(cs->stats[i]));
                }
            }
        }

        buffer = splitStringWithQuotes(buffer, &files, ' ');
    }
    if (!every)
    {
        print(tmpStat, frequencies, order, printConf);
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
    decodeMultiple(statString, &(cs->stats[lastStatsIndex]));
    //removeFromGCAndFree(statString);
}

int main(int argc, char *argv[])
{
    initGC();
    initProcess();
    mkfifo(analyzerToReportPipe, 0666);
    confAndStats cs;
    allocWrapper(1, sizeof(config), (void **)&(cs.conf));
    allocWrapper(INITIAL_CONFIG_SIZE, sizeof(stats), (void **)&(cs.stats));
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
        do
        {
            printf("->");
            command = getLine();
            //printf("report cmd: %s\n", command);
            goOn = processCommand(command, &cs);
            //removeFromGCAndFree(command);
        } while (goOn);
    }

    printf("FINE REPORT!\n");
    collectGarbage();
    return 0;
}
