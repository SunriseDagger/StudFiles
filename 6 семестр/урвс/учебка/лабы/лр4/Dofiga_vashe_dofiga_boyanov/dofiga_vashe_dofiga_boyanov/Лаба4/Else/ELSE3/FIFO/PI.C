#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#define FIFO "fifo.1"

void OnSignal (){;}

int main (int argc, char *argv[]) {
    float pi;
    int fd;						// pipe write discriptor

    signal(SIGINT,OnSignal);
    pause();
    
//    if(mkfifo(FIFO,0666)<0) printf("\n pi: %s already created.",FIFO);
    if((fd=open(FIFO,O_WRONLY))<0) printf("\n pi: can't open FIFO");
    
    printf("\n pi: fd=%d",fd);
    pi=4.-4./3.+4./5.-4./7.+4./9.-4./11.+4./13.;
    write(fd,&pi,sizeof(pi));				// write pi into pipe
    close(fd);
//    unlink(FIFO);
}


