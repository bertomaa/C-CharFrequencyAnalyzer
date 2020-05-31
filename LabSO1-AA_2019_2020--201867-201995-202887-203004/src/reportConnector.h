#ifndef __REPORTCONNECTOR_H__
#define __REPORTCONNECTOR_H__

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

char *analyzerToReportPipe = "/tmp/analyzerToReport.pipe";
int isReportConnected = 0;

typedef struct confAndEncodedString
{
    config *c;
    char *str;
} confAndEncodedString;

void sendDataToReport(int fd, confAndEncodedString conf);

void *tryToConnect(void *confPointer);

void *readUserInputAndQuit();

void launchReportConnector(config *conf, char *dataStr);

#endif