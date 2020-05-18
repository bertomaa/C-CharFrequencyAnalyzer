#include <stdio.h>
#include "wrapper.h"
#include "stats.h"
#include "config.h"
#include "commons.h"

int arrayFrequencies[256];
void printTable(int, int, char *, int *);


int main()
{
    int j;
    for (j = 0; j < 256; j++)
    {
        arrayFrequencies[j] = j;
    }
    printTable(32, 48, "\n\nPunctuation:", arrayFrequencies);
    printTable(48, 58, "\n\nNumbers:", arrayFrequencies);
    printTable(58, 65, "\n\nOperators:", arrayFrequencies);
    printTable(65, 78, "\n\nUppercase letters:", arrayFrequencies);
    printTable(78, 91, "", arrayFrequencies); //letters are divided in two rows for style's sake
    printTable(91, 97, "\n\nSymbols:", arrayFrequencies);
    printTable(97, 110, "\n\nLowercase letters:", arrayFrequencies);
    printTable(110, 123, "", arrayFrequencies);
    printTable(123, 128, "\n\nLast characters:", arrayFrequencies); //letters are divided in two rows for style's sake

    return 0;
}

void printTable(int start, int finish, char *name, int arrayFrequencies[256])
{

    printf("%s\n", name);
    int i = start;
    do
    {
        printf("%c\t", i); //prints the ascii character
        i++;
    } while (i < finish);

    printf("\n");

    for (i = start; i < finish; i++)
    {
        printf("%d\t", arrayFrequencies[i]); //prints its frequency
    }
    printf("\n");
    /*for (int j = 0; j < 130; j++)
    {
        printf("_");
    }
    printf("%s\n", name);
    int i = start;
    do
    {
        printf("%c     |    ", i);
        i++;
    } while (i < finish);

    printf("\n");

    for (int i = start; i < finish; i++)
    {
        printf("%d    |    ", i);
    }
    printf("\n");*/
}

/* printf("\n\nPunteggiatura:\n");
    int i = 32;
    do
    {
        printf("%c\t", i);
        i++;
    } while (i < 48);

    printf("\n");

    for (int i = 32; i < 48; i++)
    {
        printf("%d\t", i);
    }

    printf("\n\nNumeri:\n");
    i = 48;
    do
    {
        printf("%c\t", i);
        i++;
    } while (i < 58);

    printf("\n");

    for (int i = 48; i < 58; i++)
    {
        printf("%d\t", i);
    }

    printf("\n\nSimboli:\n");
    i = 58;
    do
    {
        printf("%c\t", i);
        i++;
    } while (i < 65);

    printf("\n");

    for (int i = 58; i < 65; i++)
    {
        printf("%d\t", i);
    }

    printf("\n\nLettere maiuscole:\n");
    i = 65;
    do
    {
        printf("%c\t", i);
        i++;
    } while (i < 78);

    printf("\n");

    for (int i = 65; i < 78; i++)
    {
        printf("%d\t", i);
    }

    printf("\n");
    i = 78;
    do
    {
        printf("%c\t", i);
        i++;
    } while (i < 91);

    printf("\n");

    for (int i = 78; i < 91; i++)
    {
        printf("%d\t", i);
    }

    printf("\n\nSegni:\n");
    i = 91;
    do
    {
        printf("%c\t", i);
        i++;
    } while (i < 97);

    printf("\n");

    for (int i = 91; i < 97; i++)
    {
        printf("%d\t", i);
    }

    printf("\n\nLettere minuscole:\n");
    i = 97;
    do
    {
        printf("%c\t", i);
        i++;
    } while (i < 110);

    printf("\n");

    for (int i = 97; i < 110; i++)
    {
        printf("%d\t", i);
    }

    printf("\n");
    i = 110;
    do
    {
        printf("%c\t", i);
        i++;
    } while (i < 123);

    printf("\n");

    for (int i = 110; i < 123; i++)
    {
        printf("%d\t", i);
    }

    printf("\n\nUltimi caratteri:\n");
    i = 123;
    do
    {
        printf("%c\t", i);
        i++;
    } while (i < 128);

    printf("\n");

    for (int i = 123; i < 128; i++)
    {
        printf("%d\t", i);
    }
    printf("\n");*/