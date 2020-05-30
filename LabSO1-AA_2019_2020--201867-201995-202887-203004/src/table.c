#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "stats.h"
#include "commons.h"
#include "config.h"

void printTable(int start, int finish, char *name, int *);

int main(int argc, char **argv)
{
    srand(time(0));
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    stats *stat;
    config conf;

    //int* arrayFrequencies = new int[256];
    int arrayFrequencies[256];

    printf("lines %d\n", w.ws_row);
    printf("columns %d\n", w.ws_col);
    int j;
    for (j = 0; j < 256; j++)
    {
        // arrayFrequencies[j] = j;
        arrayFrequencies[j] = 1 + rand() % 1000;
    }
    int i = 0;
    for(i = 0; i < ASCII_CHARACTERS; i++)
    {
        arrayFrequencies[i] = stat->frequencies[i];
    }

    printf("FILE %s\n", conf.files[stat->fileID]);

    return 0;
}

void printTable(int start, int finish, char *name, int *arrayFrequencies)
{
    int cont = 0; //counts all the characters written to have  adotted line of the same leght

    /*printf("%s\n", name); //prints the name of the section
    int i = start;
    do
    {
        printf("%c\t", i); //prints the ascii character
        i++;
    } while (i < finish);

    printf("\n");

    for (int i = start; i < finish; i++)
    {
        printf("%d\t", arrayFrequencies[i]); //prints its frequency
    }
    printf("\n");*****/

    printf("%s\n", name); //prints the name of the section
    int k = start; //k = counter for the ascii characters
    do
    {
        printf("  "); //2 spaces before the letter/symbol
        cont = cont + 2;

        printf("%c", k); //prints the ascii character
        cont++;

        printf("  "); //2 spaces after the letter/symbol
        cont = cont + 2;

        printf("|");
        cont++;

        k++;
    } while (k < finish);

    printf("\n");

    for (int i = start; i < finish; i++)
    {
        if (arrayFrequencies[i] < 10)
        {
            printf("  ");
        }
        if (arrayFrequencies[i] < 100 && arrayFrequencies[i] >= 10)
        {
            printf("  ");
        }
        if (arrayFrequencies[i] < 1000 && arrayFrequencies[i] >= 100)
        {
            printf(" ");
        }
        printf("%d", arrayFrequencies[i]); //prints its frequency
        if (arrayFrequencies[i] < 10)
        {
            printf("  ");
        }
        if (arrayFrequencies[i] < 100 && arrayFrequencies[i] >= 10)
        {
            printf(" ");
        }
        if (arrayFrequencies[i] < 1000 && arrayFrequencies[i] >= 100)
        {
            printf(" ");
        }
        printf("|");
    }
    printf("\n");
    for (int j = 0; j < cont; j++)
    {
        printf("-"); // Prints a line of -
    }
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