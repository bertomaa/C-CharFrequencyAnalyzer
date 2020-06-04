#ifndef __FORKHANDLER_H__
#define __FORKHANDLER_H__

#include "commons.h"

#define PROCESS_MAX_CHILDREN 128

int processType = FATHER;
pid_t *children;
int childrenCount = 0;
int processMaxChildren = PROCESS_MAX_CHILDREN;

void initProcess();

void sigKillHandler(int sig);

void killMeAndAllChildren(int ExitCode);

int createChild();

void setIamChild();

int getProcessType();

#endif