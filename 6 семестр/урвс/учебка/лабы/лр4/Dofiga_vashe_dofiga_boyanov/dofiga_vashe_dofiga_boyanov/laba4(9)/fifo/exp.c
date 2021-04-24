#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#define FIFO "fifo.1"

void OnSignal (){;}

int main (int argc, char *argv[]) {
    float x,exp;
    int fd;						// pipe r/w discriptor

    signal(SIGINT, OnSignal);
    pause();
//    if(mkfifo(FIFO,0666)<0) printf("\n exp: %s already created.",FIFO);
//    fifo_r=open(FIFO,O_RDONLY);
    if((fd=open(FIFO,O_RDWR))<0) printf("\n exp: can't open FIFO");
    printf("\n exp: fd=%d",fd);    
    read(fd,&x,sizeof(x));				// read x from pipe
    exp=1-x*x/2+x*x*x*x/8;
    write(fd,&exp,sizeof(exp));			// write exp into pipe
    close(fd);
//    unlink(FIFO);
}


