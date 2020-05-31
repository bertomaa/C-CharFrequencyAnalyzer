#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
int arrayFrequencies[256]; //could be 126
void printTable(int, int, char *, int *);

int getDigits(int n)
{
    int count = 0;
        do 
    {
        /* Increment digit count */
    count++;

        /* Remove last digit of 'num' */
        n /= 10;
    } while(n != 0);
    return count;
}

int main(int argc, char **argv)
{
    srand(time(0));
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    printf("lines %d\n", w.ws_row);
    printf("columns %d\n", w.ws_col);
    int j;
    int check = 0;
    for (j = 0; j < 256; j++)
    {
        // arrayFrequencies[j] = j;
        arrayFrequencies[j] = 1 + rand() % 1000000;
    }
    
    for (j = 0; j < 256; j++)
    {
        if(arrayFrequencies[j] != 0)
        {
            check = 1;
            j = 256;
        }
    }
    if(check == 0)
    {
        printf("Empty file\n");
        exit(0);
    }
    
    printTable(32, 39, "Punctuation", arrayFrequencies);
    printTable(40, 48, "Punctuation", arrayFrequencies);
    printTable(48, 58, "Numbers", arrayFrequencies);
    printTable(58, 65, "Operators", arrayFrequencies);
    printTable(65, 73, "Uppercase letters", arrayFrequencies);
    printTable(73, 82, "Uppercase letters", arrayFrequencies);
    printTable(82, 91, "Uppercase letters", arrayFrequencies); 
    printTable(91, 97, "Symbols", arrayFrequencies);
    printTable(97, 105, "Lowercase letters", arrayFrequencies);
    printTable(105, 114, "Lowercase letters", arrayFrequencies);
    printTable(114, 123, "Lowercase letters", arrayFrequencies);
    printTable(123, 128, "Other characters", arrayFrequencies); //some are divided in multiple row for style sake

    return 0;
}

void printTable(int start, int finish, char *name, int arrayFrequencies[256])
{
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

    printf("\n %s\n", name); //prints the name of the section
    int k = start; //k = counter for the ascii characters
    int max = 0; //length of the biggest number
    int dim = 0; //dimension of the "current" number
    int search = 0; //i,j,k were already used
    for (search = 0; search < 256; search++){
        if (arrayFrequencies[search] > max){
            max = arrayFrequencies[search]; //search for the biggest number in frequencies
        }
    }
    //printf("\n %d \n", max);
    const int dimMax = getDigits(max); //save the digits of the biggest number
    // int dim = getDigits(arrayFrequencies[k]);
    int i = 0;
    
    //CHARACTERS
    do
    {

        if (arrayFrequencies[k] != 0)
        {
            dim = getDigits(arrayFrequencies[k]); //digits of the current number
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

        if (arrayFrequencies[k] != 0)
        {
            dim = getDigits(arrayFrequencies[k]);
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
                printf("%d", arrayFrequencies[k]);
                for (i = (((dimMax - dim) / 2) + dim); i < dimMax ; i++)
                    printf(" ");
            }
            else
            {
                for (i = 0; i < (dimMax - dim)/ 2; i++)
                    printf(" ");
                printf("%d", arrayFrequencies[k]);
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
