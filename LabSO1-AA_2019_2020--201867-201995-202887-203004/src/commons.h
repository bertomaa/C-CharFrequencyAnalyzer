#ifndef __COMMONS_H__
#define __COMMONS_H__

#define MAX_COMMAND_LEN 1024
#define PRE_FILES_ARGS 3
#define MAX_PATH_CHARACTERS 1024
#define MAX_PATH_LEN 1024
#define INITIAL_CONFIG_SIZE 128
#define READ 0
#define WRITE 1
#define ASCII_CHARACTERS 256
#define MAX_PIPE_CHARACTERS 8128

int getPipeIndex(int index, int type)
{
    return index * 2 + type;
}

#endif