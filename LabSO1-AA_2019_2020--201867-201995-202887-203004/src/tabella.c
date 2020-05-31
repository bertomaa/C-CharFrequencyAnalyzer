#include <stdio.h>
#include "stats.h"

int array[256];
int main()
{
    int j;
    for (j = 0; j < DIM; j++)
    {
        array[j] = j;
    }

    printf("\n\nPunteggiatura:\n");
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
    printf("\n");
    return 0;
}