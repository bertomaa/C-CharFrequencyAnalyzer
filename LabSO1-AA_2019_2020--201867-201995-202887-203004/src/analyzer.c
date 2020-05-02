/*
    This fils is the analyzer: the main process creates n subprocesses, that create m subsubprocesses that analyze the files.
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

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

char **analyzeText()
{
    int fd;
    int i;
    char *ret = (char *)calloc(MAX_CHARACTERS, sizeof(char));

    //open file "texto.txt"
    fd = open("testo.txt", "r");

    int characters[ASCII_CHARACTERS];
    for (i = 0; i < ASCII_CHARACTERS; i++)
    {
        characters[i] = 0;
    }

    char a = 0;
    while ((a = (char)getc(fd)) != EOF)
    {
        characters[a]++;
    }
    for (i = 0; i < 256; i++)
    {

        //printf("Sono presenti %d simboli %c\n", characters[i], (char)i);
        //547\n89
    }

    return ret;
}

//process q, reads his part (index) of file
int q(int index, int m, const char *files[], int writePipe)
{
    //TODO: write
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
            q(i, m, files, pipes[getPipeIndex(i, WRITE)]);
        }
        else //father
            pids[i] = pid;
        printf("%s\n", files[i]);
    }

    while (wait != -1)
        ; //father waits all children
    for (i = 0; i < m; i++){
        //sumResults();//TODO: function to sum up all result got by children
    }

        return 0;
}

//data sent as input: n,m, namefile, namefile2...

int main(int argc, const char *argv[])
{
    if (checkArguments(argc, argv) != 0)
        exit(1);

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);

    //TODO: if a filename is a folder then find

    //TODO: divide files among proceses P
    int * pipesToP = createPipes(n);
    pid_t *pids = (int *)calloc(n, sizeof(int));
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
    for (i = 0; i < n; i++)
    {
        wait(pids[i]);
    }

    return 0;
}