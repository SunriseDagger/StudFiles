#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

void OnSignal (){;}

int main (int argc, char *argv[]) {
    float pi;
    int pd;						// pipe write discriptor

    sscanf(argv[0],"%d",&pd);
    signal(SIGINT, OnSignal);
    pause();

    pi=4.-4./3.+4./5.-4./7.+4./9.-4./11.+4./13.;
    write(pd,&pi,sizeof(pi));				// write pi into pipe
}


