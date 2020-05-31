#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wrapper.h"
#include "stats.h"
#include "config.h"
#include "commons.h"

char *mainToReportPipe = "/tmp/mainToReport.pipe";
char *path;
int fdToReport;

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

char *getBinPath(const char *arg0)
{
    char *res;
    allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&res); //TODO: una piotta e diesci terza marcia
    strcpy(res, arg0);
    int i;
    for (i = strlen(res) - 2; i > 0 && res[i] != '/'; i--)
        ;
    res[i + 1] = '\0';
    return res;
}

void run(config *conf)
{
    printf("Run analyzer and Report\n");

    if (!isReadyToRun(conf))
    {
        printf("Cannot run, set n, m and at least 1 file.\n");
        return;
    }

    int p = fork();
    if (p < 0)
    {
        fprintf(stderr, "Impossible to fork, quit.\n");
        exit(1);
    }
    else if (p == 0)
    { //figlio
        char *analyzerPath;
        allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&analyzerPath); //TODO: una piotta e diesci terza marcia
        strcpy(analyzerPath, path);
        strcat(analyzerPath, "analyzer");
        printf("%s\n", analyzerPath);
        char **args = exportAsArguments(conf, analyzerPath);
        execv(analyzerPath, args);
        exit(0);
    }
    //padre
    int q = fork();
    if (q < 0)
    {
        fprintf(stderr, "Impossible to fork, quit.\n");
        exit(1);
    }
    else if (q == 0)
    { //figlio
        char *reportPath;
        allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&reportPath); //TODO: una piotta e diesci terza marcia
        strcpy(reportPath, path);
        strcat(reportPath, "report");
        printf("%s\n", reportPath);
        char **args = exportAsArguments(conf, reportPath);
        execl(reportPath, reportPath, "--main", NULL);
        exit(0);
    }
    printf("sono il main!\n");

    //TODO: creare la pipe per report
    // mkfifo(mainToReportPipe, 0666);
    // fdToReport = open(mainToReportPipe, O_WRONLY);
    // if(fdToReport == -1)
    // {
    //     printf("pipe error\n");
    //     exit(1);
    // }
    // printf("main connected to report\n");
}

void addFiles(const char *arguments, config *conf)
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

void passToReport(char *command)
{
    printf("pass to Report\n");
    mkfifo(mainToReportPipe, 0666);
    fdToReport = open(mainToReportPipe, O_WRONLY);
    if(fdToReport == -1)
    {
        printf("pipe error\n");
        exit(1);
    }
    printf("main connected to report\n");
    write(fdToReport, command, sizeof(command) + 1);
    close(fdToReport);
}

void showHelp()
{
    printf("Allowed actions:\n");
    printf("set <n> <m>                 - Set values of n (number of p processes) and m (number of q processes) to pass to analyzer.\n");
    printf("add <file1> <file2> ...     - Add files (or directories) to be analyzed.\n");
    printf("config                      - Show the current configuration, with the added files\n");
    printf("run                         - Run analyzer and report\n");
    printf("report <cmd>                - Used to issue commands to report, must be used after run\n");
    printf("remove <file1> <file2> ...  - Removes files (or directories) from the list to be analyzed\n");
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

int isReadyToRun(config *conf)
{
    if (conf->n > 0 && conf->m > 0 && conf->filesCount != 0)
        return 1;
    return 0;
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

void removeFiles(char *arguments, config *conf)
{
    char *pch;
    char *input; //TODO: definire meglio size da allocare
    int error = allocWrapper(MAX_COMMAND_LEN, sizeof(char), (void **)&input);
    strcpy(input, arguments);
    pch = strtok(input, " ");
    while (pch != NULL)
    {
        removePathFromConfig(conf, pch);
        pch = strtok(NULL, " ");
    }
}

int getAction(char *command, config *newConf, config *analyzedConf)
{
    char *token = strtok(command, " ");
    if (token == NULL)
        return 1;

    if (strcmp(token, "run") == 0 || strcmp(token, "r") == 0)
    {
        run(newConf);
        int n = newConf->n;
        int m = newConf->m;
        joinConfigs(analyzedConf, newConf);
        initConfig(newConf);
        newConf->n = n;
        newConf->m = m;
    }
    else if (strcmp(token, "add") == 0)
    {
        addFiles(command + 4, newConf);
    }
    else if (strcmp(token, "set") == 0)
    {
        set(command + 4, newConf);
    }
    else if (strcmp(token, "report") == 0)
    {
        passToReport(command + 7);
    }
    else if (strcmp(token, "help") == 0)
    {
        showHelp();
    }
    else if (strcmp(token, "config") == 0 || strcmp(token, "c") == 0)
    {
        showConfig(newConf); //works
    }
    else if (strcmp(token, "remove") == 0)
    {
        removeFiles(command + 7, newConf);
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
    path = getBinPath(argv[0]);
    char *endptr;
    config initConf;

    initConfig(&initConf);

    if (argc > 1)
        if (checkArguments(argc, argv) != 0)
            exit(1);
    int i;
    //TODO: check error
    int filesCount = argc - PRE_FILES_ARGS;
    if (argc >= 3)
    {
        initConf.n = strtol(argv[1], &endptr, 10);
        initConf.m = strtol(argv[2], &endptr, 10);
        for (i = 0; i < filesCount; i++)
        {
            addFileToConfig(&initConf, argv[i + PRE_FILES_ARGS]);
        }
    }

    config *analyzedFilesConf;
    config *newFilesConf;
    newFilesConf = checkDirectories(&initConf);
    int error = allocWrapper(1, sizeof(config), (void **)&analyzedFilesConf);
    initConfig(analyzedFilesConf);

    int action;
    do
    {
        printf("-> ");
        char *command = getLine();
        action = getAction(command, newFilesConf, analyzedFilesConf);
    } while (action);
    collectGarbage();
    return 0;
}