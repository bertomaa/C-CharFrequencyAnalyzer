#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "wrapper.h"
#include "stats.h"
#include "config.h"
#include "commons.h"

char *analyzerToReportPipe = "/tmp/analyzerToReport.pipe";
char *mainToReportPipe = "/tmp/mainToReport.pipe";

void printTable(int start, int finish, char *name, stats resultStats)
{
    printf("\n %s\n", name); //prints the name of the section
    int k = start; //k = counter for the ascii characters
    int max = 0; //length of the biggest number
    int dim = 0; //dimension of the "current" number
    int search = 0; //i,j,k were already used
    for (search = 0; search < 256; search++){
        if (resultStats.frequencies[search] > max){
            max = resultStats.frequencies[search]; //search for the biggest number in frequencies
        }
    }
    //printf("\n %d \n", max);
    const int dimMax = getDigits(max); //save the digits of the biggest number
    // int dim = getDigits(arrayFrequencies[k]);
    int i = 0;
    
    //CHARACTERS
    do
    {

        if (resultStats.frequencies[k] != 0)
        {
            dim = getDigits(resultStats.frequencies[k]);
            //dim = getDigits(resultStats.frequencies[k]); //digits of the current number
            if (dim % 2 == 0) //even number of digits
            {
                for (i = 0; i < (dimMax / 2) - 1; i++) //prints spaces to the left of the CHARACTER
                    printf(" ");
                printf("%c", k);
                for (i = dimMax / 2; i < dimMax; i++) //prints spaces to the right of the CHARACTER
                    printf(" ");
            }
            else //odd number of digits
            {
                for (i = 0; i < (dimMax / 2); i++) //prints spaces to the left of the CHARACTER
                    printf(" ");
                printf("%c", k);
                for (i = (dimMax / 2) + 1; i < dimMax; i++) //prints spaces to the right of the CHARACTER
                    printf(" ");
            }
            printf("|");
        }
        k++;
    } while (k < finish);

    printf("\n");

    // NUMERI
    k = start;
    dim = 0;
    int d = 0;
    do
    {

        if (resultStats.frequencies[k] != 0)
        {
            dim = getDigits(resultStats.frequencies[k]);
            //d = dimMax - dim;
            if ((dimMax) % 2 == 0)
            {
                if(((dimMax - dim) / 2) - 1 == 0)
                {
                    printf(" ");
                }
                else
                {
                    for (i = 0; i < ((dimMax - dim) / 2) - 1; i++)
                        printf(" ");
                }
                printf("%d", resultStats.frequencies[k]);
                for (i = (((dimMax - dim) / 2) + dim); i < dimMax ; i++)
                    printf(" ");
            }
            else
            {
                for (i = 0; i < (dimMax - dim)/ 2; i++)
                    printf(" ");
                printf("%d", resultStats.frequencies[k]);
                for (i = (((dimMax - dim) / 2) + dim); i < dimMax; i++)
                    printf(" ");
            }
            printf("|");
        }
        k++;
    } while (k < finish);

    printf("\n");
    
    printf("\n");
    int j = 0;
    for (j = 0; j < dimMax*6; j++)
    {
        //for(i = 0; i < 1; i++)
            printf(" ");
        //for(i = 0; i < 7; i++)
            printf("-"); // Prints a line of -
    }
    printf("\n");
}

void print(stats resultStats)
{
    printTable(32, 39, "Punctuation", resultStats);
    printTable(40, 48, "Punctuation", resultStats);
    printTable(48, 58, "Numbers", resultStats);
    printTable(58, 65, "Operators", resultStats);
    printTable(65, 73, "Uppercase letters", resultStats);
    printTable(73, 82, "Uppercase letters", resultStats);
    printTable(82, 91, "Uppercase letters", resultStats); 
    printTable(91, 97, "Symbols", resultStats);
    printTable(97, 105, "Lowercase letters", resultStats);
    printTable(105, 114, "Lowercase letters", resultStats);
    printTable(114, 123, "Lowercase letters", resultStats);
    printTable(123, 128, "Other characters", resultStats); //some are divided in multiple row for style sake
}

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

void showHelp()
{
    printf("Allowed actions:\n");
    printf("report <file1> <file2> ...          - Shows the report of the specified files.\n");
    printf("report <directory1> <directory2>... - Shows the report of the file in the specified directoy.\n");
    printf("report <directory1> <directory2>... - Shows the report of the file in the specified directoy.\n");
    printf("config                              - Show the current configuration, with the added files \n");
}

