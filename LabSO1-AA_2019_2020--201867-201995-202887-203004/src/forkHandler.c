#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "forkHandler.h"
#include "commons.h"
#include "wrapper.h"

void alertFather(int sig)
{
    if (getProcessType() == FATHER)
    {
        printf("is father %d\n", getpid());
        //printf("SIGTERM FATHER %d\n", getpid());
        killMeAndAllChildren(1);
    }
    printf("child: me:%d padre:%d\n", getpid(), getppid());
    kill(getppid(), SIGUSR1);
}

void initProcess()
{
    signal(SIGUSR1, alertFather);
    signal(SIGTERM, killMeAndAllChildren);
    allocWrapper(processMaxChildren, sizeof(int), (void **)&children);
}

int createChild()
{
    pid_t pid = fork();
    if (pid < 0)
    {
        fatalErrorHandler("Cannot create child process, exit", 1);
    }
    else if (pid == 0)
    {
        initProcess();
        setIamChild();
        return 0;
    }
    else
    {
        if (childrenCount > processMaxChildren - 3)
        {
            processMaxChildren += PROCESS_MAX_CHILDREN;
            reallocWrapper((void **)&children, processMaxChildren);
        }
        printf(" %d %d \n", pid, childrenCount);
        children[childrenCount] = pid;
        childrenCount++;
        return 1;
    }
}

void killMeAndAllChildren(int exitCode)
{
    int i;
    for (i = 0; i < childrenCount; i++)
    {
        //printf("SIGTERM %d\n", children[i]);
        kill(children[i], SIGTERM);
    }
    while (wait(NULL) != -1)
        ;

    if (!isCollectingGarbage)
    {
        collectGarbage();
    }
    printf("Me ammazzoooo, pid:%d\n", getpid());
    exit(exitCode);
}

void setIamChild()
{
    processType = CHILD;
}

int getProcessType()
{
    return processType;
}

