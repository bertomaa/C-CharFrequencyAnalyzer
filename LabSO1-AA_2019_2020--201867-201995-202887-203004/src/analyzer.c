/*
    This fils is the analyzer: the main process creates n subprocesses, that create m subsubprocesses that analyze the files.
*/
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
#include "forkHandler.h"
#include "reportConnector.h"

int checkArguments(int argc, const char *argv[])
{
    if (argc < 4)
    {
        printf("Wrong arguments, usage is: analyzer <n> <m> <file1> <file2> ... \n");
        int i;
        for (i = 0; i < argc; i++)
            printf("argv[%d]=%s.\n", i, argv[i]);
        return 1;
    }
    return 0;
}

int *distributeQuantity(int quantity, int toDistribute)
{
    int *ret;
    allocWrapper(toDistribute, sizeof(int), (void **)&ret);
    int perProcess = quantity / toDistribute;
    int i;
    int tot = perProcess * toDistribute;
    if (toDistribute > quantity)
    {
        for (i = 0; i < quantity; i++)
        {
            ret[i] = 1;
        }
        for (i = quantity; i < toDistribute; i++)
        {
            ret[i] = 0;
        }
        return ret;
    }
    for (i = 0; i < toDistribute; i++)
    {
        ret[i] = perProcess;
    }
    for (i = 0; i < toDistribute; i++)
    {
        if (tot != quantity)
        {
            ret[i]++;
            tot++;
        }
        else
            break;
    }
    return ret;
}

int *createPipes(int size)
{
    int *pipes;
    allocWrapper(size * 2, sizeof(int), (void **)&pipes);
    int i;
    for (i = 0; i < size; i++)
    {
        if(pipe(pipes + (i * 2)) == -1)
            printf("TOMAREVACCA\n");
    }
    return pipes;
}

int writePipe(int *pipes, int index, const char *toWrite)
{
    //printf("writing to pipe %d chars\n", (int)strlen(toWrite));
    write(pipes[getPipeIndex(index, WRITE)], toWrite, (int)strlen(toWrite) + 1);
    return 0; //error code
}

int readPipe(int *pipes, int index, char *buf, int toRead)
{
    //readWrapper(pipes, index, buf, toRead)
    read(pipes[getPipeIndex(index, READ)], buf, MAX_PIPE_CHARACTERS * toRead);
    //printf("Read from pipe %d chars\n", (int)strlen(buf));
    return 0; //error code
}

int readPipeAndAppend(int *pipes, int index, char *buf, int toRead)
{
    int len = strlen(buf);
    read(pipes[getPipeIndex(index, READ)], buf + len, MAX_PIPE_CHARACTERS * toRead);
    //printf("Read from pipe and appended %d chars\n", (int)strlen(buf) - len);
    return 0; //error code
}

stats analyzeText(int fd, int offset, int bytesToRead, int id)
{
    int i;
    stats ret; 
    initStats(&ret, id);
    char *buffer;
    allocWrapper(bytesToRead + 1, sizeof(char), (void **)&buffer);
    lseek(fd, offset, SEEK_SET);
    ssize_t bytesRead = read(fd, buffer, bytesToRead);
    //printf("analyzing file: %d with offset %d reading %d\n", id, offset, bytesToRead);
    for (i = 0; i < bytesRead; i++)
    {
        if (buffer[i] >= 0)
            ret.frequencies[(int)buffer[i]]++;
    }
    //removeFromGCAndFree(buffer);
    return ret;
}

//process q, reads his part (index) of file
int q(int mIndex, int filesCount, int m, char *const *files, int writePipe, int fileIndex)
{
    char *endptr;
    initGC();
    // printf("q: %d, con filescount: %d, fileindex: %d, file: %s\n", mIndex, filesCount, fileIndex, files[fileIndex]);
    int fd, fileOffset, fileLength, i;
    stats *statsToSend, tmp;
    allocWrapper(filesCount, sizeof(stats), (void **)&statsToSend);
    for (i = 0; i < filesCount; i++)
    {
        initStats(&statsToSend[i], i + fileIndex);
    }
    for (i = fileIndex; i < fileIndex + filesCount; i++)
    {
        // printf("fileindex:%d trying to read file #%d: %s\n",fileIndex, i, files[i]);
        openWrapper(files[i], &fd);
        char *buffer;
        char *buffer2;
        allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&buffer);
        allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&buffer2);
        addDoubleQuotes(buffer2, files[i]);
        sprintf(buffer, "wc -c %s", buffer2);
        char *cmdOutput = getCommandOutput(buffer, MAX_PATH_LEN + 40);
        buffer[0] = 0;
        splitString(buffer, &cmdOutput, ' ');
        fileLength = strtol(buffer, &endptr, 10);
        //removeFromGCAndFree(buffer);
        //removeFromGCAndFree(buffer2);
        fileOffset = (fileLength / m) * mIndex;
        if (mIndex == m - 1)
            tmp = analyzeText(fd, fileOffset, fileLength - fileOffset, i);
        else
            tmp = analyzeText(fd, fileOffset, fileLength / m, i);
        sumStats(&statsToSend[i - fileIndex], &tmp);
        close(fd);
    }
    char *encoded = encodeMultiple(statsToSend, filesCount);
    //printf("sending from q%d: %s\n", mIndex, encoded);
    write(writePipe, encoded, strlen(encoded) + 1);
    close(writePipe);
    collectGarbage();
    // printf("q finished\n");
    return 0;
}

