#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "wrapper.h"
#include "stats.h"
#include "config.h"
#include "commons.h"

char *pipeToReportName = "/tmp/analyzerToReport.pipe";

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
    // printf("\n\nstat string: %s\n\n", statString);
    decodeMultiple(statString, resultStats); //TODO:check error
    printf("a\n");
    for (i = 0; i < conf.filesCount; i++)
    {
        //TODO: mettere in british
        printf("In file %s was analyzed:\n", conf.files[i]);

        printStats(resultStats[i]);
    }
    printf("FINE!\n");
    return 0;
}