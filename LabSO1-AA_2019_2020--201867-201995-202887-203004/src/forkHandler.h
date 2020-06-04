#ifndef __FORKHANDLER_H__
#define __FORKHANDLER_H__

#include "commons.h"

#define PROCESS_MAX_CHILDREN 128

extern int processType;
extern pid_t *children;
extern int childrenCount;
extern int processMaxChildren;

void alertFather(int sig);

void initProcess();

void sigKillHandler(int sig);

void killMeAndAllChildren(int ExitCode);

int createChild();

void setIamChild();

int getProcessType();

#endif