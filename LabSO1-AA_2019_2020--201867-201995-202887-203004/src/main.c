#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wrapper.h"
#include "stats.h"
#include "config.h"

#define MAX_COMMAND_LEN 1024
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
    if (strlen(linep) > 1)
        *(line - 1) = '\0';
    return linep;
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
    }
    else if (p == 0)
    { //figlio
        char **args = exportAsArguments(conf);
        execvp("./analyzer", args);
    }
    else
    { //padre
        wait();
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
}

void showHelp()
{
    printf("Allowed action(s): ");
    printf("- Set values of n and m \n");
    printf("- Add files (or directories) to analyze: usage is <file1> <file2> ... <filen>\n");
    printf("- Show the current configuration, with the added files \n");
    printf("- Run report that sums up the informations \n");
}

void showCommandNotFoundError(const char *arguments)
{
    if (arguments[0] != '\n')
        printf("Command %s is not valid, see usage with help\n", arguments);
}

void set(char *arguments, config *conf)
{
    char **endptr;
    char *pn = strtok(arguments, " ");
    char *pm = strtok(NULL, " ");
    if (pn != NULL && pm != NULL)
    {
        int n1 = strtol(pn, endptr, 10);
        int m1 = strtol(pm, endptr, 10);
        if (n1 > 0 && m1 > 0)
        {

            printf("n = %d, m = %d\n", n1, m1);
            conf->n = n1;
            conf->m = m1;
    printf("n = %d, m = %d\n", conf->n, conf->m);
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
    }
}

int getAction(char *command, config *conf)
{

    char *token = strtok(command, " ");

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
        printf("getAction: n = %d, m = %d\n", conf->n, conf->m);
    }
    else if (strcmp(token, "report") == 0)
    {
        runReport(conf);
    }
    else if (strcmp(token, "help") == 0)
    {
        showHelp();
    }
    else if (strcmp(token, "config") == 0)
    {
        showConfig(conf); //works
    }
    else if (strcmp(token, "exit") == 0 || strcmp(token, "quit") == 0 || strcmp(token, "q") == 0)
    {
        return(0);
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
    char **endptr;
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
        conf.n = strtol(argv[1], endptr, 2);
        conf.m = strtol(argv[2], endptr, 2);
        for (i = 0; i < filesCount; i++)
        {
            addFileToConfig(&conf, argv[i + PRE_FILES_ARGS]);
        }
    }

    int action;
    do
    {
        printf("Ready: ");
        char *command = getLine();
        action = getAction(command, &conf);
        printf("n = %d, m = %d\n", conf.n, conf.m);
    } while (action);
}