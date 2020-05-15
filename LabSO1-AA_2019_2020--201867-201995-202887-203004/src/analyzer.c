/*
    This fils is the analyzer: the main process creates n subprocesses, that create m subsubprocesses that analyze the files.
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "stats.h"
#include "wrapper.h"

int checkArguments(int argc, const char *argv[])
{
    if (argc < 4)
    {
        printf("Wrong arguments, usage is: analyzer <n> <m> <file1> <file2> ... \n");
        return 1;
    }
    return 0;
}

int *distributeQuantity(int quantity, int toDistribute)
{
    int *ret;
    allocWrapperPipe(toDistribute, sizeof(int), (void **)&ret);
    int perProcess = quantity / toDistribute;
    int i;
    int tot = perProcess * toDistribute;
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
    int *pipes = allocWrapperPipe(size*2, sizeof(int), (void**) &pipes);
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

int readPipe(int *pipes, int index, char *buf, int toRead)
{
    readWrapperPipe(pipes, index, buf, toRead);
    printf("Read from pipe %d chars\n", (int)strlen(buf));
    return 0;
}

//controlla se write() eseguita correttamente
int myWrite(int toWrite, const char *buf, int nbytes)
{
    printf("Writing %d chars in myWrite\n", (int)strlen(nbytes));
    writeMyWrapper(toWrite, buf, nbytes);
    return 0;
}

//controlla se read() eseguita correttamente
int myRead(int toRead, const char *buf, int nbytes)
{
    printf("reading %d chars in myRead\n", (int)strlen(nbytes));
    readMyWrapper(toRead, buf, nbytes);
    return 0; //error code
}

//controlla se close() eseguita correttamente
int myClose(int fd)
{
    printf("closing file in myClose\n");
    closeMyWrapper(fd);
    return 0;
}

int readPipeAndAppend(int *pipes, int index, char *buf, int toRead)
{
    int len = strlen(buf);
    read(pipes[getPipeIndex(index, READ)], buf + len, MAX_CHARACTERS * toRead);
    printf("Read from pipe nad appended %d chars\n", (int)strlen(buf) - len);
    return 0; //error code
}

stats analyzeText(int fd, int offset, int bytesToRead, int id)
{
    int i;
    stats ret; //TODO: dovrebbe essere allocato dinamicamente se viene ritornato?
    initStats(&ret, id);
    char* buffer;
    allocWrapperPipe(bytesToRead + 1, sizeof(char), (void**) &buffer);

    lseek(fd, offset, SEEK_SET);
    myRead(fd, buffer, bytesToRead);
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
    allocWrapperPipe(filesCount, sizeof(stats), (void **)&statsToSend);
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
        myClose(fd);
    }
    char *encoded = encodeMultiple(statsToSend, filesCount);
    printf("sending from q%d: %s\n", mIndex, encoded);
    //char *encoded = encodeMultiple(statsToSend, filesCount);                   //TODO: come sempre controllare che si sia chiusa munnezz
    myWrite(writePipe, encoded, strlen(encoded) + 1); //controlla sta cacata
    myClose(writePipe);
    //TODO: free varie e close
    return 0;
}

//process p, generates m children processes q and assigns them the sections of file to analyze
int p(int m, int filesCount, const char *files[], int writePipe, int fileIndex)
{
    printf("p: %d, con filescount: %d, fileindex: %d\n", m, filesCount, fileIndex);
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
        allocWrapperPipe(MAX_CHARACTERS * filesCount, sizeof(stats), (void **)&str);
        stats *resultStats;
        allocWrapperPipe(filesCount, sizeof(stats), (void **)&resultStats);
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
            decodeErrorWrapper(str, resultStats);
            // for (j = 0; j < filesCount; j++)
            // {
            //     printf("(p)In totale è stato letto nel file %d da q %d:\n", j, i);
            //     printStats(resultStats[j]);
            // }
            //int *p = malloc(sizeof(int));
            //*p = 3;
            //int decodeError = decode(str, resultStats, p); //TODO: check
        }
        char *resultString = encodeMultiple(resultStats, filesCount);
        printf("mandato al main:%s\n", resultString);
        myWrite(writePipe, resultString, strlen(resultString) + 1); // stessa munnezz
        myClose(writePipe);
    }
    return 0; //manco lo scrivo più
}

//data sent as input: n,m, namefile, namefile2...
int main(int argc, const char *argv[])
{
    int i;
    for (i = 0; i < 50; i++)
    {
        printf("\n");
    }
    if (checkArguments(argc, argv) != 0)
        exit(1);

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int filesCount = argc - 3;

    //TODO: if a filename is a folder then find the files

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
        allocWrapperPipe(MAX_CHARACTERS * filesCount, sizeof(stats), (void **)&stat); //TODO:trova una stima migliore
        stats *resultStats;
        for (i = 0; i < filesCount; i++)
        {
            initStats(&resultStats[i], i);
        }
        for (i = 0; i < n; i++)
        {
            readPipeAndAppend(pipesToP, i, stat, filesCount); //TODO: indovina? contorlla  che la munnezz abbia ritornato e non sia andata al lago
            printf("dal main analizer ricevo: %s\n", stat);
            //
        }
        decodeErrorWrapper(stat, resultStats);
        for (i = 0; i < filesCount; i++)
        {
            printf("In totale è stato letto nel file %d:\n", i);
            printStats(resultStats[i]);
        }
        printf("FINE!");
        return 0;
    }

    return 0;
}
