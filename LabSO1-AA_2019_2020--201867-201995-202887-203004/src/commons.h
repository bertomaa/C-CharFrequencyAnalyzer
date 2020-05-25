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

typedef struct garbageCollector
{
    int dim;
    int garbageCount;
    void **garbage;
} garbageCollector;

garbageCollector gc;

int initGC()
{
    gc.dim = 100;
    gc.garbageCount = 0;
    gc.garbage = (void **)calloc(gc.dim, sizeof(void *));
    if (gc.garbage == NULL)
    {
        fprintf(stderr, "Cannot allocate garbage collector in memory\n");
        return 1;
    }
    return 0;
}

void addToGC(void *garbage)
{
    int i = 0;
    for (i = 0; i < gc.garbageCount; i++)
    {
        if(gc.garbage[i] == garbage)
        {
            printf("Sto aggiungento di nuovo lo stesso!");
            return;
        }
    }
    if (gc.garbageCount == gc.dim - 10)
    {
        gc.dim += 100;
        gc.garbage = realloc(gc.garbage, gc.dim);
    }
    gc.garbage[gc.garbageCount] = garbage;
    gc.garbageCount++;
}

void removeFromGC(void*p){
    int i;
    for (i = 0; i < gc.garbageCount; i++)
    {
        if(gc.garbage[i] == p)
        {
            gc.garbage[i] = gc.garbage[gc.garbageCount-1];
            gc.garbageCount--;
            return;
        }
    }
}

void collectGarbage()
{
    int i;
    for (i = gc.garbageCount - 1; i > 0; i--)
    {
        // printf("%d: deleting %p\n", i, gc.garbage[i]);
        free(gc.garbage[i]);
    }
    free(gc.garbage);
    printf("freed %d elements in memory\n", gc.garbageCount);
}

char *addDoubleQuotes(char *buffer, char *path)
{
    buffer[0] = '\0';
    strcat(buffer, "\"");
    strcat(buffer, path);
    strcat(buffer, "\"");
    return buffer;
}

char *removeDoubleQuotes(char *buffer, char *path)
{
    int i;
    for (i = 0; i < strlen(path) - 2; i++)
    {
        buffer[i] = path[i + 1];
    }
    buffer[i] = '\0';
    return buffer;
}

char *getCommandOutput(const char *cmd)
{
    //printf("cmd: %s\n", cmd);
    //TODO:dite che ce lo lascia usare?
    char cmdBuffer[MAX_COMMAND_LEN];
    char *ret;
    int size = 0;
    int error = allocWrapper(MAX_PIPE_CHARACTERS, sizeof(char), (void **)&ret);
    if (error)
    {
        //TODO: gestisci errore
        exit(1);
    }
    FILE *fp = popen(cmd, "r");
    if (fp == NULL)
    {
        //TODO: gestisci errore
        exit(1);
    }
    while (fgets(cmdBuffer, MAX_PIPE_CHARACTERS, fp) != NULL)
    {
        size += strlen(cmdBuffer) + 1;
        if (size >= MAX_PIPE_CHARACTERS) //TODO: gestisci errore, stringa più lunga del massimo
            exit(1);
        strcat(ret, cmdBuffer);
    };
    return ret;
}

int getPipeIndex(int index, int type)
{
    return index * 2 + type;
}

char* splitString(char *buffer, char **str, char delimiter)
{
    //printf("buffer: %s, str: %s, delimiter: %c", buffer, *str, delimiter);
    int i;
    for (i = 0; i < strlen(*str) && (*str)[i] != delimiter; i++)
    {
        buffer[i] = (*str)[i];
    }

    //printf("buffer: %s, str: %s, delimiter: %c", buffer, *str, delimiter);
    buffer[i] = '\0';
    if ((*str)[i] == delimiter)
    {
        *str = *str + i + 1;
        return buffer;
    }
    return NULL;
}

#endif