//process p, generates m children processes q and assigns them the sections of file to analyze
int p(int m, int filesCount, char *const *files, int writePipe, int fileIndex)
{
    initGC();
    // printf("p: %d, con filescount: %d, fileindex: %d, file: %s\n", m, filesCount, fileIndex, files[fileIndex]);
    if (filesCount == 0)
    {
        close(writePipe);
        return 0;
    }
    int *pipes = createPipes(m);
    int i;
    for (i = 0; i < m; i++)
    {
        if (createChild() == 0)
        {
            q(i, filesCount, m, files, pipes[getPipeIndex(i, WRITE)], fileIndex);
            return 0;
        }
    }
    while (wait(NULL) != -1)
        ;
    char *str;
    allocWrapper(MAX_PIPE_CHARACTERS * filesCount, sizeof(stats), (void **)&str);
    stats *resultStats;
    allocWrapper(filesCount, sizeof(stats), (void **)&resultStats);
    for (i = 0; i < filesCount; i++)
    {
        initStats(&resultStats[i], i + fileIndex);
    }
    for (i = 0; i < m; i++)
    {
        readPipe(pipes, i, str, filesCount);
        //printf("p ha ricevuto: %s\n", stat);
        decodeMultiple(str, resultStats);
    }
    //removeFromGCAndFree(str);
    char *resultString = encodeMultiple(resultStats, filesCount);
    //removeFromGCAndFree(resultStats);
    //printf("mandato al main:%s\n", resultString);
    write(writePipe, resultString, strlen(resultString) + 1); // stessa munnezz
    close(writePipe);
    collectGarbage();
    // printf("p finished\n");
    return 0; //manco lo scrivo più
}

char *getDataFromPs(const config conf, int *pipesToP)
{
    int i;
    printf("Analyzing...\n");
    while (wait(NULL) != -1)
        ; //father waits all children
    printf("Analyzed!\n");
    char *stat;
    allocWrapper(MAX_PIPE_CHARACTERS * conf.filesCount, sizeof(char), (void **)&stat);
    for (i = 0; i < conf.n && i < conf.filesCount; i++)
    {
        readPipeAndAppend(pipesToP, i, stat, conf.filesCount);
    }
    //printf("dal main analizer ricevo: \n%s\n", stat);
    return stat;
}

//data sent as input: n,m, namefile, namefile2...
int main(int argc, const char *argv[])
{
    initGC();
    initProcess();
    int i;
    char *endptr;
    config *conf;
    allocWrapper(1, sizeof(config), (void **)&conf);
    initConfig(conf);
    // for (i = 0; i < 50; i++)
    // {
    //     printf("\n");
    // }
    // for (i = 0; i < argc; i++)
    // {
    //     printf("%s.\n", argv[i]);
    // }

    if (checkArguments(argc, argv) != 0)
        fatalErrorHandler("Wrong arguments, exit.", 1);
    conf->n = strtol(argv[1], &endptr, 10);
    conf->m = strtol(argv[2], &endptr, 10);
    //add paths to conf

    for (i = PRE_FILES_ARGS; i < argc; i++)
    {
        addFileToConfig(conf, argv[i]);
    }
    conf->filesCount = argc - 3;

    conf = checkDirectories(conf);

    // printf("filesCount:%d\n", conf->filesCount);
    // printFiles(conf);

    int *assignedFiles = distributeQuantity(conf->filesCount, conf->n);

    int *pipesToP = createPipes(conf->n);

    //creates subprocess p
    printf("starting with n: %d, m: %d\n", conf->n, conf->m);
    int offset = 0;
    for (i = 0; i < conf->n; i++)
    {
        if (createChild() == 0)
        {
            int ret = p(conf->m, assignedFiles[i], conf->files, pipesToP[getPipeIndex(i, WRITE)], offset);
            return ret;
        }
        offset += assignedFiles[i];
    }
    //fatalErrorHandler("prova fatal error main", 0);
    //father
    char *sendToReport = getDataFromPs(*conf, pipesToP);
    //int isReportConnected =
    launchReportConnector(conf, sendToReport);
    collectGarbage();
    printf("Analyzer exited!\n");
    exit(0);
}