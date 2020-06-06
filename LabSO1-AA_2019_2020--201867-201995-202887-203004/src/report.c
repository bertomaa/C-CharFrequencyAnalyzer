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
#include "forkHandler.h"

char *analyzerToReportPipe = "/tmp/analyzerToReport.pipe";
char *mainToReportPipe = "/tmp/mainToReport.pipe";
int FDanalyzer;

void readAnalyzer(confAndStats *conf);

void printTable(int start, int finish, char *name, stats resultStats);

void print(stats resultStats);

char *getLine()
{
    int size = MAX_COMMAND_LEN;
    int i = 0;
    char *line;
    allocWrapper(size, sizeof(char), (void **)&line);
    //TODO: error
    char c;
    while (c != '\n')
    {
        scanf("%c", &c);
        if (i + 1 >= size)
        {
            size += MAX_COMMAND_LEN;
            reallocWrapper((void **)&line, size * sizeof(char));
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
    printf("show [--every] <file1> [directory1] ...   - Shows the report of the specified files or directories.\n");
    printf("showall                                      - Shows the report of all files analyzed.\n");
    printf("help                                         - Show this help message\n");
    printf("read                                         - Read data from analyzer, this action is not required if report was created by main\n");
}

void show(char *files, confAndStats *cs)
{
    printf("show\n");
    int i;
    char *buffer;
    allocWrapper(MAX_PATH_LEN, sizeof(char), (void **)&buffer);
    buffer = splitStringWithQuotes(buffer, &files, ' ');
    int every = 0;
    int match = 0;
    while (buffer != NULL)
    {
        match = 0;
        stats tmpStat;
        initStats(&tmpStat, -1);
        for (i = 0; i < cs->conf->filesCount; i++)
        {
            if (i == 0 && strcmp("--every", buffer) == 0)
            {
                every = 1;
                break;
            }
            if (does1StringMatch2(buffer, cs->conf->files[i]))
            {
                match = 1;
                if (every)
                {
                    printf("File %s:\n", cs->conf->files[i]);
                    //printTable(cs->stats[i]);
                    printStats(cs->stats[i]);
                }
                else
                {
                    sumStats(&tmpStat, &(cs->stats[i]));
                }
            }
        }
        if (!every && match)
        {
            printf("Path %s:\n", buffer);
            //printTable(tmpStat);
            printStats(tmpStat);
        }
        buffer = splitStringWithQuotes(buffer, &files, ' ');
    }
}

void showAll(confAndStats *cs)
{
    int i;
    for (i = 0; i < cs->conf->filesCount; i++)
    {
        printf("File %s:\n", cs->conf->files[i]);
        //printTable(cs->stats[i]);
        printStats(cs->stats[i]);
    }
}

void showCommandNotFoundError(const char *arguments)
{
    if (arguments[0] != '\n')
        printf("Command %s is not valid, see usage with help\n", arguments);
}

int processCommand(char *command, confAndStats *cs)
{
    char *token = strtok(command, " ");
    if (token == NULL)
        return 1;

    //help, exit, read (richiama readAnalyzer), show (mostra il file/ i file nella directory) (showall mostra tutto),

    if (strcmp(token, "read") == 0)
    {
        readAnalyzer(cs);
    }
    else if (strcmp(token, "help") == 0)
    {
        showHelp();
    }
    else if (strcmp(token, "show") == 0)
    {
        show(command + 5, cs);
    }
    else if (strcmp(token, "showall") == 0)
    {
        showAll(cs);
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
    allocWrapper(bufferLen, sizeof(char), (void **)&buffer);
    char c = '7';
    int i = 0;
    while (c != 0)
    {
        read(fd, &c, 1);
        // printf("%c ", c);
        buffer[i] = c;
        i++;
    }
    buffer[i] = '\0';
    return buffer;
}

void readAnalyzer(confAndStats *cs)
{
    int lastStatsIndex = cs->conf->filesCount;
    FDanalyzer = open(analyzerToReportPipe, O_RDONLY);
    while (FDanalyzer == -1)
    {
        printf("Impossible to open pipe, did you run Analyzer first? Press anything to retry or q to quit\n");
        //int c = getchar();
        char c;
        scanf("%c", &c);
        if (c == 'q')
            exit(0);
        FDanalyzer = open(analyzerToReportPipe, O_RDONLY);
    };
    printf("connected!\n");
    char *e;
    char *buffer = readStringFromPipe(40, FDanalyzer);
    int filesCount = strtol(buffer, &e, 10);
    int i;

    for (i = 0; i < filesCount; i++)
    {
        buffer = readStringFromPipe(MAX_PATH_LEN, FDanalyzer);
        printf("%s\n", buffer);
        addFileToConfig(cs->conf, buffer);
        if (cs->conf->filesCount + 1 >= cs->conf->dim)
        {
            reallocWrapper((void **)&(cs->stats), (cs->conf->dim + INITIAL_CONFIG_SIZE) * sizeof(stats)); //TODO: error
        }
    }
    char *statString;
    allocWrapper(MAX_PIPE_CHARACTERS * cs->conf->filesCount, sizeof(char), (void **)&statString); //TODO: usa il wrapper,
    read(FDanalyzer, statString, MAX_PIPE_CHARACTERS * cs->conf->filesCount);
    close(FDanalyzer);
    for (i = lastStatsIndex; i < cs->conf->filesCount; i++)
    {
        initStats(&(cs->stats[i]), i);
    }
    //printf("\n\nstat string: %s\n\n", statString);
    int error = decodeMultiple(statString, &(cs->stats[lastStatsIndex])); //TODO:check error
    if (error)
    {
        fatalErrorHandler("Error in decoding pipe, exit.", 1);
    }
    for (i = 0; i < cs->conf->filesCount; i++)
    {
        printf("In file %s was analyzed:\n", cs->conf->files[i]);
        printStats(cs->stats[i]);
        //print(cs->stats[i]);
    }
}

int main(int argc, char *argv[])
{
    initGC();
    initProcess();
    mkfifo(analyzerToReportPipe, 0666);
    confAndStats cs;
    allocWrapper(1, sizeof(config), (void **)&(cs.conf));
    allocWrapper(INITIAL_CONFIG_SIZE, sizeof(stats), (void **)&(cs.stats));
    // TODO: terza marcia
    initConfig(cs.conf);
    printf("Waiting for Analyzer...\n");
    readAnalyzer(&cs);
    printf("fine\n");

    int fdFromMain;
    char *command;
    int goOn;
    if (argc > 1 && strcmp(argv[1], "--main") == 0)
    {
        do
        {
            printf("report ready\n");
            fdFromMain = open(mainToReportPipe, O_RDONLY);
            while (fdFromMain == -1)
            {
                printf("Impossible to open pipe with main, did you run report --main from command line? Press anything to retry or q to quit\n");
                //int c = getchar();
                char c;
                scanf("%c", &c);
                if (c == 'q')
                    exit(0);
                fdFromMain = open(mainToReportPipe, O_RDONLY);
            };
            printf("report opened pipe\n");
            command = readStringFromPipe(MAX_COMMAND_LEN, fdFromMain);
            printf("report cmd: %s\n", command);
            goOn = processCommand(command, &cs);
            close(fdFromMain);
        } while (goOn);
    }
    else
    {
        allocWrapper(MAX_COMMAND_LEN, sizeof(char), (void **)&command); //TODO:
        do
        {
            scanf("%2048s", command);
            printf("report cmd: %s\n", command);
            goOn = processCommand(command, &cs);
        } while (goOn);
    }

    printf("FINE REPORT!\n");
    collectGarbage();
    return 0;
}

void printTable(int start, int finish, char *name, stats resultStats)
{
    //check se la sezione è vuota
    int j = 0;
    int check = 0;
    for (j = start; j < finish; j++)
    {
        if (resultStats.frequencies[j] != 0)
        {
            check = 1;
            j = finish;
        }
    }
    if (check != 0)
    {
        printf("\n %s\n", name); //prints the name of the section
        int k = start;           //k = counter for the ascii characters
        int max = 0;             //length of the biggest number
        int dim = 0;             //dimension of the "current" number
        int search = 0;          //i,j,k were already used

        //search for the biggest number in frequencies from position START to FINISH
        for (search = start; search < finish; search++)
        {
            if (resultStats.frequencies[search] > max)
            {
                max = resultStats.frequencies[search];
            }
        }

        const int dimMax = getDigits(max); //save the digits of the biggest number

        int i = 0;

        //CHARACTERS
        do
        {
            if (resultStats.frequencies[k] != 0)
            {
                dim = getDigits(resultStats.frequencies[k]); //digits of the current number

                //first space
                printf(" ");

                if (dim % 2 == 0) //even number of digits
                {
                    for (i = 0; i < (dimMax / 2) - 1; i++) //prints spaces to the left of the CHARACTER
                        printf(" ");
                    printf("%c", k);
                    for (i = dimMax / 2; i < dimMax; i++) //prints spaces to the right of the CHARACTER
                        printf(" ");
                }
                else if (dim % 2 != 0 && dimMax % 2 == 0) //odd number of digits
                {
                    for (i = 0; i < (dimMax / 2) - 1; i++) //prints spaces to the left of the CHARACTER
                        printf(" ");
                    printf("%c", k);
                    for (i = (dimMax / 2); i < dimMax; i++) //prints spaces to the right of the CHARACTER
                        printf(" ");
                }
                else if (dim % 2 != 0 && dimMax % 2 != 0) //odd number of digits
                {
                    for (i = 0; i < (dimMax / 2); i++) //prints spaces to the left of the CHARACTER
                        printf(" ");
                    printf("%c", k);
                    for (i = (dimMax / 2) + 1; i < dimMax; i++) //prints spaces to the right of the CHARACTER
                        printf(" ");
                }

                //last space
                if ((dimMax - dim) % 2 == 0)
                    printf(" ");
                printf("|");
            }
            k++;
        } while (k < finish);

        printf("\n");

        // NUMERI
        k = start;
        dim = 0;
        do
        {

            if (resultStats.frequencies[k] != 0)
            {
                dim = getDigits(resultStats.frequencies[k]);

                //first space
                printf(" ");

                if ((dimMax - dim) % 2 == 0)
                {
                    for (i = 0; i < (dimMax - dim) / 2; i++)
                        printf(" ");
                    printf("%d", resultStats.frequencies[k]);
                    for (i = 0; i < (dimMax - dim) / 2; i++)
                        printf(" ");
                }
                else if ((dimMax - dim) % 2 != 0)
                {
                    for (i = 0; i < (dimMax - dim) / 2; i++)
                        printf(" ");
                    printf("%d", resultStats.frequencies[k]);
                    for (i = 0; i < (dimMax - dim) / 2 + 1; i++)
                        printf(" ");
                }

                //last space
                if ((dimMax - dim) % 2 == 0)
                    printf(" ");
                printf("|");
            }
            k++;
        } while (k < finish);

        printf("\n");

        max = 0;
        //search for the biggest number in frequencies
        for (search = 33; search < 127; search++){
            if (resultStats.frequencies[search] > max){
                max = resultStats.frequencies[search]; 
            }
        }

        const int maximum = getDigits(max); //save the digits of the biggest number
        
        printf("\n");
        j = 0;
        for (j = 0; j < maximum * 6; j++)
        {
            //for(i = 0; i < 1; i++)
                printf(" ");
            //for(i = 0; i < 7; i++)
                printf("-"); // Prints a line of -
        }
        printf("\n");
    }
}

void print(stats resultStats)
{
    int j = 0;
    int check = 0;
    for (j = 0; j < 256; j++)
    {
        if (resultStats.frequencies[j] != 0)
        {
            check = 1;
            j = 256;
        }
    }
    if (check == 0)
    {
        printf("Empty file\n");
        exit(0);
    }

    printf("\nNumber of tabs: %d", resultStats.frequencies[9]);
    printf("\nNumber of new lines: %d", resultStats.frequencies[11]);
    printf("\nNumber of spaces: %d", resultStats.frequencies[32]);
    printf("\nNumber of delete: %d \n", resultStats.frequencies[127]);

    int pos = 0;
    switch(align(33, 48, resultStats, 2))
    {
        case 0:
            printTable(33, 39, "Punctuation", resultStats);
            printTable(40, 48, "Punctuation", resultStats);
            break;
        case 1:
            printTable(33, 48, "Punctuation", resultStats);
            break;
        default:
            break;
            
    }

    printTable(48, 58, "Numbers", resultStats);
    printTable(58, 65, "Operators", resultStats);

    switch(align(65, 91, resultStats, 3))
    {
        case 0:
            printTable(65, 73, "Uppercase letters", resultStats);
            printTable(73, 82, "Uppercase letters", resultStats);
            printTable(82, 91, "Uppercase letters", resultStats); 
            break;
        case 1:
            printTable(65, 91, "Uppercase letters", resultStats);
            break;
        case 2:
            pos = position(65, 91, resultStats);
            printTable(65, pos, "Uppercase letters", resultStats);
            printTable(pos, 91, "Uppercase letters", resultStats);
            break;
        default:
            break;
    }
    
    printTable(91, 97, "Symbols", resultStats);

    switch(align(97, 123, resultStats, 3))
    {
        case 0:
            printTable(97, 105, "Lowercase letters", resultStats);
            printTable(105, 114, "Lowercase letters", resultStats);
            printTable(114, 123, "Lowercase letters", resultStats);
            break;
        case 1:
            printTable(97, 123, "Lowercase letters", resultStats);
            break;
        case 2:
            pos = position(97, 123, resultStats);
            printTable(97, pos, "Lowercase letters", resultStats);
            printTable(pos, 123, "Lowercase letters", resultStats);
            break;
        default:
            break;
    }

    
    printTable(123, 127, "Other characters", resultStats); //some are divided in multiple row for style sake

}

int position(int start, int finish, stats resultStats)
{
    int pos = start;
    int count = 0;
    int stop = 0;
    int i;
    for(i = start; i < finish; i++)
    {
        if(resultStats.frequencies[i] != 0)
            count++;
    }


    int s = start;

    while(stop < count/2 + 1)
    {
        if(resultStats.frequencies[s] != 0)
        {
            stop++;
            pos = s;
        }

        s++;
    }
    return pos;
}


/*
    return -1 : errore
    return 0 : print as much lines as "def"
    return n : number of lines to print
*/
int align(int start, int finish, stats resultStats, int def)
{
    int i = 0;
    int c = 0;
    for(i = start; i < finish; i++)
    {
        if(resultStats.frequencies[i] != 0)
            c++;
    }

    if(c == 0)
        return -1;
    
    //every character appears in the file
    if(c == finish - start)
        return 0;

    if(def == 3)
    {
        //x is used to check if count > (2/3)def --> if so, classical print in print function
        float x = (2/3) * def;

        //print 3 lines
        if((float)c > 18)
            return 0;

        if((float)c > 9)
            return 2;
        
        return 1;
    }

    if(def == 2)
    {
        //print 2 lines
        if((float)c > 9)
            return 0;

        return 1;
    }


}