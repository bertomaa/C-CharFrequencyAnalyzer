#include <iostream>
#include "stats.h"
#include "wrapper.h"
#include <stdio.h>
#include <cstring>

using namespace std;

stats* leggiFile()
{
    char* s;
    stats *ret = new stats();
    initStats(ret);
    int fd = open("divina_commedia_ridotta.txt", O_RDONLY);
    allocWrapper(10000, sizeof(char), (void**)&s);
    read(fd, s, 10000);
    for(int i = 0; s[i] != EOF; i++)
        ret->frequencies[s[i]]++;
    return ret;
}

stats* leggiRes()
{
    char* s;
    stats* ret = new stats();
    initStats(ret);
    int fd = open("stats.txt", O_RDONLY);
    allocWrapper(10000, sizeof(char), (void**)&s);
    read(fd, s, 10000);
    decode(s, ret);
    return ret;
}

int main()
{
    stats* myCount = leggiFile();
    stats* hisCount = leggiRes();
    int i;
    for(i = 0; i < 255; i++)
    {
        if(myCount->frequencies[i] != hisCount->frequencies[i])
            cout << "Errore carattere " << i << endl;
        else
            cout << "giusto";
    }
}