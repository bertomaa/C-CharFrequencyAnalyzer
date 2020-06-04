#ifndef __COMMONS_H__
#define __COMMONS_H__

#define MAX_COMMAND_LEN 2048
#define PRE_FILES_ARGS 3
#define MAX_PATH_LEN 1024
#define INITIAL_CONFIG_SIZE 128
#define READ 0
#define WRITE 1
#define ASCII_CHARACTERS 256
#define MAX_PIPE_CHARACTERS 8128
#define FATHER 0
#define CHILD 1

typedef struct garbageCollector
{
    int dim;
    int garbageCount;
    void **garbage;
} garbageCollector;

extern int isCollectingGarbage;
extern garbageCollector gc;

int initGC();

void addToGC(void *garbage);

void removeFromGC(void *p);

void collectGarbage();

void addDoubleQuotes(char *buffer, char *path);

char *removeDoubleQuotes(char *buffer, char *path);

char *getCommandOutput(const char *cmd);

int getPipeIndex(int index, int type);

char *splitString(char *buffer, char **str, char delimiter);

int getDigits(int n);

int does1StringMatch2(char *s1, char *s2);

void fatalErrorHandler(char *message, int errorCode);

#endif