void showCommandNotFoundError(const char *arguments)
{
    if (arguments[0] != '\n')
        printf("Command %s is not valid, see usage with help\n", arguments);
}

void showConfig(config *conf)
{
    printf("n = %d, m = %d\n", conf->n, conf->m);
    if (conf->n > 0 && conf->m > 0)
    {
        printf("Analyzer run with n = %d, m = %d\n", conf->n, conf->m);
        printFiles(conf);
    }
    else
    {
        printf("n and m not valid, see usage with help\n");
        printFiles(conf);
    }
}

void report(char *arguments, config *conf)
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

int getAction(char *command, config *conf)
{
    char *token = strtok(command, " ");
    if(token == NULL)
        return 1;

    if (strcmp(token, "report") == 0)
    {
        //CHECK SE TOKEN VA BENE COME ARGOMENTO PASSATO: L'HO MESSO A CASO AFFINCHE' COMPILASSE CORRETTAMENTE
        report(token, conf);
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

char *readStringFromPipe(int bufferLen, int fd)
{
    char *buffer;
    int error = allocWrapper(bufferLen, sizeof(char), (void **)&buffer); //TODO: usa il wrapper,
    char c = '7';
    int i = 0;
    while (c != 0)
    {
        read(fd, &c, 1);
        // printf("%c ", c);
        buffer[i] = c;
        i++;
    }
    return buffer;
}

int main(int argc, char *argv[])
{
    initGC();
    config conf;
    initConfig(&conf);
    int filesCount;
    char *e;
    printf("Waiting for Analyzer...\n");
    int fd = open(analyzerToReportPipe, O_RDONLY);
    printf("connected!\n");
    while (fd == -1)
    {
        printf("Impossible to open pipe, did you run Analyzer first? Press anything to retry or q to quit\n");
        //int c = getchar();
        char c;
        scanf("%c", &c);
        if (c == 'q')
            exit(0);
        fd = open(analyzerToReportPipe, O_RDONLY);
    };
    char *buffer = readStringFromPipe(40, fd);
    filesCount = strtol(buffer, &e, 10);
    int i;
    printf("%s\n%p\n%d\n", buffer, buffer, filesCount);
    for (i = 0; i < filesCount; i++)
    {
        buffer = readStringFromPipe(MAX_PATH_LEN, fd);
        printf("%s\n", buffer);
        addFileToConfig(&conf, buffer);
    }
    char *statString;
    int error = allocWrapper(MAX_PIPE_CHARACTERS * conf.filesCount, sizeof(char), (void **)&statString); //TODO: usa il wrapper,
    read(fd, statString, MAX_PIPE_CHARACTERS * conf.filesCount);
    stats *resultStats;
    error = allocWrapper(conf.filesCount, sizeof(stats), (void **)&resultStats); //TODO:trova una stima migliore

    for (i = 0; i < conf.filesCount; i++)
    {
        initStats(&(resultStats[i]), i);
    }
    printf("a\n");
    //printf("\n\nstat string: %s\n\n", statString);
    decodeMultiple(statString, resultStats); //TODO:check error
    printf("a\n");

    /*
    int action;
    do
    {
        printf("-> ");
        char *command = getLine();
        action = getAction(command, &conf);
    } while (action);
    */
   
    for (i = 0; i < conf.filesCount; i++)
    {
        //TODO: mettere in british
        printf("In file %s was analyzed:\n", conf.files[i]);
        print(*resultStats);
        printStats(resultStats[i]);
        printf("sgs\n");
    }

    printf("ss\n");
    int fdFromMain;
    printf("ss\n");
    printf("%d, %s\n", argc, argv[1]);
    char* command;
    if (argc > 1 && strcmp(argv[1], "--main") == 0)
    {
        while (1)
        {
            //come faccio per sapere se sono stato creato dal main o eseguito da console?
            fdFromMain = open(mainToReportPipe, O_RDONLY);
            if (fdFromMain == -1)
            {
                printf("pipe error with main\n");
                exit(1);
            }
            i++;
            printf("%d\n", i);

            command = readStringFromPipe(MAX_COMMAND_LEN, fdFromMain);
            printf("report cmd: %s\n", command);
            close(fdFromMain);
        }
    }
    else
    {
        int error = allocWrapper(MAX_COMMAND_LEN, sizeof(char), (void**) &command); //TODO:
        scanf("%s", command);
        printf("report cmd: %s\n", command);
    }

    printf("FINE REPORT!\n");
    return 0;
}

