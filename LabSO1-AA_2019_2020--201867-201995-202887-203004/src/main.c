#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wrapper.h"
#include "stats.h"
#include "config.h"
#include "commons.h"

char *getLine()
{
    int size = MAX_COMMAND_LEN;
    int i = 0;
    char *line;
    int error = allocWrapper(size, sizeof(char), (void **)&line);
    //TODO: error
    char c;
    while (c != '\n')
    {
        scanf("%c", &c);
        if (i + 1 >= size)
        {
            size += MAX_COMMAND_LEN;
            error = reallocWrapper((void **)&line, size);
            //TODO: error
        }
        line[i] = c;
        i++;
    }
    line[i - 1] = 0;
    return line;
}

void runAnalyzer(config *conf)
{
    printf("Run analyzer\n");
    // sprintf(conf->files[0], "%d", conf->n);
    // sprintf(conf->files[1], "%d", conf->m);
    int p = fork();
    if (p < 0)
    {
        fprintf(stderr, "Impossible to fork, quit.\n");
        exit(1);
    }
    else if (p == 0)
    { //figlio
        char **args = exportAsArguments(conf);
        execvp("./analyzer", args);
    }
    else
    { //padre
        //wait();
    }
}

void add(const char *arguments, config *conf)
{
    char *pch;
    char *input; //TODO: definire meglio size da allocare
    int error = allocWrapper(MAX_COMMAND_LEN, sizeof(char), (void **)&input);
    strcpy(input, arguments);
    pch = strtok(input, " ");
    while (pch != NULL)
    {
        addFileToConfig(conf, pch);
        pch = strtok(NULL, " ");
    }
}

void runReport(config *conf)
{
    printf("Run runReport\n");
    int p = fork();
    if (p < 0)
    {
        fprintf(stderr, "Impossible to fork, quit.\n");
        exit(1);
    }
    else if (p == 0)
    { //figlio
        char **args = exportAsArguments(conf);
        execvp("./report", args);
    }
    else
    { //padre
        //wait();
    }
}

void showHelp()
{
    printf("Allowed actions:\n");
    printf("set <n> <m>             - Set values of n and m.\n");
    printf("add <file1> <file2> ... - Add files (or directories) to analyze.\n");
    printf("config                  - Show the current configuration, with the added files \n");
    printf("report                  - Run report that sums up the informations \n");
}

void showCommandNotFoundError(const char *arguments)
{
    if (arguments[0] != '\n')
        printf("Command %s is not valid, see usage with help\n", arguments);
}

void set(char *arguments, config *conf)
{
    char *endptr;
    char *pn = strtok(arguments, " ");
    char *pm = strtok(NULL, " ");
    if (pn != NULL && pm != NULL)
    {
        int n1 = strtol(pn, &endptr, 10);
        int m1 = strtol(pm, &endptr, 10);
        if (n1 > 0 && m1 > 0)
        {
            conf->n = n1;
            conf->m = m1;
        }
        else
        {
            printf("Command \"set %s\" is not valid, see usage with help\n", arguments);
        }
    }
    else
    {
        showHelp();
    }
}

void showConfig(config *conf)
{
    printf("n = %d, m = %d\n", conf->n, conf->m);
    if (conf->n > 0 && conf->m > 0)
    {
        printf("Ready to run with n = %d, m = %d\n", conf->n, conf->m);
        printFiles(conf);
    }
    else
    {
        printf("Set n and m with set <n> <m>\n");
        printFiles(conf);
    }
}

int getAction(char *command, config *conf)
{
    char *token = strtok(command, " ");
    if(token == NULL)
        return 1;

    if (strcmp(token, "run") == 0)
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
    else if (strcmp(token, "report") == 0)
    {
        runReport(conf);
    }
    else if (strcmp(token, "help") == 0)
    {
        showHelp();
    }
    else if (strcmp(token, "config") == 0 || strcmp(token, "c") == 0)
    {
        showConfig(conf); //works
    }
    else if (strcmp(token, "exit") == 0 || strcmp(token, "quit") == 0 || strcmp(token, "q") == 0)
    {
        return 0;
    }
    else
    {
        showCommandNotFoundError(token);
    }
    return 1;
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
    initGC();
    char *endptr;
    config conf;

    initConfig(&conf);

    if (argc > 1)
        if (checkArguments(argc, argv) != 0)
            exit(1);
    int i;
    //TODO: check error
    int filesCount = argc - PRE_FILES_ARGS;
    if (argc >= 3)
    {
        conf.n = strtol(argv[1], &endptr, 10);
        conf.m = strtol(argv[2], &endptr, 10);
        for (i = 0; i < filesCount; i++)
        {
            addFileToConfig(&conf, argv[i + PRE_FILES_ARGS]);
        }
    }

    int action;
    do
    {
        printf("-> ");
        char *command = getLine();
        action = getAction(command, &conf);
    } while (action);
    collectGarbage();
    return 0;
}