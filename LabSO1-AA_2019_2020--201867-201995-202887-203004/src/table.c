#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
int arrayFrequencies[256]; //could be 126
void printTable(int, int, char *, int *);

int getDigits(int n)
{
    if (n == 0)
        return 1;
    int count = 0;
    while (n != 0)
    {
        n /= 10;
        ++count;
    }
    return count;
}

int position(int start, int finish, int arrayFrequencies[256])
{
    int pos = start;
    int count = 0;
    int stop = 0;
    int i;
    for(i = start; i < finish; i++)
    {
        if(arrayFrequencies[i] != 0)
            count++;
    }


    int s = start;

    while(stop < count/2 + 1)
    {
        if(arrayFrequencies[s] != 0)
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
int align(int start, int finish, int arrayFrequencies[256], int def)
{
    int i = 0;
    int c = 0;
    for(i = start; i < finish; i++)
    {
        if(arrayFrequencies[i] != 0)
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
        //arrayFrequencies[j] = j;
        arrayFrequencies[j] = 1 + rand() % 1000;
    }

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
    
    printf("\nNumber of tabs: %d", arrayFrequencies[9]);
    printf("\nNumber of new lines: %d", arrayFrequencies[11]);
    printf("\nNumber of spaces: %d", arrayFrequencies[32]);
    printf("\nNumber of delete: %d \n", arrayFrequencies[127]);

    int pos = 0;
    switch(align(33, 48, arrayFrequencies, 2))
    {
        case 0:
            printTable(33, 39, "Punctuation", arrayFrequencies);
            printTable(40, 48, "Punctuation", arrayFrequencies);
            break;
        case 1:
            printTable(33, 48, "Punctuation", arrayFrequencies);
            break;
        default:
            break;
            
    }

    printTable(48, 58, "Numbers", arrayFrequencies);
    printTable(58, 65, "Operators", arrayFrequencies);

    switch(align(65, 91, arrayFrequencies, 3))
    {
        case 0:
            printTable(65, 73, "Uppercase letters", arrayFrequencies);
            printTable(73, 82, "Uppercase letters", arrayFrequencies);
            printTable(82, 91, "Uppercase letters", arrayFrequencies); 
            break;
        case 1:
            printTable(65, 91, "Uppercase letters", arrayFrequencies);
            break;
        case 2:
            pos = position(65, 91, arrayFrequencies);
            printTable(65, pos, "Uppercase letters", arrayFrequencies);
            printTable(pos, 91, "Uppercase letters", arrayFrequencies);
            break;
        default:
            break;
    }
    
    printTable(91, 97, "Symbols", arrayFrequencies);

    switch(align(97, 123, arrayFrequencies, 3))
    {
        case 0:
            printTable(97, 105, "Lowercase letters", arrayFrequencies);
            printTable(105, 114, "Lowercase letters", arrayFrequencies);
            printTable(114, 123, "Lowercase letters", arrayFrequencies);
            break;
        case 1:
            printTable(97, 123, "Lowercase letters", arrayFrequencies);
            break;
        case 2:
            pos = position(97, 123, arrayFrequencies);
            printTable(97, pos, "Lowercase letters", arrayFrequencies);
            printTable(pos, 123, "Lowercase letters", arrayFrequencies);
            break;
        default:
            break;
    }

    
    printTable(123, 127, "Other characters", arrayFrequencies); //some are divided in multiple row for style sake

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
                //if (k == 9 || k==11)
                   // k = 32;

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
                else if (dim % 2 != 0 && dimMax % 2 != 0)//odd number of digits
                {
                    for (i = 0; i < (dimMax / 2); i++) //prints spaces to the left of the CHARACTER
                        printf(" ");
                    printf("%c", k);
                    for (i = (dimMax / 2) + 1; i < dimMax; i++) //prints spaces to the right of the CHARACTER
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
        do
        {

            if (arrayFrequencies[k] != 0)
            {
                dim = getDigits(arrayFrequencies[k]);
                
                //first space
                printf(" ");

                if ((dimMax - dim) % 2 == 0)
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

        max = 0;
        //search for the biggest number in frequencies
        for (search = 33; search < 127; search++){
            if (arrayFrequencies[search] > max){
                max = arrayFrequencies[search]; 
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