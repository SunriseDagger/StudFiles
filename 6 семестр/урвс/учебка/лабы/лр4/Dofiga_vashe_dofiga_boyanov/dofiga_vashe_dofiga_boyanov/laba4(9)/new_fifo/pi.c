#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

void OnSignal() {;}

int main (int argc, char *argv[]) {
    float pi;
    int pd;						// pipe write discriptor

//    printf("\n pi: i'm waiting...");
//    signal(SIGINT, OnSignal);
//    pause();
    pd=open("fifo.1",O_WRONLY);

    pi=4.-4./3.+4./5.-4./7.+4./9.-4./11.+4./13.;
    write(pd,&pi,sizeof(pi));				// write pi into pipe
//    printf("\n pi: pi=%f",pi);
    close(pd);
//    kill(getppid(),SIGINT);    
}


