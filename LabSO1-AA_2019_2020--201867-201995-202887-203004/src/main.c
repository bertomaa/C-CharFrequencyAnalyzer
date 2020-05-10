#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wrapper.h"
#include "stats.h"
#include "config.h"

#define MAX_COMMAND_LEN 1024
#define MAX_PATH_LEN 1024
#define PRE_FILES_ARGS 3

char *getLine()
{
    char *line = malloc(MAX_COMMAND_LEN);
    char *linep = line;
    size_t lenmax = MAX_COMMAND_LEN, len = lenmax;
    int c;

    if (line == NULL)
        return NULL;

    for (;;)
    {
        c = fgetc(stdin);
        if (c == EOF)
            break;
        if (--len == 0)
        {
            len = lenmax;
            char *linen = realloc(linep, lenmax *= 2);

            if (linen == NULL)
            {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }
        if ((*line++ = c) == '\n')
            break;
    }
    *line = '\0';
    return linep;
}

void runAnalyzer(config *conf)
{
    printf("Run analyzer\n");
    sprintf(conf->files[0], "%d", conf->n);
    sprintf(conf->files[1], "%d", conf->m);
    int p = fork();
    if (p < 0)
    {
        fprintf(stderr, "Impossible to fork, quit.\n");
    }
    else if (p == 0)
    { //figlio
        execvp("./analyzer", conf->files);
    }
    else
    { //padre
        wait();
    }
}

void add(const char *arguments, config *conf)
{
    printf("Run add\n");
}

void runReport(config *conf)
{
    printf("Run runReport\n");
}

void showHelp(const char *arguments, config *conf)
{
    printf("Run showHelp\n");
}

void showCommandNotFoundError(const char *arguments)
{
    if (arguments[0] != '\n')
        printf("Run showCommandNotFoundError\n");
}

void set(char *arguments, config *conf)
{
    char *pn = strtok(arguments, " ");
    char *pm = strtok(arguments, " ");
    conf->n = atoi(pn);
    conf->m = atoi(pm);
}

void showConfig(config *conf)
{
    if (conf->n > 0 && conf->m > 0)
    {
        printf("ready to run with n = %d, m = %d\n", conf->n, conf->m);
        printFiles(conf);
    }
    else
    {
        printf("set m and n with set <m> <n>\n");
    }
    //TODO: print files
}

int getAction(char *command, config *conf)
{

    char *token = strtok(command, " ");

    if (strcmp(token, "run\n") == 0)
    {
        runAnalyzer(conf);
    }
    else if (strcmp(token, "add") == 0)
    {
        add(command + 4, conf);
    }
    else if (strcmp(token, "set") == 0)
    {
        set(command + 4, conf);
    }
    else if (strcmp(token, "report\n") == 0)
    {
        runReport(conf);
    }
    else if (strcmp(token, "help\n") == 0)
    {
        showHelp(command, conf);
    }
    else if (strcmp(token, "help") == 0)
    {
        showHelp(command + 5, conf);
    }
    else if (strcmp(token, "config\n") == 0)
    {
        showConfig(conf);
    }
    else if (strcmp(token, "exit\n") == 0 || strcmp(token, "quit\n") == 0 || strcmp(token, "q\n") == 0)
    {
        exit(0);
    }
    else
    {
        showCommandNotFoundError(token);
    }
    return 0;
}

int checkArguments(int argc, const char *argv[])
{
    if (argc < 4)
    {

        printf("Wrong arguments, usage is: main <n> <m> <file1> <file2> ... \n");
        return 1;
    }
    return 0;
}

int main(int argc, const char *argv[])
{
    config conf;
    initConfig(&conf);
    if (argc > 1)
        if (checkArguments(argc, argv) != 0)
            exit(1);
    int i;
    //TODO: check error
    int filesCount = argc - PRE_FILES_ARGS;

    if (argc > 3)
    {
        conf.n = atoi(argv[1]);
        conf.m = atoi(argv[2]);
        for (i = 0; i < filesCount; i++)
        {
            addFileToConfig(&conf, argv[i + PRE_FILES_ARGS]);
        }
    }

    printFiles(&conf);

    int action;
    do
    {
        printf("ready: ");
        char *command = getLine();
        action = getAction(command, &conf);
    } while (1);
}