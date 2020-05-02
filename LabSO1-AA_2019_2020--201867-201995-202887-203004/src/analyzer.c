/*
    This fils is the analyzer: the main process creates n subprocesses, that create m subsubprocesses that analyze the files.
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "stats.h"

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

stats analyzeText(int fd, int offset, int bytesToRead)
{
    int i;
    stats ret;
    char buffer[MAX_CHARACTERS];

    lseek(fd, offset, SEEK_SET);
    read(fd, buffer, bytesToRead);

    for(i = 0; buffer[i] != '\0'; i++)
    {
        ret.frequencies[buffer[i]]++;
    }

    return ret;
}

//process q, reads his part (index) of file
int q(int mIndex, int filesCount, int m, const char *files[], int writePipe)
{
    int fd, fileIndex, fileLength, i;
    stats stat, res;
    for (i = 0; i < filesCount; i++)
    {
        fd = open(files[i], O_RDONLY); //TODO: gestire se il file non esiste o bla bla
        fileLength = lseek(fd, 0, SEEK_END);
        fileIndex = fileLength / m;//TODO: gestire resto divisione, insomma che lo steso carattere non venga letto 2 volte o 0
        stat = analyzeText(fd, fileIndex, fileLength);
        res = sumStats(res, stat);
        close(fd);//TODO: come sempre controllare che si sia chiusa munnezz 
    }
    char * encoded = encode(res);
    write(writePipe, encoded, strlen(encoded)+1);//controlla sta cacata
    return 0;
}

//process p, generates m children processes q and assigns them the sections of file to analyze
int p(int m, int filesCount, const char *files[], int writePipe)
{
    int *pipes = createPipes(m);
    printf("I must analize files:\n");
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
            q(i, filesCount, m, files, pipes[getPipeIndex(i, WRITE)]);
        }
        else //father
            pids[i] = pid;
        printf("%s\n", files[i]);
    }
    if(pid > 0){
        while (wait(NULL) != -1)
            ; //father waits all children
        char * stat;
        stats statsRes, tmpStats;
        int res;
        for (i = 0; i < m; i++)
        {
            readPipe(pipes, i, stat);//TODO: indovina? contorlla  che la munnezz abbia ritornato e non sia andata al lago
            res = decode(stat, &tmpStats);
            if(res != 0)
                return 1;//TODO: esegui free ecc..
            statsRes = sumStats(statsRes, tmpStats);
        }
        stat = encode(statsRes);
        write(writePipe, stat, strlen(stat)+1);// stessa munnezz
    }
    return 0;//manco lo scrivo più
}

//data sent as input: n,m, namefile, namefile2...

int main(int argc, const char *argv[])
{
    if (checkArguments(argc, argv) != 0)
        exit(1);

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);

    //TODO: if a filename is a folder then find the files

    //TODO: divide files among proceses P
    int *pipesToP = createPipes(n);
    pid_t *pids = (pid_t *)calloc(n, sizeof(int));
    int pid;

    //creates subprocess p
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
            int ret = p(m, argc - 3, argv + 3, pipesToP[getPipeIndex(i, WRITE)]);
            return ret;
        }
        else //father
            pids[i] = pid;
    }

    //father
    if(pid > 0){
        while (wait(NULL) != -1)
            ; //father waits all children
        char * stat;
        stats statsRes;
        for (i = 0; i < m; i++)
        {
            readPipe(pipesToP, i, stat);//TODO: indovina? contorlla  che la munnezz abbia ritornato e non sia andata al lago
            // statsRes = sumStats(statsRes, decode(stat));
        }
        return 0;
    }


    return 0;
}