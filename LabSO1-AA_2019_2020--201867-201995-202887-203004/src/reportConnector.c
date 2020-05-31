#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <tgmath.h>
#include <signal.h>
#include <pthread.h>
#include "config.h"
#include "reportConnector.h"

void sendDataToReport(int fd, confAndEncodedString conf)
{
    int i = 0;
    char buffer[MAX_PATH_LEN];

    sprintf(buffer, "%d", conf.c->filesCount);
    write(fd, buffer, strlen(buffer) + 1);
    for (i = 0; i < conf.c->filesCount; i++)
    {
        strcpy(buffer, conf.c->files[i]);
        sprintf(buffer, "%s", conf.c->files[i]);
        write(fd, buffer, strlen(buffer) + 1);
    }
    write(fd, conf.str, strlen(conf.str) + 1);
}

void *tryToConnect(void *confPointer)
{
    //int e = unlink(analyzerToReportPipe);
    //printf("error:%d\n", e);
    //TODO: contrllare che e sia 0 altrimenti bloccare tutto, in casoil file non esista in rpecedenza non so cosa ritorni e, ma in quel casso deve andare
    confAndEncodedString conf = *((confAndEncodedString *)confPointer);
    mkfifo(analyzerToReportPipe, 0666); //TODO: forse sti permessi vanno cambiati che naimoli può scriverci nella pipe
    printf("made fifo\n");
    int fd;
    fd = open(analyzerToReportPipe, O_WRONLY);
    isReportConnected = 1;
    printf("connected!\n");
    sendDataToReport(fd, conf);
    close(fd);
    printf("finished sending data!\n");
}

void *readUserInputAndQuit()
{
    //printf("z\n");
    char c;
    //printf("z\n");
    printf("press enter to quit or open report to see data\n");
    //printf("z\n");
    scanf("%c", &c);
    //printf("z\n");
    exit(0);
    //printf("z\n");
}

void launchReportConnector(config *conf, char *dataStr)
{
    pthread_t listenerThreadId;
    pthread_t userInputThreadId;
    int waitForUserInputThread = 0;

    confAndEncodedString toSend;
    toSend.c = conf;
    toSend.str = dataStr;
    pthread_create(&listenerThreadId, NULL, &tryToConnect, (void *)&toSend);
    sleep(1);
    if (!isReportConnected)
    {
        waitForUserInputThread = 1;
        pthread_create(&userInputThreadId, NULL, &readUserInputAndQuit, NULL);
    }

    pthread_join(listenerThreadId, NULL);
    if (waitForUserInputThread)
        pthread_cancel(userInputThreadId);

    // if (pid < 0)
    // {
    //     //TODO: handle error
    // }
    // else if (pid > 0)
    // { //father

    // char buffer[255];
    // buffer[0] = '\0';
    // while(1)
    // {
    //     sleep(1);
    //     kill(pid, SIGUSR1);
    //     read(stdin, buffer, 250);
    //     if(strlen(buffer)){
    //         exit(0);
    //     }
    // }
    // }
    // else
    // { //child

    // }
}
