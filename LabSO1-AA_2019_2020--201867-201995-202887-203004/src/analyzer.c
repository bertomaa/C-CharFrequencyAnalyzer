/*
    Questo file deve contenere l'analyzer: il processo principale crea n sottoprocessi, ognuno di questi crea m sottoprocessi che analizzano i file.
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


int checkArguments(int argc, const char* argv[])
{
    if(argc < 4)
    {
        printf("Wrong arguments, usage is: analyzer <n> <m> <file1> <file2> ... \n");
        return 1;
    }
    return 0;
}

//processo q, legge la parte dei files a lui assegnata
int q(int parte, int m, const char* files[])
{
    //TODO: scrivi
}

//processo p, genera m processi figli q e assegna loro le parti di file da analizzare
int p(int m, int filesCount, const char* files[])
{
    //TODO: scrivi
    printf("I must analize files:\n");
    int i;
    for(i = 0; i < filesCount; i++)
    {
        printf("%s\n", files[i]);
    }
    return 0;
}

//dati passati in input: n, m, nomefile1, nomefile2,...
int main(int argc, const char* argv[])
{
    if(checkArguments(argc, argv) != 0)
        exit(1);

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);

    //TODO: se un nomefile è una cartella allora trova 

    //TODO: dividi i file tra i processi p

    pid_t* pids = (int*) malloc(n * sizeof(int));
    int pid;

    //crea i sottoprocessi p
    int i;
    for(i = 0; i < n; i++)
    {
        pid = fork();
        if(pid < 0)
        {
            printf("Error in fork, exit");
            exit(2);
        }
        else if(pid == 0)    //figlio
        {
            int ret = p(m, argc - 3, argv + 3);
            return ret;
        }
        else                //padre
            pids[i] = pid;
    }

    //padre
    for(i = 0; i < n; i++)
    {
        wait(pids[i]);
    }

    return 0;
}