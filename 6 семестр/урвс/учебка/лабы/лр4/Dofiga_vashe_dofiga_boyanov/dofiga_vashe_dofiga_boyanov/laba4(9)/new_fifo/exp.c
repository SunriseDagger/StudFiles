#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

void OnSignal (){;}
int main () {
    float x,exp;
    int pd[2];						// pipe r/w discriptor

//    printf("\n exp: i'm waiting...");
//    signal(SIGINT, OnSignal);
//    pause();

    pd[0]=open("fifo.1",O_RDONLY);
    read(pd[0],&x,sizeof(x));				// read x from pipe
//    printf("\n exp: x=%f",x);    
    close(pd[0]);

    exp=1-x*x/2+x*x*x*x/8;
    pd[1]=open("fifo.1",O_WRONLY);   
    write(pd[1],&exp,sizeof(exp));			// write exp into pipe
//    printf("\n exp: exp=%f",exp);         
    close(pd[1]);
//    kill(getppid(),SIGINT);
}


