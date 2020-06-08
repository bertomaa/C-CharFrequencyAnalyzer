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

char *analyzerToReportPipe = "/tmp/analyzerToReport.pipe";
int isReportConnected = 0;
int waitForUserInputThread = 0;

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
    int fd;
    fd = open(analyzerToReportPipe, O_WRONLY);
    isReportConnected = 1;
    sendDataToReport(fd, conf);
    close(fd);
    printf("Data sent!\n");
    return NULL;
}

void *readUserInputAndQuit()
{
    //printf("z\n");
    char c;
    //printf("z\n");
    printf("Press enter to quit or open report to see data.\n");
    //printf("z\n");
    scanf("%c", &c);
    //printf("z\n");
    fatalErrorHandler("", 0);
    //printf("z\n");
    return NULL;
}

void *checkIsConnected(void* threadIdPointer)
{
    sleep(1);
    if (!isReportConnected)
    {
        waitForUserInputThread = 1;
        pthread_create((pthread_t*) threadIdPointer, NULL, &readUserInputAndQuit, NULL) ? fatalErrorHandler("Impossible to create Thread. Exit.", 1): 0;
    }
    return threadIdPointer;
}

void launchReportConnector(config *conf, char *dataStr)
{
    pthread_t listenerThreadId;
    pthread_t userInputThreadId;
    pthread_t checkConnectedId;
    void* checkConnectedIdPointer = (void*) &checkConnectedId;

    confAndEncodedString toSend;
    toSend.c = conf;
    toSend.str = dataStr;
    pthread_create(&listenerThreadId, NULL, &tryToConnect, (void *)&toSend) ? fatalErrorHandler("Impossible to create Thread. Exit.", 1) : 0;
    pthread_create(&checkConnectedId, NULL, &checkIsConnected, (void *)&userInputThreadId) ? fatalErrorHandler("Impossible to create Thread. Exit.", 1) : 0;
    pthread_join(listenerThreadId, NULL) ? fatalErrorHandler("Impossible to join Thread. Exit.", 1): 0;
    if (waitForUserInputThread)
    {
        pthread_join(checkConnectedId, (void**) &checkConnectedIdPointer);
        pthread_cancel(*((pthread_t*)checkConnectedIdPointer)) ? fatalErrorHandler("Impossible to cancel Thread. Exit.", 1): 0;
    }
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
