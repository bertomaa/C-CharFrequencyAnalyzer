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

#define MAX_PATH_CHARACTERS 1024

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
    int error = allocWrapper(toDistribute, sizeof(int), (void **)&ret);
    //TODO: check error
    printf("toDistr:%d\n", toDistribute);
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
            ret[i]++;
        else
            break;
    }
    return ret;
}

int *createPipes(int size)
{
    //TODO: checks and free
    int *pipes = (int *)calloc(size * 2, sizeof(int));
    int i;
    for (i = 0; i < size; i++)
    {
        pipe(pipes + (i * 2));
    }
    return pipes;
}

int getPipeIndex(int index, int type)
{
    return index * 2 + type;
}

int writePipe(int *pipes, int index, const char *toWrite)
{
    //TODO: checks and free
    //printf("writing to pipe %d chars\n", (int)strlen(toWrite));
    write(pipes[getPipeIndex(index, WRITE)], toWrite, (int)strlen(toWrite) + 1);
    return 0; //error code
}

int readPipe(int *pipes, int index, char *buf, int toRead)
{
    read(pipes[getPipeIndex(index, READ)], buf, MAX_PIPE_CHARACTERS * toRead);
    printf("Read from pipe %d chars\n", (int)strlen(buf));
    return 0; //error code
}

int readPipeAndAppend(int *pipes, int index, char *buf, int toRead)
{
    int len = strlen(buf);
    read(pipes[getPipeIndex(index, READ)], buf + len, MAX_PIPE_CHARACTERS * toRead);
    printf("Read from pipe and appended %d chars\n", (int)strlen(buf) - len);
    return 0; //error code
}

stats analyzeText(int fd, int offset, int bytesToRead, int id)
{
    int i;
    stats ret; //TODO: dovrebbe essere allocato dinamicamente se viene ritornato?
    initStats(&ret, id);
    char *buffer;
    int error = allocWrapper(bytesToRead + 1, sizeof(char), (void **)&buffer);
    //TODO:check error

    lseek(fd, offset, SEEK_SET);
    read(fd, buffer, bytesToRead);
    printf("analyzing file: %d with offset %d reading %d\n", id, offset, bytesToRead);

    for (i = 0; buffer[i] != '\0'; i++)
    {
        //fin qua ce sta
        ret.frequencies[buffer[i]]++;
    }

    //free(buffer);
    return ret;
}

//process q, reads his part (index) of file
int q(int mIndex, int filesCount, int m, const char *files[], int writePipe, int fileIndex)
{
    printf("q: %d, con filescount: %d, fileindex: %d\n", mIndex, filesCount, fileIndex);
    int fd, fileOffset, fileLength, i, error;
    stats *statsToSend, tmp;
    error = allocWrapper(filesCount, sizeof(stats), (void **)&statsToSend);
    //TODO: check error
    for (i = 0; i < filesCount; i++)
    {
        initStats(&statsToSend[i], i + fileIndex);
    }
    for (i = 0; i < filesCount; i++)
    {
        error = openWrapper(files[i], &fd);
        if (error)
        {
            continue;
        }
        fileLength = lseek(fd, 0, SEEK_END);
        fileOffset = (fileLength / m) * mIndex; //TODO: gestire resto divisione, insomma che lo steso carattere non venga letto 2 volte o 0
        if (mIndex == m - 1)
            tmp = analyzeText(fd, fileOffset, fileLength - fileOffset, fileIndex + i);
        else
            tmp = analyzeText(fd, fileOffset, fileLength / m, fileIndex + i);
        sumStats(&statsToSend[i], &tmp);
        close(fd);
    }
    char *encoded = encodeMultiple(statsToSend, filesCount);
    //printf("sending from q%d: %s\n", mIndex, encoded);
    //char *encoded = encodeMultiple(statsToSend, filesCount);                   //TODO: come sempre controllare che si sia chiusa munnezz
    write(writePipe, encoded, strlen(encoded) + 1); //controlla sta cacata
    close(writePipe);
    //TODO: free varie e close
    return 0;
}

