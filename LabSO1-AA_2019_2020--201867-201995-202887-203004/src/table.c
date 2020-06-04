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
    arrayFrequencies[35] = 1;
    arrayFrequencies[99] = 15;
    arrayFrequencies[105] = 105;
    arrayFrequencies[113] = 1133;
    for (j = 48; j < 58; j++)
    {
        // arrayFrequencies[j] = j;
        arrayFrequencies[j] = 0;
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
    //check se la sezione è vuota
    int j = 0;
    int check = 0;
    for (j = start; j < finish; j++)
    {
        if(arrayFrequencies[j] != 0)
        {
            check = 1;
            j = finish;
        }
    }
    if(check != 0)
    {
        
        printf("\n %s\n", name); //prints the name of the section
        int k = start; //k = counter for the ascii characters
        int max = 0; //length of the biggest number
        int dim = 0; //dimension of the "current" number
        int search = 0; //i,j,k were already used

        //search for the biggest number in frequencies from position START to FINISH
        for (search = start; search < finish; search++){
            if (arrayFrequencies[search] > max){
                max = arrayFrequencies[search]; 
            }
        }

        const int dimMax = getDigits(max); //save the digits of the biggest number

        int i = 0;
        
        //CHARACTERS
        do
        {
            if (arrayFrequencies[k] != 0)
            {
                dim = getDigits(arrayFrequencies[k]); //digits of the current number

                //first space
                printf(" ");

                if(dimMax == 2)
                {
                    if(dim == 2)
                        printf("%c", k);
                    if(dim == 1)
                    {
                        printf("%c", k);
                        printf(" ");
                    }
                }

                else if(dimMax == 3)
                {
                    if(dim == 3)
                    {
                        printf("%c", k);
                    }
                    if(dim == 2)
                    {
                        printf("%c", k);
                        printf(" ");
                    }
                    if(dim == 1)
                    {
                        printf(" ");
                        printf("%c", k);
                        printf(" ");
                    }
                }

                else if (dim % 2 == 0) //even number of digits
                {
                    for (i = 0; i < (dimMax / 2) - 1; i++) //prints spaces to the left of the CHARACTER
                        printf(" ");
                    printf("%c", k);
                    for (i = dimMax / 2; i < dimMax; i++) //prints spaces to the right of the CHARACTER
                        printf(" ");
                }
                else //odd number of digits
                {
                    for (i = 0; i < (dimMax / 2) - 1; i++) //prints spaces to the left of the CHARACTER
                        printf(" ");
                    printf("%c", k);
                    for (i = (dimMax / 2); i < dimMax; i++) //prints spaces to the right of the CHARACTER
                        printf(" ");
                }



                //last space
                if((dimMax - dim) % 2 == 0)
                    printf(" ");
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
                
                //first space
                printf(" ");

                if(dimMax == 2)
                {
                    if(dim == 2)
                        printf("%c", k);
                    if(dim == 1)
                    {
                        printf("%c", k);
                        printf(" ");
                    }
                }

                else if(dimMax == 3)
                {
                    if(dim == 3)
                    {
                        printf("%c", k);
                    }
                    if(dim == 2)
                    {
                        printf("%c", k);
                        printf(" ");
                    }
                    if(dim == 1)
                    {
                        printf(" ");
                        printf("%c", k);
                        printf(" ");
                    }
                }

                else if ((dimMax - dim) % 2 == 0)
                {
                    for(i = 0; i < (dimMax - dim)/2; i++)
                        printf(" ");
                    printf("%d", arrayFrequencies[k]);
                    for(i = 0; i < (dimMax - dim)/2; i++)
                        printf(" ");
                }
                else if((dimMax - dim) % 2 != 0)
                {
                    for (i = 0; i < (dimMax - dim)/ 2; i++)
                        printf(" ");
                    printf("%d", arrayFrequencies[k]);
                    for (i = 0; i < (dimMax - dim)/ 2 + 1; i++)
                        printf(" ");
                }

                //last space
                if((dimMax - dim) % 2 == 0)
                    printf(" ");
                printf("|");
            }
            k++;
        } while (k < finish);

        printf("\n");
        
        printf("\n");
        j = 0;
        for (j = 0; j < dimMax*6; j++)
        {
            //for(i = 0; i < 1; i++)
                printf(" ");
            //for(i = 0; i < 7; i++)
                printf("-"); // Prints a line of -
        }
        printf("\n");
    }
}
