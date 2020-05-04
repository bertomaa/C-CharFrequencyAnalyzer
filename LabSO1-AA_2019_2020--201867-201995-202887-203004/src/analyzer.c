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

//TODO: ifndef nei .h

int checkArguments(int argc, const char *argv[])
{
    if (argc < 4)
    {
        printf("Wrong arguments, usage is: analyzer <n> <m> <file1> <file2> ... \n");
        return 1;
    }
    return 0;
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
    write(pipes[getPipeIndex(index, WRITE)], toWrite, strlen(toWrite));
    return 0; //error code
}

int readPipe(int *pipes, int index, char *buf)
{
    read(pipes[getPipeIndex(index, READ)], buf, MAX_CHARACTERS);
    return 0; //error code
}

stats analyzeText(int fd, int offset, int bytesToRead, int id)
{
    int i;
    stats ret;
    initStats(&ret, id);
    char buffer[MAX_CHARACTERS];

    lseek(fd, offset, SEEK_SET);
    read(fd, buffer, bytesToRead);

    for (i = 0; buffer[i] != '\0'; i++)
    {
        //fin qua ce sta
        ret.frequencies[buffer[i]]++;
    }

    return ret;
}

//process q, reads his part (index) of file
int q(int mIndex, int filesCount, int m, const char *files[], int writePipe, int fileIndex)
{
    int fd, fileOffset, fileLength, i, error;
    stats stat;
    for (i = 0; i < filesCount; i++)
    {
        error = openWrapper(files[i], &fd);
        if (error)
        {
            continue;
        }
        fileLength = lseek(fd, 0, SEEK_END);
        fileOffset = (fileLength / m) * mIndex; //TODO: gestire resto divisione, insomma che lo steso carattere non venga letto 2 volte o 0
        stat = analyzeText(fd, fileOffset, fileLength, fileIndex + i);
        close(fd);
        char *encoded = encode(stat);                   //TODO: come sempre controllare che si sia chiusa munnezz
        write(writePipe, encoded, strlen(encoded) + 1); //controlla sta cacata
        //TODO: SE VOUI IN FUTURO METTI SIGNAL AL PADRE E IMPLEMENTA VISUALIZZAZIONE IN TEMPO REALE
        printf("figlio q ha scritto in pipe per file %s\n", files[i]);
    }
    //TODO free varie e close
    exit(0);
    return 0;
}

//process p, generates m children processes q and assigns them the sections of file to analyze
int p(int m, int filesCount, const char *files[], int writePipe, int fileIndex)
{
    int *pipes = createPipes(m);
    int i, pid;
    pid_t *pids = (int *)malloc(m * sizeof(int));
    for (i = 0; i < m; i++)
    {
        pid = fork();
        if (pid < 0)
        {
            printf("Error in fork in p, exit");
            exit(2);
        }
        else if (pid == 0) //children
        {
            q(i, filesCount, m, files, pipes[getPipeIndex(i, WRITE)], fileIndex);
        }
        else //father
            pids[i] = pid;
    }
    if (pid > 0)
    {
        while (wait(NULL) != -1)
            ; //father waits all children
        char* stat;
        int error = allocWrapper(MAX_CHARACTERS * filesCount, sizeof(stats), (void**) &stat);
        if (error)
        {
            fprintf(stderr, "Error, impossible to allocate buffer memory.");
            exit(2);
        }
        stats *resultStats;
        error = allocWrapper(filesCount, sizeof(stats), (void**) &resultStats);
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
            readPipe(pipes, i, stat); //TODO: indovina? controlla  che la munnezz abbia ritornato e non sia andata al lago
            printf("p ha ricevuto: %s\n", stat);
            /*
            //testing
            stats tmp;
            initStats(&tmp, 7);
            decode(stat, &tmp, (3));
            //testing
            */
            int decodeError = decodeMultiple(stat, resultStats); //TODO: check
            if (decodeError != 0)
            {
                printf("errore decode p");
                return 1; //TODO: esegui free ecc..
            }
            printf("fine ciclo %d\n", i);
        }
        printStats(resultStats[0]);
        char* resultString = encode(resultStats[0]);
        write(writePipe, resultString, strlen(resultString) + 1); // stessa munnezz
    }
    return 0; //manco lo scrivo più
}

//data sent as input: n,m, namefile, namefile2...
int main(int argc, const char *argv[])
{
    if (checkArguments(argc, argv) != 0)
        exit(1);

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int filesCount = argc - 3;

    //TODO: if a filename is a folder then find the files

    //TODO: divide files among proceses P
    int *pipesToP = createPipes(n);
    pid_t *pids = (pid_t *)calloc(n, sizeof(int));
    int pid;

    //creates subprocess p
    printf("starting with n: %d, m: %d\n", n, m);
    int i;
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
            int ret = p(m, filesCount, argv + 3, pipesToP[getPipeIndex(i, WRITE)], 0);
            return ret;
        }
        else
        { //father
            pids[i] = pid;
        }
    }

    //father
    if (pid > 0)
    {
        printf("\n\nmain: waiting for all children\n\n");
        while (wait(NULL) != -1)
            ; //father waits all children
        printf("\n\nmain: waited for all children\n\n");
        char* stat;
        int error = allocWrapper(MAX_CHARACTERS * filesCount, sizeof(stats), (void**) &stat);//TODO:trova una stima migliore
        stats *resultStats;
        for (i = 0; i < filesCount; i++)
        {
            initStats(&resultStats[i], i);
        }
        for (i = 0; i < n; i++)
        {
            readPipe(pipesToP, i, stat); //TODO: indovina? contorlla  che la munnezz abbia ritornato e non sia andata al lago
            printf("dal main analizer ricevo: %s\n", stat);
            decodeMultiple(stat, resultStats);      //TODO:check error
        }
        printf("In totale è stato letto:");
        printStats(resultStats[0]);
        return 0;
    }

    return 0;
}