//process p, generates m children processes q and assigns them the sections of file to analyze
int p(int m, int filesCount, const char *files[], int writePipe, int fileIndex)
{
    printf("p: %d, con filescount: %d, fileindex: %d\n", m, filesCount, fileIndex);

    if(filesCount == 0)
    {
        close(writePipe);
        return 0;
    }

    int *pipes = createPipes(m);
    int i, pid, j;
    pid_t *pids = (int *)malloc(m * sizeof(int));
    for (i = 0; i < m; i++)
    {
        pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "Error in fork in p, exit\n");
            exit(2);
        }
        else if (pid == 0) //children
        {
            q(i, filesCount, m, files, pipes[getPipeIndex(i, WRITE)], fileIndex);
            return 0;
        }
        else //father
            pids[i] = pid;
    }
    if (pid > 0)
    {
        while (wait(NULL) != -1)
            ; //father waits all children
        char *str;
        int error = allocWrapper(MAX_PIPE_CHARACTERS * filesCount, sizeof(stats), (void **)&str);
        if (error)
        {
            fprintf(stderr, "Error, impossible to allocate buffer memory.");
            exit(2);
        }
        stats *resultStats;
        error = allocWrapper(filesCount, sizeof(stats), (void **)&resultStats);
        if (error)
        {
            fprintf(stderr, "Error, impossible to allocate stats memory.");
            exit(2);
        }
        for (i = 0; i < filesCount; i++)
        {
            initStats(&resultStats[i], i + fileIndex);
        }
        int res;
        for (i = 0; i < m; i++)
        {
            readPipe(pipes, i, str, filesCount); //TODO: indovina? controlla  che la munnezz abbia ritornato e non sia andata al lago
            //printf("p ha ricevuto: %s\n", stat);
            /*
            //testing
            stats tmp;
            initStats(&tmp, 7);
            decode(stat, &tmp, (3));
            //testing
            */
            int decodeError = decodeMultiple(str, resultStats); //TODO: check
            // for (j = 0; j < filesCount; j++)
            // {
            //     printf("(p)In totale è stato letto nel file %d da q %d:\n", j, i);
            //     printStats(resultStats[j]);
            // }
            //int *p = malloc(sizeof(int));
            //*p = 3;
            //int decodeError = decode(str, resultStats, p); //TODO: check
            if (decodeError)

            {
                printf("errore decode p\n");
                return 1; //TODO: esegui free ecc..
            }
        }
        char *resultString = encodeMultiple(resultStats, filesCount);
        //printf("mandato al main:%s\n", resultString);
        write(writePipe, resultString, strlen(resultString) + 1); // stessa munnezz
        close(writePipe);
    }
    return 0; //manco lo scrivo più
}

//data sent as input: n,m, namefile, namefile2...
int main(int argc, const char *argv[])
{
    int i;
    char *endptr;
    // for (i = 0; i < 50; i++)
    // {
    //     printf("\n");
    // }
    for (i = 0; i < argc; i++)
    {
        printf("%s.\n", argv[i]);
    }
    if (checkArguments(argc, argv) != 0)
        exit(1);

    int n = strtol(argv[1], &endptr, 10);
    int m = strtol(argv[2], &endptr, 10);
    int filesCount = argc - 3;

    //TODO: if a filename is a folder then find the files
    //checkDirectories(argv + 3, filesCount, 0);

    int *assignedFiles = distributeQuantity(filesCount, n);

    int *pipesToP = createPipes(n);
    pid_t *pids = (pid_t *)calloc(n, sizeof(int));
    int pid;

    //creates subprocess p
    printf("starting with n: %d, m: %d\n", n, m);
    int offset = 0;
    for (i = 0; i < n; i++)
    {
        pid = fork();
        if (pid < 0)
        {
            printf("Error in fork, exit");
            return (2);
        }
        else if (pid == 0) //children
        {
            int ret = p(m, assignedFiles[i], argv + offset + 3, pipesToP[getPipeIndex(i, WRITE)], offset);
            return ret;
        }
        else
        { //father
            pids[i] = pid;
        }
        offset += assignedFiles[i];
    }
    //father
    if (pid > 0)
    {
        printf("\n\nmain: waiting for all children\n\n");
        while (wait(NULL) != -1)
            ; //father waits all children
        printf("\n\nmain: waited for all children\n\n");
        char *stat;
        int error = allocWrapper(MAX_PIPE_CHARACTERS * filesCount, sizeof(stats), (void **)&stat); //TODO:trova una stima migliore
        stats *resultStats;
        for (i = 0; i < filesCount; i++)
        {
            initStats(&resultStats[i], i);
        }
        for (i = 0; i < n && i < filesCount; i++)
        {
            readPipeAndAppend(pipesToP, i, stat, filesCount); //TODO: indovina? contorlla  che la munnezz abbia ritornato e non sia andata al lago
            //printf("dal main analizer ricevo: %s\n", stat);
            //
        }
        decodeMultiple(stat, resultStats); //TODO:check error
        for (i = 0; i < filesCount; i++)
        {
            printf("In totale è stato letto nel file %d:\n", i);
            printStats(resultStats[i]);
        }
        printf("FINE!\n");
        return 0;
    }

    return 0;
}
